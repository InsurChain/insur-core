#pragma once

#include <graphene/chain/database.hpp>
#include <graphene/chain/balance_object.hpp>
#include <graphene/chain/evaluator.hpp>
#include <graphene/chain/exceptions.hpp>

namespace graphene { namespace chain {

#define LOCKED_BALANCE_EXPIRED_TIME 30

class balance_lock_evaluator : public evaluator<balance_lock_evaluator>
{
public:
    typedef balance_lock_operation operation_type;

    void_result do_evaluate(const balance_lock_operation& op);
    object_id_type do_apply(const balance_lock_operation& op, int32_t billed_cpu_time_us = 0);
};

class balance_unlock_evaluator : public evaluator<balance_unlock_evaluator>
{
public:
    typedef balance_unlock_operation operation_type;

    void_result do_evaluate(const balance_unlock_operation& op);
    void_result do_apply(const balance_unlock_operation& op, int32_t billed_cpu_time_us = 0);

private:
    const lock_balance_object* lock_balance_obj = nullptr;
};

} } // graphene::chain
