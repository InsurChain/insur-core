#pragma once
#include <graphene/chain/protocol/operations.hpp>
#include <graphene/chain/evaluator.hpp>
#include <graphene/chain/database.hpp>
#include <graphene/chain/is_authorized_asset.hpp>

namespace graphene { namespace chain {

   const uint8_t max_op_string_length = 100;
   class data_storage_evaluator : public evaluator<data_storage_evaluator>
   {
      public:
         typedef data_storage_operation operation_type;

         void_result do_evaluate( const data_storage_operation& op ){
             try{
                 const database &d = db();
                 FC_ASSERT(op.data_hash.size() < max_op_string_length ,"data_hash ${r} too long, must <= 100",("r",op.data_hash));
                 FC_ASSERT(op.params.data_md5.size() < max_op_string_length,"data_md5 ${r} too long, must <= 100",("r",op.params.data_md5));

                 const chain_parameter& chain_parameters = d.get_global_properties().parameters;
                 const auto& expiration = op.params.expiration;
                 FC_ASSERT(expiration <= d.head_block_time() + chain_parameters.maximum_time_until_expiration && expiration >= d.head_block_time());


                 dlog("account signature ${s}",{"s",op.signature});


                 const account_object &from_account = op.account(d);
                 const account_object &to_account = op.proxy_account(d);
                 const auto& fee = op.params.fee;
                 const asset_object & asset_type = fee.asset_id(d);
                 try
                 {
                     GRPAHENE_ASSET(
                             is_authorized_asset(d, from_account, asset_type),
                             transfer_from_account_not_whitelisted,
                             "'from' account ${from} is not whitelisted for asset ${asset}",
                             ("from",op.account)("asset",fee.asset_id));

                     GRPAHENE_ASSET(
                             is_authorized_asset(d, to_account, asset_type),
                             transfer_from_account_not_whitelisted,
                             "'to' account ${to} is not whitelisted for asset ${asset}",
                             ("to",op.proxy_account)("asset",fee.asset_id));
                     if( asset_type.is_transfer_restricted() ){
                         GRPAHENE_ASSET(
                                 from_account.id == asset_type.issuer || to_account.id == asset_type.issuer,
                                 transfer_restricted_transfer_asset,
                                 "Asset ${asset} has transfer_restricted flag enabled",
                                 ("asset", fee.asset_id));
                     }

                     bool insufficient_balance = d.get_balance(from_account, asset_type).amount >= fee.amount;

                     FC_ASSERT(insufficient_balance, "Insufficient Balance: ${balance}, unable to transfer '${total_transfer}' from account '${a}' to '${t}'",
                             ("a",from_account.name)("t",to_account.name)("total_transfer",d.to_pretty_string(fee.amount))("balance",d.to_pretty_string(d.get_balance(from_account,asset_type))));
                     return void_result();
                 }FC_RETHROW_EXCEPTIONS(error, "Unable to pay ${a} from ${f} to ${t}", ("a",d.to_pretty_string(fee.amount))("f",op.account)("t",op.proxy_account));
             }FC_CAPTURE_AND_RETHROW((op))}
         void_result do_apply( const data_storage_operation& op ){
             try{
                 db().adjust_balance(op.account, -op.params.fee);
                 db().adjust_balance(op.proxy_account, op.params.fee);
                 return void_result();
             } FC_CAPTURE_AND_RETHROW((op))
         }
   };

} } // graphene::chain
