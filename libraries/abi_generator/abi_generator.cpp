#include <graphene/abi_generator/abi_generator.hpp>

namespace graphene {

void abi_generator::set_abi_context(const string& abi_context) {
  this->abi_context = abi_context;
}

void abi_generator::set_compiler_instance(CompilerInstance& compiler_instance) {
  this->compiler_instance = &compiler_instance;
}

bool abi_generator::is_64bit(const string& type) {
  return type_size[type] == 64;
}

bool abi_generator::is_128bit(const string& type) {
  return type_size[type] == 128;
}

bool abi_generator::is_string(const string& type) {
  return type == "String" || type == "string";
}

void abi_generator::get_all_fields(const struct_def& s, vector<field_def>& fields) {
//  abi_serializer abis(*output, fc::seconds(1)); //TODO FIXME why to create a abis instance which no using?

  for(const auto& field : s.fields) {
    fields.push_back(field);
  }

  if(s.base.size()) {
    const auto* base = find_struct(s.base);
    ABI_ASSERT(base, "Unable to find base type ${type}",("type",s.base));
    get_all_fields(*base, fields);
  }
}

bool abi_generator::is_i64_index(const vector<field_def>& fields) {
  return fields.size() >= 1 && is_64bit(fields[0].type);
}

void abi_generator::guess_index_type(table_def& table, const struct_def s) {
  vector<field_def> fields;
  get_all_fields(s, fields);
  if( is_i64_index(fields) ) {
    table.index_type = "i64";
  } else {
    ABI_ASSERT(false, "Unable to guess index type");
  }
}

void abi_generator::guess_key_names(table_def& table, const struct_def s) {

  vector<field_def> fields;
  get_all_fields(s, fields);

 if( table.index_type == "i64") {

    table.key_names.clear();
    table.key_types.clear();

    unsigned int key_size = 0;
    bool valid_key = false;
    for(auto& f : fields) {
      table.key_names.emplace_back(f.name);
      table.key_types.emplace_back(f.type);
      key_size += type_size[f.type]/8;

      if(table.index_type == "i64" && key_size >= sizeof(uint64_t)) {
        valid_key = true;
        break;
      }
    }

    ABI_ASSERT(valid_key, "Unable to guess key names");
  } else {
    ABI_ASSERT(false, "Unable to guess key names");
  }
}

const table_def* abi_generator::find_table(const table_name& name) {
  for( const auto& ta : output->tables ) {
    if(ta.name == name) {
      return &ta;
    }
  }
  return nullptr;
}

const type_def* abi_generator::find_type(const type_name& new_type_name) {
  for( const auto& td : output->types ) {
    if(td.new_type_name == new_type_name) {
      return &td;
    }
  }
  return nullptr;
}

const action_def* abi_generator::find_action(const action_name& name) {
  for( const auto& ac : output->actions ) {
    if(ac.name == name) {
      return &ac;
    }
  }
  return nullptr;
}

const struct_def* abi_generator::find_struct(const type_name& name) {
  auto rname = resolve_type(name);
  for( const auto& st : output->structs ) {
    if(st.name == rname) {
      return &st;
    }
  }
  return nullptr;
}

type_name abi_generator::resolve_type(const type_name& type){
  const auto* td = find_type(type);
  if( td ) {
    for( auto i = output->types.size(); i > 0; --i ) { // avoid infinite recursion
      const type_name& t = td->type;
      td = find_type(t);
      if( td == nullptr ) return t;
    }
  }
  return type;
}

bool abi_generator::is_one_filed_no_base(const string& type_name) {
  const auto* s = find_struct(type_name);
  return s && s->base.size() == 0 && s->fields.size() == 1;
}

string abi_generator::decl_to_string(clang::Decl* d) {
    //ASTContext& ctx = d->getASTContext();
    const auto& sm = ast_context->getSourceManager();
    clang::SourceLocation b(d->getLocStart()), _e(d->getLocEnd());
    clang::SourceLocation e(clang::Lexer::getLocForEndOfToken(_e, 0, sm, compiler_instance->getLangOpts()));
    return string(sm.getCharacterData(b),
        sm.getCharacterData(e)-sm.getCharacterData(b));
}

bool abi_generator::is_typedef(const clang::QualType& qt) {
  return isa<TypedefType>(qt.getTypePtr());
}

bool abi_generator::is_elaborated(const clang::QualType& qt) {
  return isa<ElaboratedType>(qt.getTypePtr());
}

bool abi_generator::is_vector(const clang::QualType& vqt) {

  QualType qt(vqt);

  if ( is_elaborated(qt) )
    qt = qt->getAs<clang::ElaboratedType>()->getNamedType();

  return isa<clang::TemplateSpecializationType>(qt.getTypePtr()) \
    && boost::starts_with( get_type_name(qt, false), "vector");
}

bool abi_generator::is_vector(const string& type_name) {
  return boost::ends_with(type_name, "[]");
}

bool abi_generator::is_struct_specialization(const clang::QualType& qt) {
  return is_struct(qt) && isa<clang::TemplateSpecializationType>(qt.getTypePtr());
}

bool abi_generator::is_struct(const clang::QualType& sqt) {
  clang::QualType qt(sqt);
  const auto* type = qt.getTypePtr();
  return !is_vector(qt) && (type->isStructureType() || type->isClassType());
}

clang::QualType abi_generator::get_vector_element_type(const clang::QualType& qt) {
  const auto* tst = clang::dyn_cast<const clang::TemplateSpecializationType>(qt.getTypePtr());
  ABI_ASSERT(tst != nullptr);
  const clang::TemplateArgument& arg0 = tst->getArg(0);
  return arg0.getAsType();
}

string abi_generator::get_vector_element_type(const string& type_name) {
  if( is_vector(type_name) )
    return type_name.substr(0, type_name.size()-2);
  return type_name;
}

string abi_generator::get_type_name(const clang::QualType& qt, bool with_namespace=false) {
  auto name = clang::TypeName::getFullyQualifiedName(qt, *ast_context);
  if(!with_namespace)
    name = remove_namespace(name);
  return name;
}

clang::QualType abi_generator::add_typedef(const clang::QualType& tqt, size_t recursion_depth) {

  ABI_ASSERT( ++recursion_depth < max_recursion_depth, "recursive definition, max_recursion_depth" );

  clang::QualType qt(get_named_type_if_elaborated(tqt));

  const auto* td_decl = qt->getAs<clang::TypedefType>()->getDecl();
  auto underlying_type = td_decl->getUnderlyingType().getUnqualifiedType();

  auto new_type_name = td_decl->getName().str();
  auto underlying_type_name = get_type_name(underlying_type);

  if ( is_vector(underlying_type) ) {
    underlying_type_name = add_vector(underlying_type, recursion_depth);
  }

  type_def abi_typedef;
  abi_typedef.new_type_name = new_type_name;
  abi_typedef.type = translate_type(underlying_type_name);
  const auto* td = find_type(abi_typedef.new_type_name);

  if(!td && !is_struct_specialization(underlying_type) ) {
    output->types.push_back(abi_typedef);
  } else {
    if(td) ABI_ASSERT(abi_typedef.type == td->type);
  }

  if( is_typedef(underlying_type) && !is_builtin_type(get_type_name(underlying_type)) )
    return add_typedef(underlying_type, recursion_depth);

  return underlying_type;
}

clang::CXXRecordDecl::base_class_range abi_generator::get_struct_bases(const clang::QualType& sqt) {

  clang::QualType qt(sqt);
  if(is_typedef(qt)) {
    const auto* td_decl = qt->getAs<clang::TypedefType>()->getDecl();
    qt = td_decl->getUnderlyingType().getUnqualifiedType();
  }

  const auto* record_type = qt->getAs<clang::RecordType>();
  ABI_ASSERT(record_type != nullptr);
  auto cxxrecord_decl = clang::dyn_cast<CXXRecordDecl>(record_type->getDecl());
  ABI_ASSERT(cxxrecord_decl != nullptr);
  //record_type->getCanonicalTypeInternal().dump();
  ABI_ASSERT(cxxrecord_decl->hasDefinition(), "No definition for ${t}", ("t", qt.getAsString()));

  auto bases = cxxrecord_decl->bases();

  return bases;
}

const clang::RecordDecl::field_range abi_generator::get_struct_fields(const clang::QualType& sqt) {
  clang::QualType qt(sqt);

  if(is_typedef(qt)) {
    const auto* td_decl = qt->getAs<clang::TypedefType>()->getDecl();
    qt = td_decl->getUnderlyingType().getUnqualifiedType();
  }

  const auto* record_type = qt->getAs<clang::RecordType>();
  ABI_ASSERT(record_type != nullptr);
  return record_type->getDecl()->fields();
}

string abi_generator::add_vector(const clang::QualType& vqt, size_t recursion_depth) {

  ABI_ASSERT( ++recursion_depth < max_recursion_depth, "recursive definition, max_recursion_depth" );

  clang::QualType qt(get_named_type_if_elaborated(vqt));

  auto vector_element_type = get_vector_element_type(qt);
  ABI_ASSERT(!is_vector(vector_element_type), "Only one-dimensional arrays are supported");

  add_type(vector_element_type, recursion_depth);

  auto vector_element_type_str = translate_type(get_type_name(vector_element_type));
  vector_element_type_str += "[]";

  return vector_element_type_str;
}

string abi_generator::add_type(const clang::QualType& tqt, size_t recursion_depth) {

  ABI_ASSERT( ++recursion_depth < max_recursion_depth, "recursive definition, max_recursion_depth" );

  clang::QualType qt(get_named_type_if_elaborated(tqt));

  string full_type_name = translate_type(get_type_name(qt, true));
  string type_name      = translate_type(get_type_name(qt));
  bool   is_type_def    = false;

  if( is_builtin_type(type_name) ) {
    return type_name;
  }

  if( is_typedef(qt) ) {
    qt = add_typedef(qt, recursion_depth);
    if( is_builtin_type(translate_type(get_type_name(qt))) ) {
      return type_name;
    }
    is_type_def = true;
  }

  if( is_vector(qt) ) {
    auto vector_type_name = add_vector(qt, recursion_depth);
    return is_type_def ? type_name : vector_type_name;
  }

  if( is_struct(qt) ) {
    return add_struct(qt, full_type_name, recursion_depth);
  }

  ABI_ASSERT(false, "types can only be: vector, struct, class or a built-in type. (${type}) ", ("type",get_type_name(qt)));
  return type_name;
}

clang::QualType abi_generator::get_named_type_if_elaborated(const clang::QualType& qt) {
  if( is_elaborated(qt) ) {
    return qt->getAs<clang::ElaboratedType>()->getNamedType();
  }
  return qt;
}

string abi_generator::add_struct(const clang::QualType& sqt, string full_name, size_t recursion_depth) {

  ABI_ASSERT( ++recursion_depth < max_recursion_depth, "recursive definition, max_recursion_depth" );

  clang::QualType qt(get_named_type_if_elaborated(sqt));

  if( full_name.empty() ) {
    full_name = get_type_name(qt, true);
  }

  auto name = remove_namespace(full_name);

  ABI_ASSERT(is_struct(qt), "Only struct and class are supported. ${full_name}",("full_name",full_name));

  if( find_struct(name) ) {
    auto itr = full_types.find(resolve_type(name));
    if(itr != full_types.end()) {
      ABI_ASSERT(itr->second == full_name, "Unable to add type '${full_name}' because '${conflict}' is already in.\n${t}", ("full_name",full_name)("conflict",itr->second)("t",output->types));
    }
    return name;
  }

  auto bases = get_struct_bases(qt);
  auto bitr = bases.begin();
  int total_bases = 0;

  string base_name;
  while( bitr != bases.end() ) {
    auto base_qt = bitr->getType();
    const auto* record_type = base_qt->getAs<clang::RecordType>();
    if( record_type && is_struct(base_qt) && !record_type->getDecl()->field_empty() ) {
      ABI_ASSERT(total_bases == 0, "Multiple inheritance not supported - ${type}", ("type",full_name));
      base_name = add_type(base_qt, recursion_depth);
      ++total_bases;
    }
    ++bitr;
  }

  struct_def abi_struct;
  for (const clang::FieldDecl* field : get_struct_fields(qt) ) {
    clang::QualType qt = field->getType();

    string field_name = field->getNameAsString();
    string field_type_name = add_type(qt, recursion_depth);

    field_def struct_field{field_name, field_type_name};
    ABI_ASSERT(is_builtin_type(get_vector_element_type(struct_field.type))
      || find_struct(get_vector_element_type(struct_field.type))
      || find_type(get_vector_element_type(struct_field.type))
      , "Unknown type ${type} [${abi}]",("type",struct_field.type)("abi",*output));

    type_size[string(struct_field.type)] = is_vector(struct_field.type) ? 0 : ast_context->getTypeSize(qt);
    abi_struct.fields.push_back(struct_field);
  }

  abi_struct.name = resolve_type(name);
  abi_struct.base = base_name;

  output->structs.push_back(abi_struct);

  full_types[name] = full_name;
  return name;
}

}
