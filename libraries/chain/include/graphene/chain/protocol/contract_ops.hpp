#pragma once
#include <graphene/chain/protocol/base.hpp>
#include <graphene/chain/action.hpp>
#include <graphene/chain/abi_def.hpp>

namespace graphene { namespace chain {
struct contract_deploy_operation : public base_operation {
    struct fee_parameters_type {
        uint64_t fee = 1 * GRAPHENE_BLOCKCHAIN_PRECISION;
        uint64_t price_per_kbyte = GRAPHENE_BLOCKCHAIN_PRECISION;
    };

    asset                           fee;
    fc::string                      name;
    account_id_type                 account;
    fc::string                      vm_type;
    fc::string                      vm_version;
    bytes                           code;
    abi_def                         abi;
    extensions_type                 extensions;

    account_id_type fee_payer() const
    {
        return account;
    }

    void validate() const
    {
        FC_ASSERT(fee.amount >= 0, "fee.amount < 0");
        FC_ASSERT(is_valid_name(name), "contract name is invalid");
        FC_ASSERT(code.size() > 0, "contract code cannot be empty");
    }

    share_type calculate_fee(const fee_parameters_type &k) const
    {
        auto core_fee_required = k.fee;
        auto data_fee = calculate_data_fee(fc::raw::pack_size(*this), k.price_per_kbyte);
        core_fee_required += data_fee;
        return core_fee_required;
    }
};

struct contract_update_operation : public base_operation {
    struct fee_parameters_type {
        uint64_t fee = 1 * GRAPHENE_BLOCKCHAIN_PRECISION;
        uint64_t price_per_kbyte = GRAPHENE_BLOCKCHAIN_PRECISION;
    };

    asset                           fee;
    account_id_type                 owner;
    optional<account_id_type>       new_owner;
    account_id_type                 contract;
    bytes                           code;
    abi_def                         abi;
    extensions_type                 extensions;

    account_id_type fee_payer() const
    {
        return owner;
    }

    void validate() const
    {
        FC_ASSERT(fee.amount >= 0, "fee.amount < 0");
        FC_ASSERT(code.size() > 0, "contract code cannot be empty");
    }

    share_type calculate_fee(const fee_parameters_type &k) const
    {
        auto core_fee_required = k.fee;
        auto data_fee = calculate_data_fee(fc::raw::pack_size(*this), k.price_per_kbyte);
        core_fee_required += data_fee;
        return core_fee_required;
    }
};

struct contract_call_operation : public base_operation {
    struct fee_parameters_type {
        uint64_t fee =  GRAPHENE_BLOCKCHAIN_PRECISION / 1000;
        uint64_t price_per_kbyte_ram =  GRAPHENE_BLOCKCHAIN_PRECISION / 2;
        uint64_t price_per_ms_cpu = 0;
    };

    asset                                   fee;
    account_id_type                         account;
    account_id_type                         contract_id;
    fc::optional<asset>                     amount;
    action_name                             method_name;
    bytes                                   data;
    extensions_type                         extensions;

    account_id_type fee_payer() const { return account; }

    void validate() const
    {
        FC_ASSERT(fee.amount >= 0);
        FC_ASSERT(data.size() >= 0);
    }

    share_type calculate_fee(const fee_parameters_type &k) const
    {
        // just return basic fee, real fee will be calculated after runing
        return k.fee;
    }
};

struct inter_contract_call_operation : public base_operation {
    struct fee_parameters_type {
        uint64_t fee = 0;
    };

    asset                                   fee;
    account_id_type                         sender_contract;
    account_id_type                         contract_id;
    fc::optional<asset>                     amount;
    action_name                             method_name;
    bytes                                   data;
    extensions_type                         extensions;

    account_id_type fee_payer() const { return sender_contract; }

    void validate() const
    {
        FC_ASSERT(fee.amount >= 0);
        FC_ASSERT(data.size() >= 0);
    }

    share_type calculate_fee(const fee_parameters_type &k) const
    {
        // just return basic fee, real fee will be calculated after runing
        return k.fee;
    }
};


} } // graphene::chain

FC_REFLECT(graphene::chain::contract_deploy_operation::fee_parameters_type,
            (fee)(price_per_kbyte))
FC_REFLECT(graphene::chain::contract_deploy_operation,
            (fee)
            (name)
            (account)
            (vm_type)
            (vm_version)
            (code)
            (abi)
            (extensions))
            
FC_REFLECT(graphene::chain::contract_update_operation::fee_parameters_type,
            (fee)(price_per_kbyte))
FC_REFLECT(graphene::chain::contract_update_operation,
            (fee)
            (owner)
            (new_owner)
            (contract)
            (code)
            (abi)
            (extensions))

FC_REFLECT(graphene::chain::contract_call_operation::fee_parameters_type,
            (fee)(price_per_kbyte_ram)(price_per_ms_cpu))
FC_REFLECT(graphene::chain::contract_call_operation,
            (fee)
            (account)
            (contract_id)
            (amount)
            (method_name)
            (data)
            (extensions))

FC_REFLECT(graphene::chain::inter_contract_call_operation::fee_parameters_type, (fee))
FC_REFLECT(graphene::chain::inter_contract_call_operation,
            (fee)
            (sender_contract)
            (contract_id)
            (amount)
            (method_name)
            (data)
            (extensions))
