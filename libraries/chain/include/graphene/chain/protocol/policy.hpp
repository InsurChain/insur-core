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
#include <graphene/chain/protocol/memo.hpp>

namespace graphene { namespace chain {

struct policy_operation : public base_operation
{
   struct fee_parameters_type {uint64_t fee = 0;};

   asset fee;   // always zero
   account_id_type from;//记录上传保单账户ID
   string file_name ;
   uint32_t file_size = 0;
   string file_format;
   //account_id_type account_id;//记录上传保单账户ID

   //string file_path;
   //string timepoint;
   //optional<memo_data> memo;//memo data hash code
   string policy_hash_code;//记录保单hash值
   extensions_type   extensions;
   account_id_type fee_payer()const { return from; }
   void validate()const{
//		FC_ASSERT(policy_hash_code != "");
   }
   share_type calculate_fee(const fee_parameters_type& k)const { return k.fee; }
};

//hanyang add oracle 
struct oracle_operation : public base_operation
{
   struct fee_parameters_type {uint64_t fee = 0;};

   asset fee;   // always zero
   account_id_type from;//记录上传保单账户ID
   string message ;
   extensions_type   extensions;
   account_id_type fee_payer()const { return from; }
   void validate()const{
   }
   share_type calculate_fee(const fee_parameters_type& k)const { return k.fee; }
};



} }

FC_REFLECT( graphene::chain::policy_operation::fee_parameters_type,(fee)  )

//hanyang add oracle
FC_REFLECT( graphene::chain::oracle_operation::fee_parameters_type,(fee)  )

FC_REFLECT( graphene::chain::policy_operation,(fee)(from)(file_name)(file_size)(file_format)(policy_hash_code)(extensions) )
//hanyang add oracle
FC_REFLECT( graphene::chain::oracle_operation,(fee)(from)(message)(extensions) )
