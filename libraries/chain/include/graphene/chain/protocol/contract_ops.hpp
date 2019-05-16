#pragma once
#include <graphene/chain/protocol/base.hpp>
#include <graphene/chain/action.hpp>

namespace graphene { namespace chain {
struct contract_cal_operation : public base_operation {
    struct fee_parameters_type {
        uint64_t fee = 0 * GRAPHENE_BLOCKCHAIN_PRECISION;
    };

    account_id_type                 account;
    asset                           fee;
    fc::string                      name;
    fc::string                      method;
    fc::string                      data;
    extensions_type                 extensions;

    account_id_type fee_payer() const
    {
        return account;
    }

    void validate() const
    {
        FC_ASSERT(fee.amount >= 0, "fee.amount < 0");
        FC_ASSERT(data.size() > 0);
    }

    share_type calculate_fee(const fee_parameters_type &k) const
    {
        return k.fee;
    }
};

} } // graphene::chain


FC_REFLECT(graphene::chain::contract_call_operation::fee_parameters_type,
            (fee))
FC_REFLECT(graphene::chain::contract_call_operation,
            (account)
            (fee)
            (name)
            (method)
            (extensions))
