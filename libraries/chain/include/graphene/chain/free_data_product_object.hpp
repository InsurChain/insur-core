#pragma once

#include <graphene/db/generic_index.hpp>
#include <boost/multi_index/composite_key.hpp>
#include <vector>

namespace graphene {
    namespace chain {

    using namespace std;
        class free_data_product_object : public graphene::db::abstract_object<free_data_product_object> {
        public:
            static const uint8_t space_id = protocol_ids;
            static const uint8_t type_id = free_data_product_object_type;
            uint8_t                                 status;
            uint64_t                                price;
            uint64_t                                total = 0;
            fc::string                              product_name;
            fc::string                              brief_desc;
            fc::string                              parent_id;
            fc::string                              icon;
            account_id_type                         datasource;
            data_market_id_type                     market_id;
            time_point_sec                          create_date_time;
            time_point_sec                          recommend_expiration_date_time;
            account_id_type                         issuer;

        };

        class free_data_product_search_results_object
                : public graphene::db::abstract_object<free_data_product_search_results_object> {
        public:
            static const uint8_t space_id = implementation_ids;
            static const uint8_t type_id = impl_free_data_product_search_results_object_type;

            uint64_t total;
            uint64_t filtered_total;
            vector <free_data_product_object> data;

        };

        struct by_data_market_id{};
        struct by_recommend_expiration_date_time{};
        typedef multi_index_container<
           free_data_product_object,
           indexed_by<
              ordered_unique< tag<by_id>, member< object, object_id_type, &object::id > >,
              ordered_non_unique< tag<by_data_market_id>,
                composite_key<
                   free_data_product_object,
                     member<free_data_product_object, data_market_id_type,  &free_data_product_object::market_id>
                >
              >,
                ordered_non_unique< tag<by_recommend_expiration_date_time>,
                  composite_key<
                     free_data_product_object,
                       member<free_data_product_object, time_point_sec,  &free_data_product_object::recommend_expiration_date_time>
                  >
                >
           >
        > free_data_product_multi_index_type;

        typedef generic_index<free_data_product_object, free_data_product_multi_index_type> free_data_product_index;
} }
