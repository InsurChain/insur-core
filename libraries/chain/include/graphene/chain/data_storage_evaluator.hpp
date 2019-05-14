#pragma once
#include <graphene/chain/protocol/operations.hpp>
#include <graphene/chain/evaluator.hpp>
#include <graphene/chain/database.hpp>
#include <graphene/chain/is_authorized_asset.hpp>
#include <graphene/chain/data_storage_bass_object.hpp>

namespace graphene { namespace chain {

   const uint8_t max_op_string_length = 100;
   bool verify_data_storage_signature(const fc::ecc::public_key& expected_signee, const signature_type& signature, const data_storage_params& params)
   {
       digest_type::encoder enc;
       fc::raw::pack(enc, params);
       if(fc::ecc::public_key(signature, enc.result(), true) == expected_signee)
       {
           return true;
       }
       return false;
   }
   class data_storage_evaluator : public evaluator<data_storage_evaluator>
   {
      public:
         typedef data_storage_operation operation_type;

         void_result do_evaluate( const data_storage_operation& op ){
             try{
                 const database &d = db();
                 FC_ASSERT(op.proxy_memo.size() < max_op_string_length ,"data_hash ${r} too long, must <= 100",("r",op.proxy_memo));
                 FC_ASSERT(op.requests_params.memo.size() < max_op_string_length,"data_md5 ${r} too long, must <= 100",("r",op.requests_params.memo));

                 const chain_parameter& chain_parameters = d.get_global_properties().parameters;
                 const auto& expiration = op.requests_params.expiration;
                 FC_ASSERT(expiration <= d.head_block_time() + chain_parameters.maximum_time_until_expiration && expiration >= d.head_block_time(),"user expiration invalid, ${e}",("e",expiration));


                 const account_object &from_account = op.requests_params.from(d);
                 dlog("account signature ${s}",{"s",op.signature});
                 const auto& keys = from_account.active.get_keys();

                 FC_ASSERT(keys.size() == 1,"do not support multisig account, account ${a}",("a", op.requests_params.from));
                 FC_ASSERT(verify_data_storage_signature(keys.at(0),op.signature,op.requests_params), "verify user signature error");
                 const auto& data_storage_idx = d.get_index_type<data_storage_index>().indices().get<by_signature>();
                 auto maybe_found = data_storage_idx.find(op.signature);
                 FC_ASSERT(maybe_found == data_storage_idx.end(), "user request signature already used once! signature ${s}",("s",op.signature));


                 const account_object &to_account = op.requests_params.to(d);
                 const auto& fee = op.requests_params.amount;
                 const asset_object & asset_type = fee.asset_id(d);
                 try
                 {
                     GRPAHENE_ASSET(
                             is_authorized_asset(d, from_account, asset_type),
                             transfer_from_account_not_whitelisted,
                             "'from' account ${from} is not whitelisted for asset ${asset}",
                             ("from",op.requests_params.from)("asset",fee.asset_id));

                     GRPAHENE_ASSET(
                             is_authorized_asset(d, to_account, asset_type),
                             transfer_from_account_not_whitelisted,
                             "'to' account ${to} is not whitelisted for asset ${asset}",
                             ("to",op.requests_params.to)("asset",fee.asset_id));
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
                 }FC_RETHROW_EXCEPTIONS(error, "Unable to pay ${a} from ${f} to ${t}", ("a",d.to_pretty_string(fee.amount))("f",from_account.name)("t",to_account.name));
             }FC_CAPTURE_AND_RETHROW((op))}


         void_result do_apply( const data_storage_operation& op ){
             try{
                 const auto& new_object = db().create<data_storage_baas_object>([&](data_storage_baas_object& obj){
                         obj.signature    =  op.signature;
                         obj.expiration   =  op.params.expiration;
                         });
                 db().adjust_balance(op.account, -op.params.fee);
                 db().adjust_balance(op.proxy_account, op.params.fee);
                 return void_result();
             } FC_CAPTURE_AND_RETHROW((op))
         }
   };

} } // graphene::chain
