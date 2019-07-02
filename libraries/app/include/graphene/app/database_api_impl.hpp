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

#include <graphene/app/full_account.hpp>
#include <graphene/chain/protocol/types.hpp>
#include <graphene/chain/database.hpp>
#include <graphene/chain/account_object.hpp>
#include <graphene/chain/asset_object.hpp>
#include <graphene/chain/balance_object.hpp>
#include <graphene/chain/chain_property_object.hpp>
#include <graphene/chain/committee_member_object.hpp>
#include <graphene/chain/confidential_object.hpp>
#include <graphene/chain/market_object.hpp>
#include <graphene/chain/operation_history_object.hpp>
#include <graphene/chain/proposal_object.hpp>
#include <graphene/chain/worker_object.hpp>
#include <graphene/chain/witness_object.hpp>
#include <graphene/chain/data_market_object.hpp>
#include <graphene/chain/data_transaction_object.hpp>
#include <graphene/chain/second_hand_data_object.hpp>
#include <graphene/app/database_api_common.hpp>
#include <graphene/chain/pocs_object.hpp>
#include <graphene/chain/transaction_entry_object.hpp>

#include <fc/api.hpp>
#include <fc/optional.hpp>
#include <fc/variant_object.hpp>
#include <fc/bloom_filter.hpp>
#include <fc/smart_ref_impl.hpp>

#include <boost/container/flat_set.hpp>

#include <functional>
#include <map>
#include <memory>
#include <vector>

namespace graphene { namespace app {

using namespace graphene::chain;
using namespace std;

typedef std::map< std::pair<graphene::chain::asset_id_type, graphene::chain::asset_id_type>, std::vector<fc::variant> > market_queue_type;

typedef pair<account_id_type, uint64_t> PAIR;

struct cmp_pair_by_value {
   bool operator()(const PAIR& lhs, const PAIR& rhs) {
       return lhs.second > rhs.second;
   }
};

class database_api_impl : public std::enable_shared_from_this<database_api_impl>
{
   public:
      explicit database_api_impl( graphene::chain::database& db );
      ~database_api_impl();

      // Objects
      fc::variants get_objects(const vector<object_id_type>& ids)const;
      get_table_rows_result get_table_rows_ex(string contract, string table, const get_table_rows_params &params) const;
      get_table_rows_result get_table_rows(string contract, string table, uint64_t start, uint64_t limit=10) const;
      fc::variants get_table_objects(uint64_t code, uint64_t scope, uint64_t table, uint64_t lower, uint64_t uppper, uint64_t limit=10) const;
      bytes serialize_contract_call_args(string contract, string method, string json_args) const;

      // Subscriptions
      void set_subscribe_callback( std::function<void(const variant&)> cb, bool notify_remove_create );
      void set_data_transaction_subscribe_callback( std::function<void(const variant&)> cb, bool notify_remove_create );
      void set_data_transaction_products_subscribe_callback(std::function<void(const variant&)> cb, vector<object_id_type> ids);
      void set_pending_transaction_callback( std::function<void(const variant&)> cb );
      void set_block_applied_callback( std::function<void(const variant& block_id)> cb );
      void cancel_all_subscriptions();
      void unsubscribe_data_transaction_callback();

      // Blocks and transactions
      optional<block_header> get_block_header(uint32_t block_num)const;
      optional<signed_block_with_info> get_block(uint32_t block_num)const;
      optional<signed_block_with_info> get_block_by_id(block_id_type block_id)const;
      processed_transaction get_transaction( uint32_t block_num, uint32_t trx_in_block )const;
      optional<processed_transaction> get_transaction_rows(transaction_id_type txid)const;

      // Globals
      chain_property_object get_chain_properties()const;
      global_property_object get_global_properties()const;
      data_transaction_commission_percent_t get_commission_percent() const;
      vm_cpu_limit_t get_cpu_limit() const;
      fc::variant_object get_config()const;
      chain_id_type get_chain_id()const;
      dynamic_global_property_object get_dynamic_global_properties()const;

      // Keys
      vector<vector<account_id_type>> get_key_references( vector<public_key_type> key )const;
     bool is_public_key_registered(string public_key) const;

      state_snapshot_result create_snapshot() const;

      // Accounts
      vector<optional<account_object>> get_accounts(const vector<account_id_type>& account_ids)const;
      std::map<string,full_account> get_full_accounts( const vector<string>& names_or_ids, bool subscribe );
      optional<account_object> get_account_by_name( string name )const;
      optional<account_object> get_account_by_contract_code(uint64_t code)const;
      vector<account_id_type> get_account_references( account_id_type account_id )const;
      vector<optional<account_object>> lookup_account_names(const vector<string>& account_names)const;
      map<string,account_id_type> lookup_accounts(const string& lower_bound_name, uint32_t limit)const;
      uint64_t get_transaction_count() const;
      uint64_t get_account_count() const;
      uint64_t get_asset_count() const;
      bool is_account_registered(string name) const;


