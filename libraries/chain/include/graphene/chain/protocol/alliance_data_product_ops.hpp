#pragma once
#include <graphene/chain/protocol/base.hpp>
#include <graphene/chain/protocol/types.hpp>
#include <graphene/chain/alliance_data_product_object.hpp>

namespace graphene { namespace chain {

    struct alliance_data_product_create_operation : public base_operation
    {
        struct fee_parameters_type {
           uint64_t fee  = 10 * GRAPHENE_BLOCKCHAIN_PRECISION;
        };

        fc::string                          product_name;
        fc::string                          brief_desc;
        data_market_id_type                 id;
        uint64_t                            refer_price;
        fc::string                          icon;
        vector<schema_context_object>       schema_contexts;
        account_id_type                     issuer;
        time_point_sec                      create_date_time;;
        asset                               fee;
        extensions_type                     extensions;
        account_id_type fee_payer() const { return issuer; }
        void validate() const {}
        share_type calculate_fee( const fee_parameters_type& k ) const {
            return k.fee;
        }

    };

} } // graphene::chain

FC_REFLECT( graphene::chain::alliance_data_product_create_operation::fee_parameters_type, (fee) )

FC_REFLECT( graphene::chain::alliance_data_product_create_operation,
            (product_name)
            (brief_desc)
            (id)
            (refer_price)
            (icon)
            (schema_contexts)
            (fee)
            (create_date_time)
            (issuer)
            (extensions)
)
