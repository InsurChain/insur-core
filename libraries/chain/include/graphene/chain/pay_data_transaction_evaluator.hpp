#pragma once
#include <graphene/chain/protocol/operations.hpp>
#include <graphene/chain/evaluator.hpp>
#include <graphene/chain/database.hpp>

namespace graphene { namespace chain {

   class pay_data_transaction_evaluator : public evaluator<pay_data_transaction_evaluator>
   {
      public:
         typedef pay_data_transaction_operation operation_type;

         void_result do_evaluate( const pay_data_transaction_operation& op );
         operation_result do_apply(const pay_data_transaction_operation& op, int32_t billed_cpu_time_us = 0);

         void prepare_fee(account_id_type account_id, asset fee, const operation& o);

      private:
         void update_alliance_pocs(alliance_id_type alliance_id, object_id_type product_id, const pay_data_transaction_operation& op);
         share_type cut_fee(share_type a, uint16_t p);
         optional<account_object> get_account_by_name(const string& account_name);

      private:
        uint64_t transaction_fee = 0;
   };

} } // graphene::chain

