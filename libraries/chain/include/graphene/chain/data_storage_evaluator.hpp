#pragma once
#include <graphene/chain/protocol/operations.hpp>
#include <graphene/chain/evaluator.hpp>
#include <graphene/chain/database.hpp>
#include <graphene/chain/is_authorized_asset.hpp>
#include <graphene/chain/data_storage_bass_object.hpp>

namespace graphene { namespace chain {

   const uint8_t max_op_string_length = 100;
   class data_storage_evaluator : public evaluator<data_storage_evaluator>
   {
      public:
         typedef data_storage_operation operation_type;

         share_type cut_fee(share_type st, uint16_t p)
         {
             if( st==0 || p == 0 ) return 0;
             if(p == GAPHENE_100_PERCENT) return st;
             fc::uint128 r(st.value);
             r *= p;
             r /= GRAPHENE_100_PERCENT;
             return r.to_uint64();
         }
         void_result do_evaluate( const data_storage_operation& op ){
             try{
                 const database &d = db();
                 FC_ASSERT(op.proxy_memo.size() < max_op_string_length ,"data_hash ${r} too long, must <= 100",("r",op.proxy_memo));
                 FC_ASSERT(op.requests_params.memo.size() < max_op_string_length,"data_md5 ${r} too long, must <= 100",("r",op.requests_params.memo));

                 const auto& asset_by_symbol = d.get_index_type<asset_index>().indices().get<by_symbol>();
                 auto insur = asset_by_symbol.find(GRAPHENE_SYMBOL_INSUR);
                 FC_ASSERT(insur != asset_by_symbol.end());
                 FC_ASSERT(insur->get_id() == op.fee.asset_id, "fee asset must be INSUR");

                 const chain_parameter& chain_parameters = d.get_global_properties().parameters;
                 const auto& expiration = op.requests_params.expiration;
                 FC_ASSERT(expiration <= d.head_block_time() + chain_parameters.maximum_time_until_expiration && expiration >= d.head_block_time(),"user expiration invalid, ${e}",("e",expiration));


                 const account_object &from_account = op.requests_params.from(d);
                 dlog("account signature ${s}",{"s",op.requests_params.signatures.at(0)});
                 
                 const auto& keys = from_account.active.get_keys();
                 FC_ASSERT(keys.size() == 1,"do not support multisig account, account ${a}",("a", op.requests_params.from));
                 FC_ASSERT(op.requests_params.verify_data_storage_signature(keys.at(0),op.requests_params), "verify user signature error");
                 FC_ASSERT(op.requests_params.signatures.size() > 0, "no signature");
                 
                 const auto& data_storage_idx = d.get_index_type<data_storage_index>().indices().get<by_signature>();
                 auto maybe_found = data_storage_idx.find(op.requests_params.signatures.at(0));
                 FC_ASSERT(maybe_found == data_storage_idx.end(), "user request signature already used once! signature ${s}",("s",op.requests_params.signatures.at(0));


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
                         obj.signature    =  op.requests_params.signatures.at(0);
                         obj.expiration   =  op.requests_params.expiration;
                         });
                 share_type commission_amount =cut_fee(op.requests_params.amount.amount, op.requests_params.percentage);
                 asset commission_asset = asset(commission_amount, op.requests_params.amount.asset_id);
                 asset delta_asset = op.requests_params.amount - commission_asset;
                 idump((commission_asset)(delta_asset));

                 db().adjust_balance(op.get_from_account(), -delta_asset);
                 db().adjust_balance(op.get_to_account(), delta_asset);
                 db().adjust_balance(op.get_proxy_account(), commission_asset);

                 return void_result();
             } FC_CAPTURE_AND_RETHROW((op))
         }
   };

} } // graphene::chain
