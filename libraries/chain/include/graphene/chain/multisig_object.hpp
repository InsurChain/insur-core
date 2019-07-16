/*
 * Copyright (c) 2018 oxarbitrage and contributors.
 *
 * The MIT License
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */
#pragma once

#include <graphene/chain/protocol/types.hpp>
#include <graphene/db/object.hpp>
#include <graphene/db/generic_index.hpp>

namespace graphene { namespace chain {

      /**
       * Temporally 
       */
      class multisig_object : public graphene::db::abstract_object<multisig_object> {
         public:
            static const uint8_t space_id = implementation_ids;
            static const uint8_t type_id  = impl_multisig_object_type;

            account_id_type         from;
            asset                   amount;
            string                  first;
            string                  second;
            string                  third;
            string                  fourth;
            string                  fifth;
            string                  sixth;
            string                  seventh;
      };

      struct by_from_first_id;
      struct by_from_second_id;
      struct by_from_third_id;
      struct by_from_fourth_id;
      struct by_from_fifth_id;
      struct by_from_sixth_id;
      struct by_from_seventh_id;
      typedef multi_index_container<
         multisig_object,
         indexed_by<
            ordered_unique< tag< by_id >, member< object, object_id_type, &object::id > >,

            ordered_unique< tag< by_from_first_id >,
               composite_key< multisig_object,
                  member< multisig_object, account_id_type,  &multisig_object::from >,
                  member< multisig_object, string, &multisig_object::first >
               >
            >,
            ordered_non_unique< tag< by_from_second_id >,
               composite_key< multisig_object,
                  member< multisig_object, account_id_type,  &multisig_object::from >,
                  member< multisig_object, string, &multisig_object::second >
               >
            >,
            ordered_non_unique< tag< by_from_third_id >,
               composite_key< multisig_object,
                  member< multisig_object, account_id_type,  &multisig_object::from >,
                  member< multisig_object, string, &multisig_object::third >
               >
            >,

            ordered_non_unique< tag< by_from_fourth_id >,
               composite_key< multisig_object,
                  member< multisig_object, account_id_type,  &multisig_object::from >,
                  member< multisig_object, string, &multisig_object::fourth >
               >
            >,
            ordered_non_unique< tag< by_from_fifth_id >,
               composite_key< multisig_object,
                  member< multisig_object, account_id_type,  &multisig_object::from >,
                  member< multisig_object, string, &multisig_object::fifth >
               >
            >,
            ordered_non_unique< tag< by_from_sixth_id >,
               composite_key< multisig_object,
                  member< multisig_object, account_id_type,  &multisig_object::from >,
                  member< multisig_object, string, &multisig_object::sixth >
               >
            >,
            ordered_non_unique< tag< by_from_seventh_id >,
               composite_key< multisig_object,
                  member< multisig_object, account_id_type,  &multisig_object::from >,
                  member< multisig_object, string, &multisig_object::seventh >
               >
            >
         >

      > multisig_object_index_type;

      typedef generic_index< multisig_object, multisig_object_index_type > multisig_index;

   } }

FC_REFLECT_DERIVED( graphene::chain::multisig_object, (graphene::db::object),
(from)(amount)(first)(second)(third)(fourth)(fifth)(sixth)(seventh ));
