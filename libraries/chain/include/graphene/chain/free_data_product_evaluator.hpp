#pragma once
#include <graphene/chain/protocol/operations.hpp>
#include <graphene/chain/evaluator.hpp>
#include <graphene/chain/database.hpp>
#include <graphene/chain/free_data_product_object.hpp>

namespace graphene { namespace chain {

   class free_data_product_create_evaluator : public evaluator<free_data_product_create_evaluator>
   {
      public:
         typedef free_data_product_create_operation operation_type;

         void_result do_evaluate( const free_data_product_create_operation& o );
         object_id_type do_apply(const free_data_product_create_operation& o, int32_t billed_cpu_time_us = 0);
   };
   class free_data_product_update_evaluator : public evaluator<free_data_product_update_evaluator>
   {
      public:
         typedef free_data_product_update_operation operation_type;

         void_result do_evaluate( const free_data_product_update_operation& o );
         void_result do_apply(const free_data_product_update_operation& o, int32_t billed_cpu_time_us = 0);
   };

} } // graphene::chain
