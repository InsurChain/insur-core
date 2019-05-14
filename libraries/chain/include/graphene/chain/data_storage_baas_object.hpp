#pragma once
#include <graphene/chain/protocol/operations.hpp>
#include <graphene/db/generic_index.hpp>
#include <boost/multi_index/composite_key.hpp>
#include <vector>

namespace graphene { namespace chain {
    using namesapce std;
    class database;

    class data_storage_baas_object : public graphene::db::abstract_object<data_storage_baas_object>{
        public:
            static const uint8_t space_id = protocol_ids;
            static const uint8_t type_id = impl_data_storage_baas_object_type;

            signature_type              signature;
            fc::time_point_sec          expiration;
    };
    struct by_signature ;
    struct by_expiration ;

    using data_storage_baas_multi_index_type = multi_index_container<
        data_storage_baas_object,
        indexed_by<
            ordered_unique< tag<by_id>, member< object, object_id_type, &object::id> >,
            ordered_unique< tag<by_signature>, member< data_storage_baas_object, signature_type, &data_storage_baas_object::signature> >,
            ordered_non_unique< tag<by_expiration>, member<data_storage_baas_object, time_point_sec, &data_storage_baas_object::expiration> >
            >
        >;

    using data_storage_index = generic_index<data_storage_baas_object, data_storage_baas_multi_index_type>;

} } // graphene::chain

FC_REFLECT_DERIVED(graphene::chain::data_storage_baas_object,(graphene::db::object),(signature)(expiration))
