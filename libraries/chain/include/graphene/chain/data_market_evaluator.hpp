#pragma once
#include <graphene/chain/protocol/operations.hpp>
#include <graphene/chain/evaluator.hpp>
#include <graphene/chain/database.hpp>
#include <graphene/chain/data_market_object.hpp>
#include <graphene/chain/committee_member_object.hpp>

namespace graphene { namespace chain {

   class data_market_create_evaluator : public evaluator<data_market_create_evaluator>
   {
      public:
         typedef data_market_create_operation operation_type;

         void_result do_evaluate( const data_market_create_operation& o );
         object_id_type do_apply(const data_market_create_operation& o, uint32_t billed_cpu_time_us = 0);
   };
   class data_market_update_evaluator : public evaluator<data_market_update_evaluator>
   {
      public:
         typedef data_market_update_operation operation_type;

         void_result do_evaluate( const data_market_update_operation& o );
         void_result do_apply(const data_market_update_operation& o, uint32_t billed_cpu_time_us = 0);
   };

} } // graphene::chain
