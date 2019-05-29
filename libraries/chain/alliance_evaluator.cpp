
#include <graphene/chain/alliance_evaluator.hpp>
#include <graphene/chain/database.hpp>

namespace graphene { namespace chain {


void_result alliance_create_evaluator::do_evaluate( const alliance_create_operation& op )
{ try {
    for (auto &datasource : op.members) {
        FC_ASSERT(db().get(datasource).is_datasource_member());
    }

    for (auto& data_opt : op.data_products) {
        FC_ASSERT(db().find_object(data_opt), "This product_id is not found, product_id is ${id}", ("id", data_opt));
    }

    for (auto& ext : op.extensions) {
        if (ext.which() == future_extensions::tag<pocs_threshold_alliance_t>::value) {
            FC_ASSERT(op.members.size() == ext.get<pocs_threshold_alliance_t>().pocs_thresholds.size(), "pocs_thresholds size error");
            FC_ASSERT(op.members.size() == ext.get<pocs_threshold_alliance_t>().fee_bases.size(), "fee_bases size error");
            FC_ASSERT(op.data_products.size() == ext.get<pocs_threshold_alliance_t>().product_pocs_weights.size(), "product_pocs_weights size error");
            break;
        }
    }

    FC_ASSERT(db().get(op.issuer).is_lifetime_member());

    FC_ASSERT(db().find_object(op.id));

    return void_result();
} FC_CAPTURE_AND_RETHROW( (op) ) }

object_id_type alliance_create_evaluator::do_apply(const alliance_create_operation& op, int32_t billed_cpu_time_us)
{ try {

   const auto& new_object = db().create<alliance_object>( [&]( alliance_object& obj ){
        obj.alliance_name = op.alliance_name;
        obj.brief_desc = op.brief_desc;
        obj.data_products = op.data_products;
        obj.prices = op.prices;
        obj.status = alliance_undo_status;
        obj.id = op.id;
        obj.icon = op.icon;
        obj.issuer = op.issuer;
        obj.total = 0;
        obj.create_date_time = op.create_date_time;
        obj.recommend_expiration_date_time = op.recommend_expiration_date_time;
        obj.members = op.members;

        obj.pocs_thresholds = vector<uint64_t>(op.members.size(), 0);
        obj.fee_bases = vector<uint64_t>(op.members.size(), 1);
        obj.product_pocs_weights = vector<uint64_t>(op.data_products.size(), 1);
        for (auto& ext : op.extensions) {
           if (ext.which() == future_extensions::tag<pocs_threshold_alliance_t>::value) {
              obj.pocs_thresholds = ext.get<pocs_threshold_alliance_t>().pocs_thresholds;
              obj.fee_bases = ext.get<pocs_threshold_alliance_t>().fee_bases;
              obj.product_pocs_weights = ext.get<pocs_threshold_alliance_t>().product_pocs_weights;
              break;
           }
        }
   });
   return  new_object.id;

} FC_CAPTURE_AND_RETHROW( (op) ) }

void_result alliance_update_evaluator::do_evaluate( const alliance_update_operation& op )
{ try {
    FC_ASSERT(trx_state->_is_proposed_trx);
    for (auto& ext : op.extensions) {
        if (ext.which() == future_extensions::tag<pocs_threshold_alliance_t>::value) {
            FC_ASSERT(op.new_members->size() == ext.get<pocs_threshold_alliance_t>().pocs_thresholds.size(), "pocs_thresholds size error");
            FC_ASSERT(op.new_members->size() == ext.get<pocs_threshold_alliance_t>().fee_bases.size(), "fee_bases size error");
            FC_ASSERT(op.new_data_products->size() == ext.get<pocs_threshold_alliance_t>().product_pocs_weights.size(), "product_pocs_weights size error");
            break;
        }
    }
    return void_result();
} FC_CAPTURE_AND_RETHROW( (op) ) }

void_result alliance_update_evaluator::do_apply(const alliance_update_operation& op, int32_t billed_cpu_time_us)
{
    try {
        database& _db = db();
        _db.modify(
           _db.get(op.alliance),
           [&]( alliance_object& obj )
           {
              if(op.new_alliance_name.valid())
                 obj.alliance_name =  *op.new_alliance_name;
              if( op.new_brief_desc.valid() )
                 obj.brief_desc =  *op.new_brief_desc;
              if( op.new_data_products.valid() )
                 obj.data_products = *op.new_data_products;
              if( op.new_prices.valid() )
                 obj.prices =  *op.new_prices;
              if( op.new_status.valid() )
                 obj.status =  *op.new_status;
              if( op.new_id.valid() )
                 obj.id =  *op.new_id;
              if( op.new_icon.valid() )
                 obj.icon =  *op.new_icon;
              if(op.new_recommend_expiration_date_time.valid())
                 obj.recommend_expiration_date_time =  *op.new_recommend_expiration_date_time;
              if (op.new_members.valid())
                 obj.members = *op.new_members;

              obj.pocs_thresholds = vector<uint64_t>(op.new_members->size(), 0);
              obj.fee_bases = vector<uint64_t>(op.new_members->size(), 1);
              obj.product_pocs_weights = vector<uint64_t>(op.new_data_products->size(), 1);
              for (auto& ext : op.extensions) {
                 if (ext.which() == future_extensions::tag<pocs_threshold_alliance_t>::value) {
                    obj.pocs_thresholds = ext.get<pocs_threshold_alliance_t>().pocs_thresholds;
                    obj.fee_bases = ext.get<pocs_threshold_alliance_t>().fee_bases;
                    obj.product_pocs_weights = ext.get<pocs_threshold_alliance_t>().product_pocs_weights;
                    break;
                 }
              }
           });
        return void_result();

} FC_CAPTURE_AND_RETHROW( (op) ) }
} } // graphene::chain
