#pragma once

#include <graphene/chain/protocol/operations.hpp>
#include <graphene/db/generic_index.hpp>
#include <boost/multi_index/composite_key.hpp>
#include <vector>
namespace graphene {
    namespace chain {

    using namespace std;
    class database;
    class alliance_object;

        struct  data_transaction_datasource_status_object
        {
            account_id_type     datasource;
            uint8_t             status = 0;
        };

        class data_transaction_object : public graphene::db::abstract_object<data_transaction_object> {
        public:
            static const uint8_t space_id = protocol_ids;
            static const uint8_t type_id = data_transaction_object_type;

            string                                  request_id;
            object_id_type                          product_id;
            string                                  version;
            string                                  params;
            uint8_t                                 status = 0;
            time_point_sec                          create_date_time;
            account_id_type                         requester;
            fc::optional<alliance_id_type>            alliance_id = fc::optional<alliance_id_type>();
            string                                  memo;
            vector<data_transaction_datasource_status_object>             datasources_status;
            uint64_t                                product_pay = 0;
            uint64_t                                transaction_fee = 0;
            uint64_t                                commission = 0;
        };

        struct sort_data_transaction_object_by_create_date_time {
            bool operator() (const data_transaction_object &l, const data_transaction_object &r) {
                return l.create_date_time > r.create_date_time;
            }
        };

        class data_transaction_search_results_object : public graphene::db::abstract_object<data_transaction_search_results_object> {
        public:
            static const uint8_t space_id = implementation_ids;
            static const uint8_t type_id = impl_data_transaction_search_results_object_type;

            uint64_t total = 0;
            vector <data_transaction_object> data;
        };

        class data_transaction_complain_object : public graphene::db::abstract_object<data_transaction_complain_object>{
        public:
            static const uint8_t space_id = protocol_ids;
            static const uint8_t type_id = data_transaction_complain_object_type;
            account_id_type                      requester;
            account_id_type                      datasource;
            string                               data_transaction_request_id;
            string                               merchant_copyright_hash;
            string                               datasource_copyright_hash;
            time_point_sec                       create_date_time;
        };

        struct by_request_id {};
        struct by_create_date_time {};
        struct by_requester {};
        using data_transaction_multi_index_type = multi_index_container<
            data_transaction_object,
            indexed_by<
                ordered_unique< tag<by_id>, member< object, object_id_type, &object::id > >,
                ordered_non_unique< tag<by_create_date_time>, member<data_transaction_object, time_point_sec, &data_transaction_object::create_date_time> >,
                ordered_non_unique< tag<by_requester>,
                        member<data_transaction_object, account_id_type, &data_transaction_object::requester>
                >,
                ordered_unique< tag<by_request_id>,
                    composite_key<
                        data_transaction_object,
                        member<data_transaction_object, string, &data_transaction_object::request_id>,
                        member<data_transaction_object, time_point_sec, &data_transaction_object::create_date_time>
                    >,
                    composite_key_compare<std::less<string>, std::less<time_point_sec>>
                >
             >
        >;
        using data_transaction_index = generic_index<data_transaction_object, data_transaction_multi_index_type>;


        using data_transaction_complain_multi_index_type = multi_index_container<
            data_transaction_complain_object,
            indexed_by<
                ordered_unique< tag<by_id>, member< object, object_id_type, &object::id > >,
                ordered_non_unique< tag< by_create_date_time >, member< data_transaction_complain_object, time_point_sec, &data_transaction_complain_object::create_date_time > >,
                ordered_unique< tag<by_request_id>, member<data_transaction_complain_object, string, &data_transaction_complain_object::data_transaction_request_id> >
                >
            >;

        using data_transaction_complain_index = generic_index<data_transaction_complain_object, data_transaction_complain_multi_index_type>;
    }
}

FC_REFLECT( graphene::chain::data_transaction_datasource_status_object,
            (datasource)
            (status)
)


FC_REFLECT_DERIVED( graphene::chain::data_transaction_complain_object,
            (graphene::db::object),
            (requester)
            (datasource)
            (data_transaction_request_id)
            (merchant_copyright_hash)
            (datasource_copyright_hash)
            (create_date_time)
 )

FC_REFLECT_DERIVED(graphene::chain::data_transaction_object,
                   (graphene::db::object),
                   (request_id)
                   (product_id)
                   (version)
                   (params)
                   (status)
                   (create_date_time)
                   (requester)
                   (alliance_id)
                   (memo)
                   (datasources_status)
                   (product_pay)
                   (transaction_fee)
                   (commission))
FC_REFLECT_DERIVED(graphene::chain::data_transaction_search_results_object,
                   (graphene::db::object),
                   (total)
                   (data))
