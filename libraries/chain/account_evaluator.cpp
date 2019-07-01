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

#include <fc/smart_ref_impl.hpp>
#include <graphene/chain/account_evaluator.hpp>
#include <graphene/chain/buyback.hpp>
#include <graphene/chain/buyback_object.hpp>
#include <graphene/chain/database.hpp>
#include <graphene/chain/exceptions.hpp>
#include <graphene/chain/hardfork.hpp>
#include <graphene/chain/internal_exceptions.hpp>
#include <graphene/chain/special_authority.hpp>
#include <graphene/chain/special_authority_object.hpp>
#include <algorithm>


namespace graphene { namespace chain {


void verify_authority_accounts( const database& db, const authority& a )
{
   const auto& chain_params = db.get_global_properties().parameters;
   GRAPHENE_ASSERT(
      a.num_auths() <= chain_params.maximum_authority_membership,
      internal_verify_auth_max_auth_exceeded,
      "Maximum authority membership exceeded" );
   for( const auto& acnt : a.account_auths )
   {
      GRAPHENE_ASSERT( db.find_object( acnt.first ) != nullptr,
         internal_verify_auth_account_not_found,
         "Account ${a} specified in authority does not exist",
         ("a", acnt.first) );
   }
}

void verify_account_votes( const database& db, const account_options& options )
{
   // ensure account's votes satisfy requirements
   // NB only the part of vote checking that requires chain state is here,
   // the rest occurs in account_options::validate()

   const auto& gpo = db.get_global_properties();
   const auto& chain_params = gpo.parameters;

   FC_ASSERT( options.num_witness <= chain_params.maximum_witness_count,
              "Voted for more witnesses than currently allowed (${c})", ("c", chain_params.maximum_witness_count) );
   FC_ASSERT( options.num_committee <= chain_params.maximum_committee_count,
              "Voted for more committee members than currently allowed (${c})", ("c", chain_params.maximum_committee_count) );

   FC_ASSERT(db.find_object(options.voting_account), "Invalid proxy account specified.");

   uint32_t max_vote_id = gpo.next_available_vote_id;
   for (auto id : options.votes) {
       FC_ASSERT(id < max_vote_id, "assert ${a} < ${b} failed", ("a", id)("b", max_vote_id));
   }
}

void_result account_create_evaluator::do_evaluate( const account_create_operation& op )
{ try {
   database& d = db();

   FC_ASSERT( fee_paying_account->is_lifetime_member(), "Only Lifetime members may register an account." );
   FC_ASSERT( op.referrer(d).is_member(d.head_block_time()), "The referrer must be either a lifetime or annual subscriber." );

   try
   {
      verify_authority_accounts( d, op.owner );
      verify_authority_accounts( d, op.active );
   }
   GRAPHENE_RECODE_EXC( internal_verify_auth_max_auth_exceeded, account_create_max_auth_exceeded )
   GRAPHENE_RECODE_EXC( internal_verify_auth_account_not_found, account_create_auth_account_not_found )

   if( op.extensions.value.owner_special_authority.valid() )
      evaluate_special_authority( d, *op.extensions.value.owner_special_authority );
   if( op.extensions.value.active_special_authority.valid() )
      evaluate_special_authority( d, *op.extensions.value.active_special_authority );
   if( op.extensions.value.buyback_options.valid() )
      evaluate_buyback_account_options( d, *op.extensions.value.buyback_options );
   verify_account_votes( d, op.options );

   auto& acnt_indx = d.get_index_type<account_index>();
   if( op.name.size() )
   {
      auto current_account_itr = acnt_indx.indices().get<by_name>().find( op.name );
      FC_ASSERT( current_account_itr == acnt_indx.indices().get<by_name>().end() );
   }

   return void_result();
} FC_CAPTURE_AND_RETHROW( (op) ) }

object_id_type account_create_evaluator::do_apply(const account_create_operation& o, uint32_t billed_cpu_time_us)
{ try {

   database& d = db();
   uint16_t referrer_percent = o.referrer_percent;
   bool has_small_percent = (
         (d.head_block_time() <= HARDFORK_453_TIME)
      && (o.referrer != o.registrar  )
      && (o.referrer_percent != 0    )
      && (o.referrer_percent <= 0x100)
      );

   if( has_small_percent )
   {
      if( referrer_percent >= 100 )
      {
         wlog( "between 100% and 0x100%:  ${o}", ("o", o) );
      }
      referrer_percent = referrer_percent*100;
      if( referrer_percent > GRAPHENE_100_PERCENT )
         referrer_percent = GRAPHENE_100_PERCENT;
   }

   const auto& global_properties = d.get_global_properties();

   const auto& new_acnt_object = d.create<account_object>( [&o, &d, &global_properties, referrer_percent]( account_object& obj ){
         obj.registrar = o.registrar;
         obj.referrer = o.referrer;
         obj.lifetime_referrer = o.referrer(d).lifetime_referrer;

         auto& params = global_properties.parameters;
         obj.network_fee_percentage = params.network_percent_of_fee;
         obj.lifetime_referrer_fee_percentage = params.lifetime_referrer_percent_of_fee;
         obj.referrer_rewards_percentage = referrer_percent;

         obj.name             = o.name;
         obj.owner            = o.owner;
         obj.active           = o.active;
         obj.options          = o.options;
         obj.statistics = d.create<account_statistics_object>([&](account_statistics_object& s){s.owner = obj.id;}).id;

         if( o.extensions.value.owner_special_authority.valid() )
            obj.owner_special_authority = *(o.extensions.value.owner_special_authority);
         if( o.extensions.value.active_special_authority.valid() )
            obj.active_special_authority = *(o.extensions.value.active_special_authority);
         if( o.extensions.value.buyback_options.valid() )
         {
            obj.allowed_assets = o.extensions.value.buyback_options->markets;
            obj.allowed_assets->emplace( o.extensions.value.buyback_options->asset_to_buy );
         }
   });

   /*
   if( has_small_percent )
   {
      wlog( "Account affected by #453 registered in block ${n}:  ${na} reg=${reg} ref=${ref}:${refp} ltr=${ltr}:${ltrp}",
         ("n", d.head_block_num()) ("na", new_acnt_object.id)
         ("reg", o.registrar) ("ref", o.referrer) ("ltr", new_acnt_object.lifetime_referrer)
         ("refp", new_acnt_object.referrer_rewards_percentage) ("ltrp", new_acnt_object.lifetime_referrer_fee_percentage) );
      wlog( "Affected account object is ${o}", ("o", new_acnt_object) );
   }
   */

   const auto& dynamic_properties = d.get_dynamic_global_properties();
   d.modify(dynamic_properties, [](dynamic_global_property_object& p) {
      ++p.accounts_registered_this_interval;
   });

   if( dynamic_properties.accounts_registered_this_interval % global_properties.parameters.accounts_per_fee_scale == 0
         && global_properties.parameters.account_fee_scale_bitshifts != 0 )
   {
      d.modify(global_properties, [](global_property_object& p) {
         p.parameters.current_fees->get<account_create_operation>().basic_fee <<= p.parameters.account_fee_scale_bitshifts;
      });
   }

   if(    o.extensions.value.owner_special_authority.valid()
       || o.extensions.value.active_special_authority.valid() )
   {
      d.create< special_authority_object >( [&]( special_authority_object& sa )
      {
         sa.account = new_acnt_object.id;
      } );
   }

   if( o.extensions.value.buyback_options.valid() )
   {
      asset_id_type asset_to_buy = o.extensions.value.buyback_options->asset_to_buy;

      d.create< buyback_object >( [&]( buyback_object& bo )
      {
         bo.asset_to_buy = asset_to_buy;
      } );

      d.modify( asset_to_buy(d), [&]( asset_object& a )
      {
         a.buyback_account = new_acnt_object.id;
      } );
   }

   return new_acnt_object.id;
} FC_CAPTURE_AND_RETHROW((o)) }


void_result account_update_evaluator::do_evaluate( const account_update_operation& o )
{ try {
   database& d = db();

   try
   {
      if( o.owner )  verify_authority_accounts( d, *o.owner );
      if( o.active ) verify_authority_accounts( d, *o.active );
   }
   GRAPHENE_RECODE_EXC( internal_verify_auth_max_auth_exceeded, account_update_max_auth_exceeded )
   GRAPHENE_RECODE_EXC( internal_verify_auth_account_not_found, account_update_auth_account_not_found )

   if( o.extensions.value.owner_special_authority.valid() )
      evaluate_special_authority( d, *o.extensions.value.owner_special_authority );
   if( o.extensions.value.active_special_authority.valid() )
      evaluate_special_authority( d, *o.extensions.value.active_special_authority );

   acnt = &o.account(d);

   if( o.new_options.valid() )
      verify_account_votes( d, *o.new_options );

   return void_result();
} FC_CAPTURE_AND_RETHROW( (o) ) }

void_result account_update_evaluator::do_apply(const account_update_operation& o, uint32_t billed_cpu_time_us)
{ try {
   database& d = db();
   bool sa_before, sa_after;
   d.modify( *acnt, [&](account_object& a){
      if( o.owner )
      {
         a.owner = *o.owner;
         a.top_n_control_flags = 0;
      }
      if( o.active )
      {
         a.active = *o.active;
         a.top_n_control_flags = 0;
      }
      if( o.new_options ) a.options = *o.new_options;
      sa_before = a.has_special_authority();
      if( o.extensions.value.owner_special_authority.valid() )
      {
         a.owner_special_authority = *(o.extensions.value.owner_special_authority);
         a.top_n_control_flags = 0;
      }
      if( o.extensions.value.active_special_authority.valid() )
      {
         a.active_special_authority = *(o.extensions.value.active_special_authority);
         a.top_n_control_flags = 0;
      }
      sa_after = a.has_special_authority();
   });

   if( sa_before & (!sa_after) )
   {
      const auto& sa_idx = d.get_index_type< special_authority_index >().indices().get<by_account>();
      auto sa_it = sa_idx.find( o.account );
      assert( sa_it != sa_idx.end() );
      d.remove( *sa_it );
   }
   else if( (!sa_before) & sa_after )
   {
      d.create< special_authority_object >( [&]( special_authority_object& sa )
      {
         sa.account = o.account;
      } );
   }

   return void_result();
} FC_CAPTURE_AND_RETHROW( (o) ) }

void_result account_whitelist_evaluator::do_evaluate(const account_whitelist_operation& o)
{ try {
   database& d = db();

   listed_account = &o.account_to_list(d);
   if( !d.get_global_properties().parameters.allow_non_member_whitelists )
      FC_ASSERT(o.authorizing_account(d).is_lifetime_member());

   return void_result();
} FC_CAPTURE_AND_RETHROW( (o) ) }

void_result account_whitelist_evaluator::do_apply(const account_whitelist_operation& o, uint32_t billed_cpu_time_us)
{ try {
   database& d = db();

   d.modify(*listed_account, [&o](account_object& a) {
      if( o.new_listing & o.white_listed )
         a.whitelisting_accounts.insert(o.authorizing_account);
      else
         a.whitelisting_accounts.erase(o.authorizing_account);

      if( o.new_listing & o.black_listed )
         a.blacklisting_accounts.insert(o.authorizing_account);
      else
         a.blacklisting_accounts.erase(o.authorizing_account);
   });

   /** for tracking purposes only, this state is not needed to evaluate */
   d.modify( o.authorizing_account(d), [&]( account_object& a ) {
     if( o.new_listing & o.white_listed )
        a.whitelisted_accounts.insert( o.account_to_list );
     else
        a.whitelisted_accounts.erase( o.account_to_list );

     if( o.new_listing & o.black_listed )
        a.blacklisted_accounts.insert( o.account_to_list );
     else
        a.blacklisted_accounts.erase( o.account_to_list );
   });

   return void_result();
} FC_CAPTURE_AND_RETHROW( (o) ) }

