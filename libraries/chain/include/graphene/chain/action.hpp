#pragma once
#include<graphene/chain/protocol/types.hpp>
namespace graphene{ namespace chain{

    typedef uint64_t account_name;
    typedef uint64_t action_name;

    struct action{
        account_name            account;
        action_name             name;
        bytes                   data;

        action(){}

        template<typename T, std::enable_if_t<std::is_base_of<bytes, T>::value, int> =1>
        action(const T& value)
        {
            account = T::get_account();
            name    = T::get_name();
            data.assign(value.data(), value.data() + value.size());
        }
        
        template<typename T, std::enable_if_t<std::is_base_of<bytes, T>::value, int> =1>
        action(const T& value)
        {
            account = T::get_account();
            name    = T::get_name();
            data    = fc::raw::pack(value);
        }

        action( account_name account , action_name name ,const bytes& data ):account(account),name(name),data(data)
        {
        }
        template<class T>
        T data_as()const
        {
            FC_ASSET( account == T::get_account() );
            FC_ASSET( name == T::get_name() );
            return fc::raw::unpack<T>(data);

        }

    };
}}// namespace graphene::chain
FC_REFLECT( graphene::chain::action, (account)(name)(data) )
