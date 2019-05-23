#pragma once

#include <graphene/chain/protocol/operations.hpp>
#include <graphene/db/generic_index.hpp>
#include <boost/multi_index/composite_key.hpp>
#include <vector>
namespace graphene {
    namespace chain {

    using namespace std;

        class database;

        class data_market_object : public graphene::db::abstract_object<data_market_object> {
        public:
            static const uint8_t space_id = protocol_ids;
            static const uint8_t type_id = data_market_object_type;

            uint8_t         status = 0;
            uint8_t         data_market_type;
            uint32_t        order_num;

            string          name;
            account_id_type issuer;
            time_point_sec  create_date_time;
        };
        struct by_issuer{};
        struct by_data_market_type{};
         typedef multi_index_container<
           data_market_object,
           indexed_by<
              ordered_unique< tag<by_id>, member< object, object_id_type, &object::id > >,
              ordered_non_unique< tag<by_issuer>, member<data_market_object, account_id_type, &data_market_object::issuer > >,
              ordered_non_unique< tag<by_data_market_type>,
                composite_key<
                   data_market_object,
                   member<data_market_object, uint8_t, &data_market_object::data_market_type>
                >
              >
           >
        >data_market_multi_index_type;
        using data_market_index = generic_index<data_market_object, data_market_multi_index_type>;
    }
}//graphene::chian


FC_REFLECT_DERIVED(graphene::chain::data_market_object,
                   (graphene::db::object),
                   (status)
                   (data_market_type)
                   (order_num)
                   (name)
                   (issuer)
                   (create_date_time)
                   )
