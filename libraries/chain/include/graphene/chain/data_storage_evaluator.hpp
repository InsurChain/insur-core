#pragma once
#include <graphene/chain/protocol/operations.hpp>
#include <graphene/chain/evaluator.hpp>
#include <graphene/chain/database.hpp>

namespace graphene { namespace chain {

   class data_storage_evaluator : public evaluator<data_storage_evaluator>
   {
      public:
         typedef data_storage_operation operation_type;

         void_result do_evaluate( const data_storage_operation& op ){
             ilog("data_storage_evaluator do_evaluate");
             return void_result();
         }
         void_result do_apply( const data_storage_operation& op ){
             ilog("data_storage_evaluator do_evaluate");
             return void_result();
         }
   };

} } // graphene::chain
