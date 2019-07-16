
#pragma once

#include <graphene/chain/protocol/operations.hpp>
#include <graphene/db/generic_index.hpp>
#include <boost/multi_index/composite_key.hpp>
#include <vector>
namespace graphene {
    namespace chain {

    using namespace std;
    class database;

        /**
         * Query the signature_object
         * @brief
         */
        class signature_object : public graphene::db::abstract_object<signature_object> {
        public:
            static const uint8_t space_id = implementation_ids;
            static const uint8_t type_id = impl_signature_object_type;

            // user's signature who use proxy_transfer service
            signature_type              signature;
            // expiration time
            fc::time_point_sec          expiration;
        };

        /**
         * @ingroup object_index
         */
        struct by_signature;
        struct by_expiration;
        using signature_multi_index_type = multi_index_container<
            signature_object,
            indexed_by<
                ordered_unique< tag<by_id>, member< object, object_id_type, &object::id > >,
                ordered_unique< tag<by_signature>, member<signature_object, signature_type, &signature_object::signature> >,
                ordered_non_unique< tag<by_expiration>, member<signature_object, time_point_sec, &signature_object::expiration > >
             >
        >;
        /**
         * @ingroup object_index
         */
        using signature_index = generic_index<signature_object, signature_multi_index_type>;

    }
}

FC_REFLECT_DERIVED(graphene::chain::signature_object,
                   (graphene::db::object),
                   (signature)
                   (expiration))
