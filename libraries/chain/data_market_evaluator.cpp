#include <graphene/chain/data_market_evaluator.hpp>
#include <graphene/chain/database.hpp>

namespace graphene { namespace chain {

void_result data_market_create_evaluator::do_evaluate( const data_market_create_operation& op )
{ try {
   FC_ASSERT(db().get(op.issuer).is_lifetime_member());
   return void_result();
} FC_CAPTURE_AND_RETHROW( (op) ) }

object_id_type data_market_create_evaluator::do_apply(const data_market_create_operation& op, uint32_t billed_cpu_time_us)
{ try {
    const auto& new_object = db().create<data_market_object>( [&]( data_market_object& obj ) {
       dlog("data_market_create do_apply");
       obj.issuer = op.issuer;
       obj.name = op.name;
       obj.data_market_type = op.data_market_type;
       obj.order_num = op.order_num;
       obj.status = data_market_undo_status;
       obj.create_date_time = op.create_date_time;
   });
   return  new_object.id;

} FC_CAPTURE_AND_RETHROW( (op) ) }

void_result data_market_update_evaluator::do_evaluate( const data_market_update_operation& op )
{ try {
   FC_ASSERT(trx_state->_is_proposed_trx);
   return void_result();
} FC_CAPTURE_AND_RETHROW( (op) ) }

void_result data_market_update_evaluator::do_apply(const data_market_update_operation& op, uint32_t billed_cpu_time_us)
{
    try {
        database& _db = db();
        _db.modify(
           _db.get(op.data_market),
           [&]( data_market_object& cat ) {
              if( op.new_name.valid() )
                 cat.name =  *op.new_name;
              if( op.new_order_num.valid() )
                 cat.order_num =  *op.new_order_num;
              if( op.new_status.valid())
                 cat.status =  *op.new_status;
           });
        return void_result();

} FC_CAPTURE_AND_RETHROW( (op) ) }

} } // graphene::chain
