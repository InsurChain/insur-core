#pragma once

#include <graphene/chain/protocol/operations.hpp>
#include <graphene/db/generic_index.hpp>
#include <boost/multi_index/composite_key.hpp>
#include <vector>
namespace graphene {
    namespace chain {

    using namespace std;

        class alliance_data_product_object : public graphene::db::abstract_object<alliance_data_product_object> {
        public:
            static const uint8_t space_id = protocol_ids;
            static const uint8_t type_id =  alliance_data_product_object_type;

            fc::string                              product_name;
            fc::string                              brief_desc;
            fc::string                              icon;
            uint8_t                                 status;
            uint64_t                                refer_price;
            uint64_t                                total = 0;
            uint64_t                                pocs_threshold = 0;

            data_market_id_type                     id;
            time_point_sec                          create_date_time;
            account_id_type                         issuer;
        };
        
        struct by_data_market_id{};
        typedef multi_index_container<
           alliance_data_product_object,
           indexed_by<
              ordered_unique< tag<by_id>, member< object, object_id_type, &object::id > >,
              ordered_non_unique< tag<by_data_market_id>,
                composite_key<
                   alliance_data_product_object,
                     member<alliance_data_product_object, data_market_id_type,  &alliance_data_product_object::id>
                >
              >
           >
        >alliance_data_product_multi_index_type;

        struct by_recommend_expiration_date_time;
        using alliance_data_product_index = generic_index<alliance_data_product_object, alliance_data_product_multi_index_type>;
}}

FC_REFLECT_DERIVED(graphene::chain::alliance_data_product_object,
                   (graphene::db::object),
                   (product_name)(brief_desc)
                   (icon)
                   (status)(refer_price)(total)(pocs_threshold)(id)(create_date_time)(issuer))
