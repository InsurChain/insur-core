#pragma once
#include <graphene/chain/protocol/operations.hpp>
#include <graphene/chain/evaluator.hpp>
#include <graphene/chain/database.hpp>
#include <graphene/chain/alliance_object.hpp>
#include <graphene/chain/committee_member_object.hpp>

namespace graphene { namespace chain {

   class alliance_create_evaluator : public evaluator<alliance_create_evaluator>
    {
    public:
        typedef alliance_create_operation operation_type;

        void_result do_evaluate( const alliance_create_operation& o );
        object_id_type do_apply(const alliance_create_operation& o, int32_t billed_cpu_time_us = 0);
    };

   class alliance_update_evaluator : public evaluator<alliance_update_evaluator>
    {
    public:
        typedef alliance_update_operation operation_type;

        void_result do_evaluate( const alliance_update_operation& o );
        void_result do_apply(const alliance_update_operation& o, int32_t billed_cpu_time_us = 0);
    };

} } // graphene::chain
