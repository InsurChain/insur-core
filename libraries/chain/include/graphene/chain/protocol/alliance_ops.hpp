#pragma once
#include <graphene/chain/protocol/base.hpp>
#include <graphene/chain/protocol/types.hpp>
#include <graphene/chain/alliance_object.hpp>

namespace graphene { namespace chain {

    struct alliance_create_operation : public base_operation
    {
        struct fee_parameters_type {
           uint64_t fee  = 10 * GRAPHENE_BLOCKCHAIN_PRECISION;
        };

        alliance_id_type                       alliance;
        fc::string                           alliance_name;
        fc::string                           brief_desc;
        vector<alliance_data_product_id_type>  data_products;
        vector<uint64_t>                     prices;
        data_market_id_type                  id;
        fc::string                           icon;
        uint64_t                             total;
        asset                                fee;
        account_id_type                      issuer;
        time_point_sec                       create_date_time;;
        time_point_sec                       recommend_expiration_date_time;
        vector<account_id_type>              members;
        extensions_type                      extensions;

        account_id_type fee_payer() const { return issuer; }
        void validate() const {}
        share_type calculate_fee( const fee_parameters_type& k ) const {
            return k.fee;
        }
    };

    // updated by committee_member
    struct alliance_update_operation : public base_operation
    {
        struct fee_parameters_type {
            uint64_t fee  = 1 * GRAPHENE_BLOCKCHAIN_PRECISION;
        };

        alliance_id_type                                  alliance;
        optional <fc::string>                           new_alliance_name;
        optional <fc::string>                           new_brief_desc;
        optional <vector<alliance_data_product_id_type>>  new_data_products;
        optional <vector<uint64_t>>                     new_prices;
        optional <uint8_t>                              new_status;
        optional <data_market_id_type>                  new_id;
        optional <fc::string>                           new_icon;
        optional <account_id_type>                      new_issuer;
        uint64_t                                        new_total;
        asset                                           fee;
        optional < time_point_sec >                     new_recommend_expiration_date_time;
        optional <vector<account_id_type>>              new_members;
        extensions_type                                 extensions;

        account_id_type fee_payer() const {
            return account_id_type();
        }
        void validate() const {
            FC_ASSERT(fee.amount >= 0);
        }
        share_type calculate_fee( const fee_parameters_type& k ) const {
            return k.fee;
        }
    };

} } // graphene::chain

FC_REFLECT( graphene::chain::alliance_create_operation::fee_parameters_type, (fee) )
FC_REFLECT( graphene::chain::alliance_update_operation::fee_parameters_type, (fee) )

FC_REFLECT( graphene::chain::alliance_create_operation,
            (alliance)
            (alliance_name)
            (brief_desc)
            (data_products)
            (prices)
            (id)
            (icon)
            (fee)
            (issuer)
            (create_date_time)
            (recommend_expiration_date_time)
            (members)
            (extensions)
)
FC_REFLECT( graphene::chain::alliance_update_operation,
            (alliance)
            (new_alliance_name)
            (new_brief_desc)
            (new_data_products)
            (new_prices)
            (new_status)
            (new_id)
            (new_icon)
            (new_issuer)
            (fee)
            (new_recommend_expiration_date_time)
            (new_members)
            (extensions)
)
