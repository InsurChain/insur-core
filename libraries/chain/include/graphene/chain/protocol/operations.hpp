/*
 * Copyright (c) 2015 Cryptonomex, Inc., and contributors.
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
#include <graphene/chain/protocol/base.hpp>
#include <graphene/chain/protocol/account.hpp>
#include <graphene/chain/protocol/assert.hpp>
#include <graphene/chain/protocol/asset_ops.hpp>
#include <graphene/chain/protocol/balance.hpp>
#include <graphene/chain/protocol/custom.hpp>
#include <graphene/chain/protocol/committee_member.hpp>
#include <graphene/chain/protocol/confidential.hpp>
#include <graphene/chain/protocol/fba.hpp>
#include <graphene/chain/protocol/market.hpp>
#include <graphene/chain/protocol/proposal.hpp>
#include <graphene/chain/protocol/transfer.hpp>
#include <graphene/chain/protocol/vesting.hpp>
#include <graphene/chain/protocol/pnt_transfer.hpp>
#include <graphene/chain/protocol/withdraw_permission.hpp>
#include <graphene/chain/protocol/witness.hpp>
#include <graphene/chain/protocol/worker.hpp>
#include <graphene/chain/protocol/stale_ops.hpp>
#include <graphene/chain/protocol/data_market_ops.hpp>
#include <graphene/chain/protocol/data_transaction_ops.hpp>
#include <graphene/chain/protocol/free_data_product_ops.hpp>
#include <graphene/chain/protocol/alliance_data_product_ops.hpp>
#include <graphene/chain/protocol/alliance_ops.hpp>
#include <graphene/chain/protocol/pay_data_transaction_ops.hpp>
#include <graphene/chain/protocol/datasource_copyright_ops.hpp>
#include <graphene/chain/protocol/loyalty_ops.hpp>
#include <graphene/chain/protocol/proxy_transfer_ops.hpp>
#include <graphene/chain/protocol/contract_ops.hpp>
#include <graphene/chain/protocol/data_storage_ops.hpp>
#include <graphene/chain/protocol/escrow.hpp>


namespace graphene { namespace chain {

   /**
    * @ingroup operations
    *
    * Defines the set of valid operations as a discriminated union type.
    */
   typedef fc::static_variant<
            transfer_operation,
            limit_order_create_operation,
            limit_order_cancel_operation,
            call_order_update_operation,
            fill_order_operation,           // VIRTUAL
            account_create_operation,
            account_update_operation,
            account_whitelist_operation,
            account_upgrade_operation,
            account_transfer_operation,
            asset_create_operation,
            asset_update_operation,
            asset_update_bitasset_operation,
            asset_update_feed_producers_operation,
            asset_issue_operation,
            asset_reserve_operation,
            asset_fund_fee_pool_operation,
            asset_settle_operation,
            asset_global_settle_operation,
            asset_publish_feed_operation,
            witness_create_operation,
            witness_update_operation,
            proposal_create_operation,
            proposal_update_operation,
            proposal_delete_operation,
            withdraw_permission_create_operation,
            withdraw_permission_update_operation,
            withdraw_permission_claim_operation,
            withdraw_permission_delete_operation,
            committee_member_create_operation,
            committee_member_update_operation,
            committee_member_update_global_parameters_operation,
            vesting_balance_create_operation,
            vesting_balance_withdraw_operation,
            worker_create_operation,
            custom_operation,
            assert_operation,
            balance_claim_operation,
            override_transfer_operation,
            transfer_to_blind_operation,
            blind_transfer_operation,
            transfer_from_blind_operation,
            asset_settle_cancel_operation,  // VIRTUAL
            asset_claim_fees_operation,
            fba_distribute_operation,        // VIRTUAL
            pnt_transfer_operation,
            alliance_data_product_create_operation,
            alliance_data_product_update_operation,
            alliance_create_operation,
            alliance_update_operation,
            escrow_transfer_operation,
            escrow_approve_operation,
            escrow_dispute_operation,
            escrow_release_operation,
            account_upgrade_merchant_operation,
            account_upgrade_datasource_operation,
            stale_data_market_category_create_operation,
            stale_data_market_category_update_operation,
            stale_free_data_product_create_operation,
            stale_free_data_product_update_operation,
            stale_alliance_data_product_create_operation,
            stale_alliance_data_product_update_operation,
            stale_alliance_create_operation,
            stale_alliance_update_operation,
            data_transaction_create_operation, 
            data_transaction_update_operation, 
            pay_data_transaction_operation,  
            account_upgrade_data_transaction_member_operation, 
            data_transaction_datasource_upload_operation, 
            data_transaction_datasource_validate_error_operation, 
            data_market_category_create_operation,
            data_market_category_update_operation,
            free_data_product_create_operation,
            free_data_product_update_operation,
            datasource_copyright_clear_operation,
            data_transaction_complain_operation,
            balance_lock_operation,
            balance_unlock_operation,
            proxy_transfer_operation, 
            contract_deploy_operation, 
            contract_call_operation, 
            contract_update_operation, 
            trust_node_pledge_withdraw_operation, 
            inline_transfer_operation, 
            inter_contract_call_operation 
         > operation;

   /// @} // operations group

   /**
    *  Appends required authorites to the result vector.  The authorities appended are not the
    *  same as those returned by get_required_auth 
    *
    *  @return a set of required authorities for @ref op
    */
   void operation_get_required_authorities( const operation& op, 
                                            flat_set<account_id_type>& active,
                                            flat_set<account_id_type>& owner,
                                            vector<authority>&  other );

   void operation_validate( const operation& op );

   /**
    *  @brief necessary to support nested operations inside the proposal_create_operation
    */
   struct op_wrapper
   {
      public:
         op_wrapper(const operation& op = operation()):op(op){}
         operation op;
   };

} } // graphene::chain

FC_REFLECT_TYPENAME( graphene::chain::operation )
FC_REFLECT( graphene::chain::op_wrapper, (op) )
   
