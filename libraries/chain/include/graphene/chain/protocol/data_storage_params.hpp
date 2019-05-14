#pragma once
#include <vector>

namespace graphene { namespace chain {

    struct data_storage_params.hpp{
        asset       fee;
        fs::string  data_md5;
        uint32_t    data_length;
    };

} }

FC_REFLECT( graphene::chain::data_storage_params,(fee)(data_md5)(data_length) )
