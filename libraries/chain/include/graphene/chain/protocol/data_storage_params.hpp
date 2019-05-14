#pragma once
#include <vector>

namespace graphene { namespace chain {

    struct data_storage_params.hpp{
        asset                fee;
        fs::string           data_md5;
        fc::time_point_sec   expiration;
    };

} }

FC_REFLECT( graphene::chain::data_storage_params,(fee)(data_md5)(expiration) )