 void_result account_upgrade_evaluator::do_evaluate(const account_upgrade_evaluator::operation_type& o)
 { try {
    database& d = db();

    account = &d.get(o.account_to_upgrade);
    FC_ASSERT(!account->is_lifetime_member());

    return {};
 //} FC_CAPTURE_AND_RETHROW( (o) ) }
 } FC_RETHROW_EXCEPTIONS( error, "Unable to upgrade account '${a}'", ("a",o.account_to_upgrade(db()).name) ) }

 void_result account_upgrade_evaluator::do_apply(const account_upgrade_evaluator::operation_type& o, uint32_t billed_cpu_time_us)
 { try {
    database& d = db();

    d.modify(*account, [&](account_object& a) {
       if( o.upgrade_to_lifetime_member )
       {
          // Upgrade to lifetime member. I don't care what the account was before.
          a.statistics(d).process_fees(a, d);
          a.membership_expiration_date = time_point_sec::maximum();
          a.referrer = a.registrar = a.lifetime_referrer = a.get_id();
          a.lifetime_referrer_fee_percentage = GRAPHENE_100_PERCENT - a.network_fee_percentage;
       }
    });

    return {};
 } FC_RETHROW_EXCEPTIONS( error, "Unable to upgrade account '${a}'", ("a",o.account_to_upgrade(db()).name) ) }

void_result account_upgrade_merchant_evaluator::do_evaluate(const account_upgrade_merchant_evaluator::operation_type& o)
{ try {
   uint8_t op_version = 0;
   for (auto& ext : o.extensions) {
       if (ext.which() == future_extensions::tag<operation_ext_version_t>::value) {
           if (operation_version_one == ext.get<operation_ext_version_t>().version) {
               op_version = operation_version_one;
               break;
           }
       }
   }

   if (operation_version_one == op_version) {
       FC_ASSERT(trx_state->_is_proposed_trx);
       database& d = db();
       account = &d.get(o.account_to_upgrade);
       auth_referrer_account = &d.get(o.auth_referrer);
       FC_ASSERT(auth_referrer_account != 0);
       if (o.upgrade_to_merchant_member){
          FC_ASSERT(!account->is_merchant_member());
       }
       else{
          FC_ASSERT(account->is_merchant_member());
       }
   }
   else {
       database& d = db();
       account = &d.get(o.account_to_upgrade);
       auth_referrer_account = &d.get(o.auth_referrer);
       FC_ASSERT(auth_referrer_account  != 0);
       FC_ASSERT(auth_referrer_account->is_lifetime_member());
       if (o.upgrade_to_merchant_member){
          FC_ASSERT(!account->is_merchant_member());
       }
       else{
          FC_ASSERT(account->is_merchant_member());
       }
   }

   return {};
} FC_RETHROW_EXCEPTIONS( error, "Unable to upgrade merchant account '${a}'", ("a",o.account_to_upgrade(db()).name) ) }

void_result account_upgrade_merchant_evaluator::do_apply(const account_upgrade_merchant_evaluator::operation_type& o, uint32_t billed_cpu_time_us)
{ try {
   database& d = db();
   d.modify(*account, [&](account_object& a) {
      if( o.upgrade_to_merchant_member ){
         // Upgrade to merchant member. I don't care what the account was before.
         a.statistics(d).process_fees(a, d);
         a.merchant_auth_referrer = o.auth_referrer;
         a.merchant_expiration_date = time_point_sec::maximum();
      }
      else{
         a.statistics(d).process_fees(a, d);
         a.merchant_auth_referrer = o.auth_referrer;
         a.merchant_expiration_date = time_point_sec::min();
      }

   });
   return {};
} FC_RETHROW_EXCEPTIONS( error, "Unable to upgrade merchant account '${a}'", ("a",o.account_to_upgrade(db()).name) ) }

void_result account_upgrade_data_transaction_member_evaluator::do_evaluate(const account_upgrade_data_transaction_member_evaluator::operation_type& o)
{ try {
    dlog("account_upgrade_data_transaction_member do_evaluate");
    FC_ASSERT(trx_state->_is_proposed_trx);

    database& d = db();
    account = &d.get(o.account_to_upgrade);
    if (o.upgrade_to_data_transaction_member){
       FC_ASSERT(!account->is_data_transaction_member());
    }

    return {};
} FC_RETHROW_EXCEPTIONS( error, "Unable to upgrade data_transaction_member account '${a}'", ("a",o.account_to_upgrade(db()).name) ) }

void_result account_upgrade_data_transaction_member_evaluator::do_apply(const account_upgrade_data_transaction_member_evaluator::operation_type& o, uint32_t billed_cpu_time_us)
{ try {
    dlog("account_upgrade_data_transaction_member do_apply");
    database& d = db();
    d.modify(*account, [&](account_object& a) {
       if (o.upgrade_to_data_transaction_member)
       {
          a.statistics(d).process_fees(a, d);
          a.data_transaction_member_expiration_date = time_point_sec::maximum();
       }

    });
    return {};
} FC_RETHROW_EXCEPTIONS( error, "Unable to upgrade data_transaction_member account '${a}'", ("a",o.account_to_upgrade(db()).name) ) }

