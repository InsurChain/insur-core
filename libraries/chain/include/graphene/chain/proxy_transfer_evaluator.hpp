#pragma once
#include <graphene/chain/protocol/operations.hpp>
#include <graphene/chain/evaluator.hpp>
#include <graphene/chain/database.hpp>

namespace graphene { namespace chain {

   class proxy_transfer_evaluator : public evaluator<proxy_transfer_evaluator>
   {
     public:
       typedef proxy_transfer_operation operation_type;

       void_result do_evaluate(const proxy_transfer_operation &o);
       void_result do_apply(const proxy_transfer_operation &o, int32_t billed_cpu_time_us = 0);

     private:
       share_type cut_fee(share_type a, uint16_t p);
   };
} }
