#pragma once

#include <graphene/chain/protocol/operations.hpp>
#include <graphene/db/generic_index.hpp>
#include <boost/multi_index/composite_key.hpp>
#include <vector>
namespace graphene {
    namespace chain {

    using namespace std;

        class alliance_object : public graphene::db::abstract_object<alliance_object> {
        public:
            static const uint8_t space_id = protocol_ids;
            static const uint8_t type_id = alliance_object_type;

            fc::string                              alliance_name;
            fc::string                              brief_desc;
            vector<alliance_data_product_id_type>     data_products;
            vector<uint64_t>                        prices;
            uint8_t                                 status;
            data_market_id_type                     id;
            fc::string                              icon;
            time_point_sec                          create_date_time;
            time_point_sec                          recommend_expiration_date_time;
            uint64_t                                total;
            account_id_type                         issuer;
            vector<account_id_type>                 members;

            vector<uint64_t>                        pocs_thresholds;
            vector<uint64_t>                        fee_bases;
            vector<uint64_t>                        product_pocs_weights;
        };

        struct sort_alliance_object_by_name {
            bool operator() (const alliance_object &l, const alliance_object &r) {
                return l.alliance_name.compare(r.alliance_name) < 0 ? true : false;
            }
        };

        struct by_data_market_id;
        struct by_recommend_expiration_date_time;
        typedef multi_index_container<
           alliance_object,
           indexed_by<
              ordered_unique< tag<by_id>, member< object, object_id_type, &object::id > >,
              ordered_non_unique< tag<by_data_market_id>,
                composite_key<
                   alliance_object,
                     member<alliance_object, data_market_id_type,  &alliance_object::id>
                >
              >,
              ordered_non_unique< tag<by_recommend_expiration_date_time>,
                composite_key<
                   alliance_object,
                    member<alliance_object, time_point_sec,  &alliance_object::recommend_expiration_date_time>
                >
              >
           >
        > alliance_multi_index_type;
        using alliance_index = generic_index<alliance_object, alliance_multi_index_type>;

} }

FC_REFLECT_DERIVED(graphene::chain::alliance_object,
                   (graphene::db::object),
                   (alliance_name)(brief_desc)
                   (data_products)(prices)(status)
                   (id)(icon)(issuer)(create_date_time)(recommend_expiration_date_time)(total)(members)(pocs_thresholds)(fee_bases)(product_pocs_weights))
