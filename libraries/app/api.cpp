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
#include <cctype>

#include <graphene/app/api.hpp>
#include <graphene/app/api_access.hpp>
#include <graphene/app/application.hpp>
#include <graphene/chain/database.hpp>
#include <graphene/chain/get_config.hpp>
#include <graphene/utilities/key_conversion.hpp>
#include <graphene/chain/protocol/fee_schedule.hpp>
#include <graphene/chain/confidential_object.hpp>
#include <graphene/chain/market_object.hpp>
#include <graphene/chain/transaction_object.hpp>
#include <graphene/chain/withdraw_permission_object.hpp>
#include <graphene/chain/worker_object.hpp>
#include <graphene/chain/protocol/operations.hpp>

#include <fc/crypto/hex.hpp>
#include <fc/smart_ref_impl.hpp>

namespace graphene { namespace app {
    class network_broadcast_api;

    login_api::login_api(application& a)
    :_app(a)
    {
    }

    login_api::~login_api()
    {
    }

    bool login_api::login(const string& user, const string& password)
    {
       optional< api_access_info > acc = _app.get_api_access_info( user );
       if( !acc.valid() )
          return false;
       if( acc->password_hash_b64 != "*" )
       {
          std::string password_salt = fc::base64_decode( acc->password_salt_b64 );
          std::string acc_password_hash = fc::base64_decode( acc->password_hash_b64 );

          fc::sha256 hash_obj = fc::sha256::hash( password + password_salt );
          if( hash_obj.data_size() != acc_password_hash.length() )
             return false;
          if( memcmp( hash_obj.data(), acc_password_hash.c_str(), hash_obj.data_size() ) != 0 )
             return false;
       }

       for( const std::string& api_name : acc->allowed_apis )
          enable_api( api_name );
       return true;
    }

    void login_api::enable_api( const std::string& api_name )
    {
       if( api_name == "database_api" )
       {
          _database_api = std::make_shared< database_api >( std::ref( *_app.chain_database() ) );
       }
       else if( api_name == "block_api" )
       {
          _block_api = std::make_shared< block_api >( std::ref( *_app.chain_database() ) );
       }
       else if( api_name == "network_broadcast_api" )
       {
          _network_broadcast_api = std::make_shared< network_broadcast_api >( std::ref( _app ) );
       }
       else if( api_name == "history_api" )
       {
          _history_api = std::make_shared< history_api >( _app );
       }
       else if( api_name == "network_node_api" )
       {
          _network_node_api = std::make_shared< network_node_api >( std::ref(_app) );
       }
       else if( api_name == "crypto_api" )
       {
          _crypto_api = std::make_shared< crypto_api >();
       }
       else if( api_name == "asset_api" )
       {
          _asset_api = std::make_shared< asset_api >( std::ref( *_app.chain_database() ) );
       }
       else if( api_name == "debug_api" )
       {
          // can only enable this API if the plugin was loaded
          if( _app.get_plugin( "debug_witness" ) )
             _debug_api = std::make_shared< graphene::debug_witness::debug_api >( std::ref(_app) );
       }
       return;
    }

    // block_api
    block_api::block_api(graphene::chain::database& db) : _db(db) { }
    block_api::~block_api() { }

    vector<optional<signed_block>> block_api::get_blocks(uint32_t block_num_from, uint32_t block_num_to)const
    {
       FC_ASSERT( block_num_to >= block_num_from );
       vector<optional<signed_block>> res;
       for(uint32_t block_num=block_num_from; block_num<=block_num_to; block_num++) {
          res.push_back(_db.fetch_block_by_number(block_num));
       }
       return res;
    }


    network_node_api::network_node_api( application& a ) : _app( a )
    {
    }

    fc::variant_object network_node_api::get_info() const
    {
       fc::mutable_variant_object result = _app.p2p_node()->network_get_info();
       result["connection_count"] = _app.p2p_node()->get_connection_count();
       return result;
    }

    void network_node_api::add_node(const fc::ip::endpoint& ep)
    {
       _app.p2p_node()->add_node(ep);
    }

    std::vector<net::peer_status> network_node_api::get_connected_peers() const
    {
       return _app.p2p_node()->get_connected_peers();
    }

    std::vector<net::potential_peer_record> network_node_api::get_potential_peers() const
    {
       return _app.p2p_node()->get_potential_peers();
    }

    fc::variant_object network_node_api::get_advanced_node_parameters() const
    {
       return _app.p2p_node()->get_advanced_node_parameters();
    }

    void network_node_api::set_advanced_node_parameters(const fc::variant_object& params)
    {
       return _app.p2p_node()->set_advanced_node_parameters(params);
    }


    fc::api<network_broadcast_api> login_api::network_broadcast()const
    {
       FC_ASSERT(_network_broadcast_api);
       return *_network_broadcast_api;
    }

    fc::api<block_api> login_api::block()const
    {
       FC_ASSERT(_block_api);
       return *_block_api;
    }