      // Balances
      vector<asset> get_account_balances(account_id_type id, const flat_set<asset_id_type>& assets)const;
      vector<asset> get_account_lock_balances(account_id_type id, const flat_set<asset_id_type>& assets)const;
      vector<asset> get_named_account_balances(const std::string& name, const flat_set<asset_id_type>& assets)const;
      vector<balance_object> get_balance_objects( const vector<address>& addrs )const;
      vector<asset> get_vested_balances( const vector<balance_id_type>& objs )const;
      vector<vesting_balance_object> get_vesting_balances( account_id_type account_id )const;

      // Assets
      vector<optional<asset_object>> get_assets(const vector<asset_id_type>& asset_ids)const;
      vector<asset_object>           list_assets(const string& lower_bound_symbol, uint32_t limit)const;
      vector<optional<asset_object>> lookup_asset_symbols(const vector<string>& symbols_or_ids)const;

      optional<pocs_object>              get_pocs_object(alliance_id_type alliance_id, account_id_type account_id, object_id_type product_id) const;

      // Witnesses
      vector<optional<witness_object>> get_witnesses(const vector<witness_id_type>& witness_ids)const;
      fc::optional<witness_object> get_witness_by_account(account_id_type account)const;
      map<string, witness_id_type> lookup_witness_accounts(const string& lower_bound_name, uint32_t limit)const;
      uint64_t get_witness_count()const;

      vector<account_id_type> get_trust_nodes() const;

      // Committee members
      vector<optional<committee_member_object>> get_committee_members(const vector<committee_member_id_type>& committee_member_ids)const;
      fc::optional<committee_member_object> get_committee_member_by_account(account_id_type account)const;
      map<string, committee_member_id_type> lookup_committee_member_accounts(const string& lower_bound_name, uint32_t limit)const;
      uint64_t get_committee_member_count() const;

      // Votes
      vector<variant> lookup_vote_ids( const vector<vote_id_type>& votes )const;

      // statistics
      uint64_t get_data_transaction_product_costs(fc::time_point_sec start, fc::time_point_sec end) const;
      uint64_t get_data_transaction_total_count(fc::time_point_sec start, fc::time_point_sec end) const; 
      uint64_t get_merchants_total_count() const;
      uint64_t get_data_transaction_commission(fc::time_point_sec start, fc::time_point_sec end) const;
      uint64_t get_data_transaction_pay_fee(fc::time_point_sec start, fc::time_point_sec end) const;      
      uint64_t get_data_transaction_product_costs_by_requester(string requester, fc::time_point_sec start, fc::time_point_sec end) const;
      uint64_t get_data_transaction_total_count_by_requester(string requester, fc::time_point_sec start, fc::time_point_sec end) const; 
      uint64_t get_data_transaction_pay_fees_by_requester(string requester, fc::time_point_sec start, fc::time_point_sec end) const;
      uint64_t get_data_transaction_product_costs_by_product_id(string product_id, fc::time_point_sec start, fc::time_point_sec end) const;
      uint64_t get_data_transaction_total_count_by_product_id(string product_id, fc::time_point_sec start, fc::time_point_sec end) const; 
      map<account_id_type, uint64_t> list_data_transaction_complain_requesters(fc::time_point_sec start_date_time, fc::time_point_sec end_date_time, uint8_t limit) const;
      
      map<account_id_type, uint64_t> list_data_transaction_complain_datasources(fc::time_point_sec start_date_time, fc::time_point_sec end_date_time, uint8_t limit) const;

      // Authority / validation
      std::string get_transaction_hex(const signed_transaction& trx)const;
      std::string get_unsigned_transaction_hex(const transaction &trx) const;
      std::string serialize_transaction(const signed_transaction& trx) const;
      set<public_key_type> get_required_signatures( const signed_transaction& trx, const flat_set<public_key_type>& available_keys )const;
      set<public_key_type> get_potential_signatures( const signed_transaction& trx )const;
      set<address> get_potential_address_signatures( const signed_transaction& trx )const;
      bool verify_authority( const signed_transaction& trx )const;
      bool verify_account_authority( const string& name_or_id, const flat_set<public_key_type>& signers )const;
      processed_transaction validate_transaction( const signed_transaction& trx )const;
      vector< fc::variant > get_required_fees( const vector<operation>& ops, asset_id_type id )const;

      // Proposed transactions
     vector<proposal_object> get_proposed_transactions( account_id_type id )const;

      // Blinded balances
      vector<blinded_balance_object> get_blinded_balances( const flat_set<commitment_type>& commitments )const;


      //data market interface

      /**
       *
       * a test function
       * @see get_test()
       * @brief get_test
       * @return
       */
      uint64_t                          get_test() const;

    /**
     * @brief list_home_free_data_products
     * @param num
     * @return
     */
    vector<free_data_product_object>  list_home_free_data_products(uint8_t limit) const;
    /**
     *
     * @brief list_home_alliances
     * @param num
     * @return
     */
    vector<alliance_object>  list_home_alliances(uint8_t limit) const;
    /**
     *
     * @brief list_home_alliance_products
     * @return
     */
    vector<data_market_category_object> list_data_market_categories(uint8_t data_market_type) const;

