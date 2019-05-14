#pragma once
#include <graphene/chain/protocol/operations.hpp>
#include <graphene/chain/evaluator.hpp>
#include <graphene/chain/database.hpp>
#include <graphene/chain/is_authorized_asset.hpp>

namespace graphene { namespace chain {

   class data_storage_evaluator : public evaluator<data_storage_evaluator>
   {
      public:
         typedef data_storage_operation operation_type;

         void_result do_evaluate( const data_storage_operation& op ){
             try{
                 const database &d = db();

                 const chain_parameter& chain_parameters = d.get_global_properties().parameters;
                 const auto& expiration = op.params.expiration;
                 FC_ASSERT(expiration <= d.head_block_time() + chain_parameters.maximum_time_until_expiration && expiration >= d.head_block_time());


                 dlog("account signature ${s}",{"s",op.signature});


                 const account_object &from_account = op.account(d);
                 const account_object &to_account = op.proxy_account(d);
                 const asset_object &asset_type = op.params.fee.asset_id(d);

                 return void_result();
             }FC_CAPTURE_AND_RETHROW((op))
         }
         void_result do_apply( const data_storage_operation& op ){
             try{
                 db().adjust_balance(op.account, -op.params.fee);
                 db().adjust_balance(op.proxy_account, op.params.fee);
                 return void_result();
             } FC_CAPTURE_AND_RETHROW((op))
         }
   };

} } // graphene::chain
