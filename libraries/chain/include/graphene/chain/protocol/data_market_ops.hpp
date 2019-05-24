#pragma once
#include <graphene/chain/protocol/base.hpp>
#include <graphene/chain/protocol/types.hpp>
#include <graphene/chain/data_market_object.hpp>

namespace graphene { namespace chain {

    struct data_market_create_operation : public base_operation
    {
        struct fee_parameters_type {
           uint64_t fee  = 10 * GRAPHENE_BLOCKCHAIN_PRECISION;
        };

        fc::string              name;
        uint8_t                 data_market_type;
        uint32_t                order_num;
        asset                   fee;
        account_id_type         issuer;
        time_point_sec          create_date_time;
        extensions_type         extensions;
        account_id_type fee_payer()const { return issuer; }
        void            validate() const {}
        share_type calculate_fee( const fee_parameters_type& k ) const {
            return k.fee;
        }

    };

    struct data_market_update_operation : public base_operation
    {
        struct fee_parameters_type {
           uint64_t fee  = 0;
        };

        optional< fc::string >          new_name;
        optional< uint32_t >            new_order_num;
        data_market_id_type             data_market;
        extensions_type                 extensions;
        uint8_t                         data_market_type;
        asset                           fee;
        optional< uint8_t >             new_status;
        account_id_type fee_payer() const { return account_id_type(); }
        void validate()const{
            FC_ASSERT(fee.amount >= 0);
        }
        share_type calculate_fee( const fee_parameters_type& k ) const {
            return k.fee;
        }
    };


} } // graphene::chain

FC_REFLECT( graphene::chain::data_market_create_operation::fee_parameters_type, (fee) )
FC_REFLECT( graphene::chain::data_market_update_operation::fee_parameters_type, (fee) )

FC_REFLECT( graphene::chain::data_market_create_operation,
            (name)
            (data_market_type)
            (order_num)
            (fee)
            (issuer)
            (create_date_time)
            (extensions)
)
FC_REFLECT( graphene::chain::data_market_update_operation,
            (new_name)
            (new_order_num)
            (data_market)
            (data_market_type)
            (fee)
            (new_status)
            (extensions)
)
