#pragma once

#include <graphene/chain/authority.hpp>
#include <graphene/chain/types.hpp>

namespace graphene { namespace chain {

using action_name    = graphene::chain::action_name;

struct newaccount {
   account_name                     creator;
   account_name                     name;
   authority                        owner;
   authority                        active;

   static action_name get_name() {
      return N(newaccount);
   }

   static account_name get_account() {
      return config::system_account_name;
   }

};


struct setabi {
   account_name                     account;
   bytes                            abi;

   static action_name get_name() {
      return N(setabi);
   }

   static account_name get_account() {
      return config::system_account_name;
   }
};

struct setcode {
   account_name                     account;
   uint8_t                          vmtype = 0;
   uint8_t                          vmversion = 0;
   bytes                            code;

   static action_name get_name() {
      return N(setcode);
   }

   static account_name get_account() {
      return config::system_account_name;
   }
};

struct updateauth {
   account_name                      account;
   permission_name                   permission;
   permission_name                   parent;
   authority                         auth;

   static action_name get_name() {
      return N(updateauth);
   }

   static account_name get_account() {
      return config::system_account_name;
   }
};


struct linkauth {
   linkauth() = default;
   linkauth(const account_name& account, const account_name& code, const action_name& type, const permission_name& requirement)
   :account(account), code(code), type(type), requirement(requirement)
   {}

   account_name                      account;
   account_name                      code;
   action_name                       type;
   permission_name                   requirement;

   static action_name get_name() {
      return N(linkauth);
   }

   static account_name get_account() {
      return config::system_account_name;
   }
};

struct unlinkauth {
   unlinkauth() = default;
   unlinkauth(const account_name& account, const account_name& code, const action_name& type)
   :account(account), code(code), type(type)
   {}

   account_name                      account;
   account_name                      code;
   action_name                       type;

   static action_name get_name() {
      return N(unlinkauth);
   }

   static account_name get_account() {
      return config::system_account_name;
   }
};

struct canceldelay {
   permission_level      canceling_auth;
   transaction_id_type   trx_id;

   static action_name get_name() {
      return N(canceldelay);
   }

   static account_name get_account() {
      return config::system_account_name;
   }
};

struct onerror {
   uint128_t      sender_id;
   bytes          sent_trx;

   onerror( uint128_t sid, const char* data, size_t len )
   :sender_id(sid),sent_trx(data,data+len){}

   static action_name get_name() {
      return N(onerror);
   }

   static account_name get_account() {
      return config::system_account_name;
   }
};

struct deleteauth {
   deleteauth() = default;
   deleteauth(const account_name& account, const permission_name& permission)
   :account(account), permission(permission)
   {}

   account_name                      account;
   permission_name                   permission;

   static action_name get_name() {
      return N(deleteauth);
   }

   static account_name get_account() {
      return config::system_account_name;
   }
};


} } /// namespace graphene::chain

FC_REFLECT( graphene::chain::newaccount                       , (creator)(name)(owner)(active) )
FC_REFLECT( graphene::chain::setcode                          , (account)(vmtype)(vmversion)(code) )
FC_REFLECT( graphene::chain::setabi                           , (account)(abi) )
FC_REFLECT( graphene::chain::updateauth                       , (account)(permission)(parent)(auth) )
FC_REFLECT( graphene::chain::deleteauth                       , (account)(permission) )
FC_REFLECT( graphene::chain::linkauth                         , (account)(code)(type)(requirement) )
FC_REFLECT( graphene::chain::unlinkauth                       , (account)(code)(type) )
FC_REFLECT( graphene::chain::canceldelay                      , (canceling_auth)(trx_id) )
FC_REFLECT( graphene::chain::onerror                          , (sender_id)(sent_trx) )
