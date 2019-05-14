#pragma once
#include <graphene/chain/protocol/base.hpp>
#include <graphene/chain/protocol/memo.hpp>
#include <graphene/chain/protocol/ext.hpp>
#include <vector>

namespace graphene { namespace chain {

struct data_storage_params{
    account_id_type      from;
    account_id_type      to;
    account_id_type      proxy_account;
    uint16_t             percentage = 0;
    asset                amount;
    fc::string           memo;
    fc::time_point_sec   expiration;
    vector<signature_type> signatures;
    bool verify_data_storage_signature(const fc::ecc::public_key& expected_signee, const data_storage_params& params)
    {
       auto p = params; 
       p.signatures.clear();
       digest_type::encoder enc;
       fc::raw::pack(enc, p);
       for(const auto& sig: params.signatures)
           if(fc::ecc::public_key(sig,enc.result(), true) == expected_signee)
               return true;
       return false;
    }
};
struct data_storage_operation : public base_operation
{
   struct fee_parameters_type {uint64_t fee = 0;};
   fc::string               proxy_memo;
   asset                    fee;
   data_storage_params      requests_params;
   extensions_type          extensions;

   account_id_type fee_payer() const{return requests_params.proxy_account;}
   account_id_type get_from_account() const{return requests_params.from;}
   account_id_type get_to_account() const{return requests_params.to;}
   account_id_type get_proxy_account() const{return requests_params.proxy_account;}
   void validate() const
   {
       FC_ASSERT(fee.amount >= 0);
       FC_ASSERT(requests_params.amount.amount >= 0);
       FC_ASSERT(proxy_memo.size() > 0);
       FC_ASSERT(get_from_account() != get_to_account());
   }

   share_type calculate_fee(const fee_parameters_type &k)const{
       return k.fee;
   

};
} }//graphene::chain

FC_REFLECT( graphene::chain::data_storage_params,(from)(to)(proxy_account)(percentage)(amount)(memo)(expiration)(signatures) )
FC_REFLECT( graphene::chain::data_storage_operation::fee_parameters_type,(fee)  )
FC_REFLECT( graphene::chain::data_storage_operation,(proxy_memo)(fee)(requests_params)(extensions) )
