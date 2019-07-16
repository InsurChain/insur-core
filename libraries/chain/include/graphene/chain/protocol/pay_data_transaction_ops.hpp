#pragma once
#include <graphene/chain/protocol/base.hpp>

namespace graphene { namespace chain {

   /**
    * @ingroup operations
    *
    * @brief Transfers an amount of one asset from one account to another
    *
    *  Fees are paid by the "from" account
    *
    *  @pre amount.amount > 0
    *  @pre fee.amount >= 0
    *  @pre from != to
    *  @post from account's balance will be reduced by fee and amount
    *  @post to account's balance will be increased by amount
    *  @return n/a
    */
   struct pay_data_transaction_operation : public base_operation
   {
      struct fee_parameters_type {
         uint64_t fee = GRAPHENE_BLOCKCHAIN_PRECISION  / 1000;
      };

      asset                         fee;
      /// Account to pay_data_transaction asset from
      account_id_type               from;
      /// Account to pay_data_transaction asset to
      account_id_type               to;
      asset                         amount;
      fc::string                    request_id;
      extensions_type               extensions;

      account_id_type fee_payer() const { return from; }
      void validate() const {
          FC_ASSERT( fee.amount >= 0 );
          FC_ASSERT( from != to );
          FC_ASSERT( amount.amount > 0 );
      }
      share_type calculate_fee(const fee_parameters_type& k) const {
          return k.fee;
      }
   };

}} // graphene::chain

FC_REFLECT(graphene::chain::pay_data_transaction_operation::fee_parameters_type, (fee))
FC_REFLECT(graphene::chain::pay_data_transaction_operation, (fee)(from)(to)(amount)(request_id)(extensions))