    free_data_product_search_results_object   list_free_data_products(string data_market_category_id,uint32_t offset,uint32_t limit,string order_by,string keyword,bool show_all = false) const;
    alliance_data_product_search_results_object   list_alliance_data_products(string data_market_category_id,uint32_t offset,uint32_t limit,string order_by,string keyword,bool show_all = false) const;


    /**
     * @brief list_search_alliances
     * @return
     */
    alliance_search_results_object  list_alliances(string data_market_category_id,uint32_t offset,uint32_t limit,string order_by,string keyword,bool show_all = false) const;

    optional<data_transaction_object> get_data_transaction_by_request_id(string request_id) const;
    data_transaction_search_results_object list_data_transactions_by_requester(string requester, uint32_t limit) const;

    map<account_id_type, uint64_t> list_second_hand_datasources(time_point_sec start_date_time, time_point_sec end_date_time, uint32_t limit) const;
    uint32_t list_total_second_hand_transaction_counts_by_datasource(fc::time_point_sec start_date_time, fc::time_point_sec end_date_time, account_id_type datasource_account) const;

    vector<optional<data_market_category_object>> get_data_market_categories(const vector<data_market_category_id_type>& data_market_category_ids)const;
    vector<optional<free_data_product_object>> get_free_data_products(const vector<free_data_product_id_type>& free_data_product_ids)const;
    vector<optional<alliance_data_product_object>> get_alliance_data_products(const vector<alliance_data_product_id_type>& alliance_data_product_ids)const;

    /**
     * @brief get_alliances
     * @return
     */
    vector<optional<alliance_object> > get_alliances(const vector<alliance_id_type>& alliance_ids) const;

    /**
     * @brief get witness participation rate
     * @return uint32_t
     */   
    uint32_t get_witness_participation_rate() const;

   //private:
      template<typename T>
      void subscribe_to_item( const T& i )const
      {
         auto vec = fc::raw::pack(i);
         if( !_subscribe_callback )
            return;

         if( !is_subscribed_to_item(i) )
         {
            _subscribe_filter.insert( vec.data(), vec.size() );//(vecconst char*)&i, sizeof(i) );
         }
      }

      template<typename T>
      bool is_subscribed_to_item( const T& i )const
      {
         if( !_subscribe_callback )
            return false;

         return _subscribe_filter.contains( i );
      }

      bool is_impacted_account( const flat_set<account_id_type>& accounts)
      {
         if( !_subscribed_accounts.size() || !accounts.size() )
            return false;

         return std::any_of(accounts.begin(), accounts.end(), [this](const account_id_type& account) {
            return _subscribed_accounts.find(account) != _subscribed_accounts.end();
         });
      }

      void broadcast_updates( const vector<variant>& updates );
      void handle_object_changed(bool force_notify, bool full_object, const vector<object_id_type>& ids, const flat_set<account_id_type>& impacted_accounts, std::function<const object*(object_id_type id)> find_object);
      
      /** called every time a block is applied to report the objects that were changed */
      void on_objects_new(const vector<object_id_type>& ids, const flat_set<account_id_type>& impacted_accounts);
      void on_objects_changed(const vector<object_id_type>& ids, const flat_set<account_id_type>& impacted_accounts);
      void on_objects_removed(const vector<object_id_type>& ids, const vector<const object*>& objs, const flat_set<account_id_type>& impacted_accounts);
      void on_applied_block();

      void broadcast_data_transaction_updates(const fc::variant& update);
      void on_data_transaction_objects_changed(const string& request_id);

      bool _notify_remove_create = false;
      mutable fc::bloom_filter _subscribe_filter;
      std::set<account_id_type> _subscribed_accounts;

      // for data transaction subscribe
      std::function<void(const fc::variant&)> _data_transaction_subscribe_callback;
      std::function<void(const fc::variant&)> _data_transaction_products_subscribe_callback;
      std::vector<object_id_type> _data_transaction_subscribe_products;
      boost::signals2::scoped_connection                                                                                           _data_transaction_change_connection;

      std::function<void(const fc::variant&)> _subscribe_callback;
      std::function<void(const fc::variant&)> _pending_trx_callback;
      std::function<void(const fc::variant&)> _block_applied_callback;

      boost::signals2::scoped_connection                                                                                           _new_connection;
      boost::signals2::scoped_connection                                                                                           _change_connection;
      boost::signals2::scoped_connection                                                                                           _removed_connection;
      boost::signals2::scoped_connection                                                                                           _applied_block_connection;
      boost::signals2::scoped_connection                                                                                           _pending_trx_connection;
      graphene::chain::database&                                                                                                   _db;


      string alliance_img_1;
      string alliance_img_2;
      string alliance_img_3;
      string alliance_img_4;
      string alliance_img_5;
      string data_img_1;
      string data_img_2;
      string data_img_3;
      string data_img_4;
      string data_img_5;
      string data_img_6;
      string data_img_7;
      string data_img_8;

}; // database_api_impl class

}}