    fc::api<network_node_api> login_api::network_node()const
    {
       FC_ASSERT(_network_node_api);
       return *_network_node_api;
    }

    fc::api<database_api> login_api::database()const
    {
       FC_ASSERT(_database_api);
       return *_database_api;
    }

    fc::api<history_api> login_api::history() const
    {
       FC_ASSERT(_history_api);
       return *_history_api;
    }

    fc::api<crypto_api> login_api::crypto() const
    {
       FC_ASSERT(_crypto_api);
       return *_crypto_api;
    }

    fc::api<asset_api> login_api::asset() const
    {
       FC_ASSERT(_asset_api);
       return *_asset_api;
    }

    fc::api<graphene::debug_witness::debug_api> login_api::debug() const
    {
       FC_ASSERT(_debug_api);
       return *_debug_api;
    }

    vector<operation_history_object> history_api::get_account_history( account_id_type account, 
                                                                       operation_history_id_type stop, 
                                                                       unsigned limit, 
                                                                       operation_history_id_type start ) const
    {
       FC_ASSERT( _app.chain_database() );
       const auto& db = *_app.chain_database();       
       FC_ASSERT( limit <= 100 );
       vector<operation_history_object> result;
       const auto& stats = account(db).statistics(db);
       if( stats.most_recent_op == account_transaction_history_id_type() ) return result;
       const account_transaction_history_object* node = &stats.most_recent_op(db);
       if( start == operation_history_id_type() )
          start = node->operation_id;
          
       while(node && node->operation_id.instance.value > stop.instance.value && result.size() < limit)
       {
          if( node->operation_id.instance.value <= start.instance.value )
             result.push_back( node->operation_id(db) );
          if( node->next == account_transaction_history_id_type() )
             node = nullptr;
          else node = &node->next(db);
       }
       
       if( stop.instance.value == 0 && result.size() < limit )
       {
          node = db.find(account_transaction_history_id_type());
          if( node && node->account == account)
             result.push_back( node->operation_id(db) );
       }
       return result;
    }

    history_operation_detail history_api::get_account_history_by_operations( account_id_type account,
                                                                                     vector<uint32_t> operation_indexs,
                                                                                     uint32_t start,
                                                                                     unsigned limit)
    {
        FC_ASSERT(limit <= 100 && limit > 0);
        uint32_t total = 0;
        history_operation_detail result;
        vector<operation_history_object> operation_history_objs_tmp;
        vector<operation_history_object> operation_history_objs;
        start = start > 0 ? start : 1;
        operation_history_objs_tmp = get_relative_account_history(account, start, limit, limit + start - 1);
        for(auto& operation_history_obj : operation_history_objs_tmp){
            ++total;
            if (operation_indexs.empty() || find(operation_indexs.begin(), operation_indexs.end(), operation_history_obj.op.which()) != operation_indexs.end()){
                operation_history_objs.push_back(operation_history_obj);
            }
        }
        result.total_without_operations = total;
        result.operation_history_objs = operation_history_objs;
        return result;
    }
    
    vector<operation_history_object> history_api::get_account_history_operations( account_id_type account, 
                                                                                  int operation_id,
                                                                                  operation_history_id_type start, 
                                                                                  operation_history_id_type stop,
                                                                                  unsigned limit) const
    {
       FC_ASSERT( _app.chain_database() );
       const auto& db = *_app.chain_database();
       FC_ASSERT( limit <= 100 );
       vector<operation_history_object> result;
       const auto& stats = account(db).statistics(db);
       if( stats.most_recent_op == account_transaction_history_id_type() ) return result;
       const account_transaction_history_object* node = &stats.most_recent_op(db);
       if( start == operation_history_id_type() )
          start = node->operation_id;

       while(node && node->operation_id.instance.value > stop.instance.value && result.size() < limit)
       {
          if( node->operation_id.instance.value <= start.instance.value ) {

             if(node->operation_id(db).op.which() == operation_id)
               result.push_back( node->operation_id(db) );
             }
          if( node->next == account_transaction_history_id_type() )
             node = nullptr;
          else node = &node->next(db);
       }
       if( stop.instance.value == 0 && result.size() < limit ) {
          const account_transaction_history_object head = account_transaction_history_id_type()(db);
          if( head.account == account && head.operation_id(db).op.which() == operation_id )
             result.push_back(head.operation_id(db));
       }
       return result;
    }