 void_result account_upgrade_datasource_evaluator::do_evaluate(const account_upgrade_datasource_evaluator::operation_type& o)
 { try {
   dlog("account_upgrade_datasource do_evaluate");
   uint8_t op_version = 0;
   for (auto& ext : o.extensions) {
       if (ext.which() == future_extensions::tag<operation_ext_version_t>::value) {
           if (operation_version_one == ext.get<operation_ext_version_t>().version) {
               op_version = operation_version_one;
               break;
           }
       }
   }
   if (operation_version_one == op_version) {
       FC_ASSERT(trx_state->_is_proposed_trx);
       database& d = db();
       account = &d.get(o.account_to_upgrade);
       auth_referrer_account = &d.get(o.auth_referrer);
       FC_ASSERT(auth_referrer_account != 0);
       FC_ASSERT(account->is_merchant_member());
       if (o.upgrade_to_datasource_member){
          FC_ASSERT(!account->is_datasource_member());
       }
       else{
          FC_ASSERT(account->is_datasource_member());
       }
   }
   else {
       database& d = db();
       account = &d.get(o.account_to_upgrade);
       auth_referrer_account = &d.get(o.auth_referrer);
       FC_ASSERT(auth_referrer_account  != 0);
       FC_ASSERT(auth_referrer_account->is_lifetime_member());
       FC_ASSERT(account->is_merchant_member());
       if (o.upgrade_to_datasource_member){
          FC_ASSERT(!account->is_datasource_member());
       }
       else{
          FC_ASSERT(account->is_datasource_member());
       }
   }
    return {};
 //} FC_CAPTURE_AND_RETHROW( (o) ) }
 } FC_RETHROW_EXCEPTIONS( error, "Unable to upgrade datasource account '${a}'", ("a",o.account_to_upgrade(db()).name) ) }

 void_result account_upgrade_datasource_evaluator::do_apply(const account_upgrade_datasource_evaluator::operation_type& o, uint32_t billed_cpu_time_us)
 { try {
    database& d = db();
    d.modify(*account, [&](account_object& a) {
       if( o.upgrade_to_datasource_member ){
          // Upgrade to datasource member. I don't care what the account was before.
          a.statistics(d).process_fees(a, d);
          a.datasource_auth_referrer = o.auth_referrer;
          a.datasource_expiration_date = time_point_sec::maximum();
       }
       else{
          a.statistics(d).process_fees(a, d);
          a.datasource_auth_referrer = o.auth_referrer;
          a.datasource_expiration_date = time_point_sec::min(); 
       }

    });
    return {};
 } FC_RETHROW_EXCEPTIONS( error, "Unable to upgrade datasource account '${a}'", ("a",o.account_to_upgrade(db()).name) ) }

} } // graphene::chain
