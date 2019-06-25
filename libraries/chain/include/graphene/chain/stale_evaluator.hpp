#pragma once
#include <graphene/chain/protocol/operations.hpp>
#include <graphene/chain/evaluator.hpp>
#include <graphene/chain/database.hpp>
#include <graphene/chain/data_market_object.hpp>
#include <graphene/chain/free_data_product_object.hpp>
#include <graphene/chain/alliance_object.hpp>
#include <graphene/chain/alliance_data_product_object.hpp>

namespace graphene { namespace chain {

   class stale_data_market_category_create_evaluator : public evaluator<stale_data_market_category_create_evaluator>
   {
      public:
         typedef stale_data_market_category_create_operation operation_type;

         void_result do_evaluate( const stale_data_market_category_create_operation& o );
         object_id_type do_apply(const stale_data_market_category_create_operation& o, int32_t billed_cpu_time_us = 0);
   };
   class stale_data_market_category_update_evaluator : public evaluator<stale_data_market_category_update_evaluator>
   {
      public:
         typedef stale_data_market_category_update_operation operation_type;

         void_result do_evaluate( const stale_data_market_category_update_operation& o );
         void_result do_apply(const stale_data_market_category_update_operation& o, int32_t billed_cpu_time_us = 0);
   };
   class stale_free_data_product_create_evaluator : public evaluator<stale_free_data_product_create_evaluator>
   {
      public:
         typedef stale_free_data_product_create_operation operation_type;

         void_result do_evaluate( const stale_free_data_product_create_operation& o );
         object_id_type do_apply(const stale_free_data_product_create_operation& o, int32_t billed_cpu_time_us = 0);
   };
   class stale_free_data_product_update_evaluator : public evaluator<stale_free_data_product_update_evaluator>
   {
      public:
         typedef stale_free_data_product_update_operation operation_type;

         void_result do_evaluate( const stale_free_data_product_update_operation& o );
         void_result do_apply(const stale_free_data_product_update_operation& o, int32_t billed_cpu_time_us = 0);
   };
   class stale_alliance_create_evaluator : public evaluator<stale_alliance_create_evaluator>
    {
    public:
        typedef stale_alliance_create_operation operation_type;

        void_result do_evaluate( const stale_alliance_create_operation& o );
        object_id_type do_apply(const stale_alliance_create_operation& o, int32_t billed_cpu_time_us = 0);
    };

   class stale_alliance_update_evaluator : public evaluator<stale_alliance_update_evaluator>
    {
    public:
        typedef stale_alliance_update_operation operation_type;

        void_result do_evaluate( const stale_alliance_update_operation& o );
        void_result do_apply(const stale_alliance_update_operation& o, int32_t billed_cpu_time_us = 0);
    };
   class stale_alliance_data_product_create_evaluator : public evaluator<stale_alliance_data_product_create_evaluator>
   {
      public:
         typedef stale_alliance_data_product_create_operation operation_type;

         void_result do_evaluate( const stale_alliance_data_product_create_operation& o );
         object_id_type do_apply(const stale_alliance_data_product_create_operation& o, int32_t billed_cpu_time_us = 0);
   };

   class stale_alliance_data_product_update_evaluator : public evaluator<stale_alliance_data_product_update_evaluator>
   {
      public:
         typedef stale_alliance_data_product_update_operation operation_type;

         void_result do_evaluate( const stale_alliance_data_product_update_operation& o );
         void_result do_apply(const stale_alliance_data_product_update_operation& o, int32_t billed_cpu_time_us = 0);
   };

} } // graphene::chain