    vector<operation_history_object> history_api::get_relative_account_history( account_id_type account, 
                                                                                uint32_t stop, 
                                                                                unsigned limit, 
                                                                                uint32_t start) const
    {
        FC_ASSERT( _app.chain_database() );
        const auto& db = *_app.chain_database();
        FC_ASSERT(limit <= 100);
        vector<operation_history_object> result;
        const auto& stats = account(db).statistics(db);
        if( start == 0 )
            start = stats.total_ops;
        else
            start = min( stats.total_ops, start );

        if (start >= stop && start > stats.removed_ops && limit > 0)
        {
            const auto& hist_idx = db.get_index_type<account_transaction_history_index>();
            const auto& by_seq_idx = hist_idx.indices().get<by_seq>();

            auto itr = by_seq_idx.upper_bound( boost::make_tuple( account, start ) );
            auto itr_stop = by_seq_idx.lower_bound( boost::make_tuple( account, stop ) );
            FC_ASSERT(itr != itr_stop,"can't find history");
            do
            {
                --itr;
                result.push_back( itr->operation_id(db) );
            }
            while ( itr != itr_stop && result.size() < limit );
        }
        return result;
    }

    crypto_api::crypto_api(){};
    
    commitment_type crypto_api::blind( const blind_factor_type& blind, uint64_t value )
    {
       return fc::ecc::blind( blind, value );
    }
   
    blind_factor_type crypto_api::blind_sum( const std::vector<blind_factor_type>& blinds_in, uint32_t non_neg )
    {
       return fc::ecc::blind_sum( blinds_in, non_neg );
    }
   
    bool crypto_api::verify_sum( const std::vector<commitment_type>& commits_in, const std::vector<commitment_type>& neg_commits_in, int64_t excess )
    {
       return fc::ecc::verify_sum( commits_in, neg_commits_in, excess );
    }
    
    verify_range_result crypto_api::verify_range( const commitment_type& commit, const std::vector<char>& proof )
    {
       verify_range_result result;
       result.success = fc::ecc::verify_range( result.min_val, result.max_val, commit, proof );
       return result;
    }
    
    std::vector<char> crypto_api::range_proof_sign( uint64_t min_value, 
                                                    const commitment_type& commit, 
                                                    const blind_factor_type& commit_blind, 
                                                    const blind_factor_type& nonce,
                                                    int8_t base10_exp,
                                                    uint8_t min_bits,
                                                    uint64_t actual_value )
    {
       return fc::ecc::range_proof_sign( min_value, commit, commit_blind, nonce, base10_exp, min_bits, actual_value );
    }
                               
    verify_range_proof_rewind_result crypto_api::verify_range_proof_rewind( const blind_factor_type& nonce,
                                                                            const commitment_type& commit, 
                                                                            const std::vector<char>& proof )
    {
       verify_range_proof_rewind_result result;
       result.success = fc::ecc::verify_range_proof_rewind( result.blind_out, 
                                                            result.value_out, 
                                                            result.message_out, 
                                                            nonce, 
                                                            result.min_val, 
                                                            result.max_val, 
                                                            const_cast< commitment_type& >( commit ), 
                                                            proof );
       return result;
    }
                                    
    range_proof_info crypto_api::range_get_info( const std::vector<char>& proof )
    {
       return fc::ecc::range_get_info( proof );
    }

    // asset_api
    asset_api::asset_api(graphene::chain::database& db) : _db(db) { }
    asset_api::~asset_api() { }

    vector<account_asset_balance> asset_api::get_asset_holders( asset_id_type asset_id ) const {

      const auto& bal_idx = _db.get_index_type< account_balance_index >().indices().get< by_asset_balance >();
      auto range = bal_idx.equal_range( boost::make_tuple( asset_id ) );

      vector<account_asset_balance> result;

      for( const account_balance_object& bal : boost::make_iterator_range( range.first, range.second ) )
      {
        if( bal.balance.value == 0 ) continue;

        auto account = _db.find(bal.owner);

        account_asset_balance aab;
        aab.name       = account->name;
        aab.account_id = account->id;
        aab.amount     = bal.balance.value;

        result.push_back(aab);
      }

      return result;
    }
    // get number of asset holders.
    int asset_api::get_asset_holders_count( asset_id_type asset_id ) const {

      const auto& bal_idx = _db.get_index_type< account_balance_index >().indices().get< by_asset_balance >();
      auto range = bal_idx.equal_range( boost::make_tuple( asset_id ) );
            
      int count = boost::distance(range) - 1;

      return count;
    }
    // function to get vector of system assets with holders count.
    vector<asset_holders> asset_api::get_all_asset_holders() const {
            
      vector<asset_holders> result;
            
      vector<asset_id_type> total_assets;
      for( const asset_object& asset_obj : _db.get_index_type<asset_index>().indices() )
      {
        const auto& dasset_obj = asset_obj.dynamic_asset_data_id(_db);

        asset_id_type asset_id;
        asset_id = dasset_obj.id;

        const auto& bal_idx = _db.get_index_type< account_balance_index >().indices().get< by_asset_balance >();
        auto range = bal_idx.equal_range( boost::make_tuple( asset_id ) );

        int count = boost::distance(range) - 1;
                
        asset_holders ah;
        ah.asset_id       = asset_id;
        ah.count     = count;

        result.push_back(ah);
      }

      return result;
    }

} } // graphene::app
