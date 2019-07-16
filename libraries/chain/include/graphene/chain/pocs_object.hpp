#pragma once

#include <graphene/chain/protocol/types.hpp>
#include <graphene/db/generic_index.hpp>

namespace graphene { namespace chain {

    /**
     *  @brief caculate Contribution ratio
     *  @ingroup object
     *  @ingroup protocol
     */
     class pocs_object : public graphene::chain::abstract_object<pocs_object>
     {
       public:
          static const uint8_t space_id = protocol_ids;
          static const uint8_t type_id = pocs_object_type;
          league_id_type    league_id;
          account_id_type   account_id;
          uint64_t          total_sell = 0;
          uint64_t          total_buy = 0;
     };

     struct by_id;
     struct by_league_id;
     struct by_account_id;
     struct by_multi_id;

     typedef multi_index_container<
        pocs_object, indexed_by<
          ordered_unique<tag<by_id>, member<object, object_id_type, &object::id>>,
          ordered_non_unique<tag<by_league_id>, composite_key<pocs_object, member<pocs_object, league_id_type, &pocs_object::league_id>>>,
          ordered_non_unique<tag<by_account_id>, composite_key<pocs_object, member<pocs_object, account_id_type, &pocs_object::account_id>>>,
          ordered_unique<tag<by_multi_id>, composite_key<pocs_object, member<pocs_object, league_id_type, &pocs_object::league_id>,
          member<pocs_object, account_id_type, &pocs_object::account_id>>>
        >
     > pocs_multi_index_type;

     typedef generic_index<pocs_object, pocs_multi_index_type> pocs_index;
 }}

 FC_REFLECT_DERIVED (graphene::chain::pocs_object,
                     (graphene::chain::object),
                     (league_id)(account_id)(total_sell)(total_buy))
