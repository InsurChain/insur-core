#pragma once
#include <graphene/chain/protocol/operations.hpp>
#include <graphene/chain/evaluator.hpp>
#include <graphene/chain/database.hpp>
#include <graphene/chain/datasource_copyright_object.hpp>

namespace graphene { namespace chain {

class datasource_copyright_clear_evaluator : public evaluator<datasource_copyright_clear_evaluator>
{
   public:
       typedef datasource_copyright_clear_operation operation_type;

       void_result do_evaluate(const datasource_copyright_clear_operation& o);
       void_result do_apply(const datasource_copyright_clear_operation& o, uint32_t billed_cpu_time_us = 0);
};

} } // graphene::chain
