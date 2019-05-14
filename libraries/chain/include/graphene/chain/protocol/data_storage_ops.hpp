#pragma once
#include <graphene/chain/protocol/base.hpp>
#include <graphene/chain/protocol/memo.hpp>
#include <graphene/chain/protocol/ext.hpp>
#include <graphene/chain/protocol/data_storage_params.hpp>

namespace graphene { namespace chain {

struct data_storage_operation : public base_operation
{
   struct fee_parameters_type {uint64_t fee = 0;};
   account_id_type          proxy_account;
   account_id_type          account;
   fc::string               data_hash;
   asset                    fee;
   data_storage_params      params;
   signture_type            signature;
   extensions_type          extensions;

   account_id_type fee_payer() const{return proxy_account;}
   void validate() const{}

   share_type calculate_fee(const fee_parameters_type &k)const{
       return k.fee;
   

};
} }//graphene::chain

FC_REFLECT( graphene::chain::data_storage_operation::fee_parameters_type,(fee)  )
FC_REFLECT( graphene::chain::data_storage_operation,(proxy_account)(account)(data_hash)(fee)(params)(signature)(extensions) )
