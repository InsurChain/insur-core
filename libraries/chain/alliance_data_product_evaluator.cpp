
#include <graphene/chain/alliance_data_product_evaluator.hpp>
#include <graphene/chain/database.hpp>

namespace graphene { namespace chain {
void_result alliance_data_product_create_evaluator::do_evaluate( const alliance_data_product_create_operation& op )
{ try {
    FC_ASSERT(db().get(op.issuer).is_lifetime_member());

    for (auto& ext : op.extensions) {
        if (ext.which() == future_extensions::tag<pocs_threshold_alliance_data_product_t>::value) {
            uint64_t pocs_threshold = ext.get<pocs_threshold_alliance_data_product_t>().pocs_threshold;
            break;
        }
    }
    return void_result();
} FC_CAPTURE_AND_RETHROW( (op) ) }

object_id_type alliance_data_product_create_evaluator::do_apply(const alliance_data_product_create_operation& op, int32_t billed_cpu_time_us)
{ try {

   const auto& new_object = db().create<alliance_data_product_object>( [&]( alliance_data_product_object& obj ){

           obj.brief_desc = op.brief_desc;
           obj.id = op.id;
           obj.product_name = op.product_name;
           obj.refer_price = op.refer_price;
           obj.status = data_product_undo_status;
           obj.schema_contexts = op.schema_contexts;
           obj.issuer = op.issuer;
           obj.create_date_time = op.create_date_time;
           obj.icon = op.icon;
           obj.total = 0;
           for (auto& ext : op.extensions) {
              if (ext.which() == future_extensions::tag<pocs_threshold_alliance_data_product_t>::value) {
                 obj.pocs_threshold = ext.get<pocs_threshold_alliance_data_product_t>().pocs_threshold;
                 break;
              }
           }

   });
   return  new_object.id;

} FC_CAPTURE_AND_RETHROW( (op) ) }


void_result alliance_data_product_update_evaluator::do_evaluate( const alliance_data_product_update_operation& op )
{ try {
    FC_ASSERT(trx_state->_is_proposed_trx);
    for (auto& ext : op.extensions) {
        if (ext.which() == future_extensions::tag<pocs_threshold_alliance_data_product_t>::value) {
            uint64_t pocs_threshold = ext.get<pocs_threshold_alliance_data_product_t>().pocs_threshold;
            break;
        }
    }
    return void_result();
} FC_CAPTURE_AND_RETHROW( (op) ) }


void_result alliance_data_product_update_evaluator::do_apply(const alliance_data_product_update_operation& op, int32_t billed_cpu_time_us)
{
    try {
        database& _db = db();
        _db.modify(
           _db.get(op.alliance_data_product),
           [&]( alliance_data_product_object& obj )
           {
              if( op.new_product_name.valid() )
                 obj.product_name =  *op.new_product_name;
              if( op.new_brief_desc.valid() )
                 obj.brief_desc =  *op.new_brief_desc;
              if( op.new_id.valid() )
                 obj.id =  *op.new_id;
              if( op.new_refer_price.valid() )
                 obj.refer_price =  *op.new_refer_price;
              if( op.new_status.valid() )
                 obj.status =  *op.new_status;
              if( op.new_icon.valid() )
                 obj.icon =  *op.new_icon;
              if( op.new_schema_contexts.valid() )
                 obj.schema_contexts =  *op.new_schema_contexts;
              for (auto& ext : op.extensions) {
                 if (ext.which() == future_extensions::tag<pocs_threshold_alliance_data_product_t>::value) {
                    obj.pocs_threshold = ext.get<pocs_threshold_alliance_data_product_t>().pocs_threshold;
                    break;
                 }
              }
           });
        return void_result();

} FC_CAPTURE_AND_RETHROW( (op) ) }

}}
