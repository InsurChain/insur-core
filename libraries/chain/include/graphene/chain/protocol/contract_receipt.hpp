#pragma once
#include <graphene/chain/protocol/asset.hpp>

namespace graphene { namespace chain {

   struct contract_receipt_old {
       uint32_t         billed_cpu_time_us = 0;
       uint32_t         ram_usage_bs = 0;
       asset            fee;

       explicit operator std::string() const
       {
           return "{\"billed_cpu_time_us\":" + std::to_string(billed_cpu_time_us) +
                  ", \"ram_usage_bs\":" + std::to_string(ram_usage_bs)  +
                  ", \"fee\":{\"asset_id\":" + std::string(object_id_type(fee.asset_id)) +
                  ", \"amount\":"+ std::to_string(fee.amount.value) + "}}";
       }
   };

   struct account_receipt {
	   account_id_type  account;
       int64_t          ram_bytes;
       asset            ram_fee;

       explicit operator std::string() const
       {
           return "{\"account\":" + std::to_string(account.instance.value) +
				  ", \"ram_bytes\":" + std::to_string(ram_bytes) +
                  ", \"ram_fee\":{\"asset_id\":" + std::string(object_id_type(ram_fee.asset_id)) +
                  ", \"amount\":"+ std::to_string(ram_fee.amount.value) + "}}";
       }
   };

   struct contract_receipt {
       uint32_t                billed_cpu_time_us = 0;
       asset                   fee;
       vector<account_receipt> ram_receipts;


       explicit operator std::string() const
       {
            string receipts_str = "{\"billed_cpu_time_us\":" + std::to_string(billed_cpu_time_us) +
                                  ",\"fee\":{\"asset_id\":" + std::string(object_id_type(fee.asset_id)) +
                                  ",\"amount\":"+ std::to_string(fee.amount.value) + "},\"ram_receipts\":[";
            for(const auto &receipt : ram_receipts)
               receipts_str += ((string)receipt + ",");
            receipts_str += "]}";
            return receipts_str;
       }
   };

} }  // graphene::chain

FC_REFLECT(graphene::chain::contract_receipt_old,
                            (billed_cpu_time_us)
                            (ram_usage_bs)
                            (fee))

FC_REFLECT(graphene::chain::account_receipt,
                            (account)
                            (ram_bytes)
                            (ram_fee))

FC_REFLECT(graphene::chain::contract_receipt,
                            (billed_cpu_time_us)
							(fee)
							(ram_receipts))
