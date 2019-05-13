#pragma once
#include <graphene/chain/protocol/operations.hpp>
#include <graphene/chain/protocol/contract_receipt.hpp>
#include <graphene/chain/evaluator.hpp>

namespace graphene { namespace chain {

class contract_deploy_evaluator : public evaluator<contract_deploy_evaluator>
{
  public:
    typedef contract_deploy_operation operation_type;

    void_result do_evaluate(const contract_deploy_operation &op);
    object_id_type do_apply(const contract_deploy_operation &op, uint32_t billed_cpu_time_us = 0);
};

class contract_update_evaluator : public evaluator<contract_update_evaluator>
{
  public:
    typedef contract_update_operation operation_type;

    void_result do_evaluate(const contract_update_operation &op);
    void_result do_apply(const contract_update_operation &op, uint32_t billed_cpu_time_us = 0);
    string code_hash;
};

class contract_call_evaluator : public evaluator<contract_call_evaluator>
{
  public:
    typedef contract_call_operation operation_type;

    void_result do_evaluate(const contract_call_operation &op);
    operation_result do_apply(const contract_call_operation &op, uint32_t billed_cpu_time_us = 0);

    contract_receipt_old contract_exec_old(database& db, const contract_call_operation& op, uint32_t billed_cpu_time_us);
    contract_receipt     contract_exec(database& db, const contract_call_operation& op, uint32_t billed_cpu_time_us);

    virtual void convert_fee() override;
    virtual void pay_fee() override;

  private:
    contract_call_operation::fee_parameters_type get_contract_call_fee_parameter(const database &db);
    void charge_base_fee(database &db, const contract_call_operation &op, uint32_t cpu_time_us);
    void charge_ram_fee_by_account(account_receipt &r, database &db, const contract_call_operation &op);

  private:
    account_id_type ram_account_id;
    contract_call_operation::fee_parameters_type fee_param;
};

class inter_contract_call_evaluator : public evaluator<inter_contract_call_evaluator>
{
  public:
    typedef inter_contract_call_operation operation_type;

    void_result do_evaluate(const inter_contract_call_operation &op);
    void_result do_apply(const inter_contract_call_operation &op, uint32_t billed_cpu_time_us = 0);
};

} }
