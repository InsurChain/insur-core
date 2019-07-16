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
#include <graphene/chain/multisig_evaluator.hpp>
#include <graphene/chain/account_object.hpp>
#include <graphene/chain/exceptions.hpp>
#include <graphene/chain/hardfork.hpp>
#include <graphene/chain/is_authorized_asset.hpp>
#include <graphene/chain/multisig_evaluator.hpp>
#include <graphene/chain/multisig_object.hpp>

namespace graphene { namespace chain {
void_result multisig_evaluator::do_evaluate( const multisig_operation& op ){
        return void_result();
}
object_id_type multisig_evaluator::do_apply( const multisig_operation& o )
{ try {
   db().adjust_balance( o.from, -o.amount );
    int size = o.owners.size();
    const multisig_object&mul = db().create<multisig_object>([&](multisig_object&mul){
        mul.from                = o.from;
        //mul.owners              = o.owners;
        //size at least  3 at most 7
        mul.first               = o.owners[0];
        mul.second              = o.owners[1];
        mul.third               = o.owners[2];
        if(4 == size)
        {
            mul.fourth = o.owners[3];
        }
        else if (5 == size)
        {
            mul.fourth = o.owners[3];
            mul.fifth = o.owners[4];
        }
        else if (6 == size)
        {
            mul.fourth = o.owners[3];
            mul.fifth = o.owners[4];
            mul.sixth = o.owners[5];
        }
        else if (7 == size)
        {
            mul.fourth = o.owners[3];
            mul.fifth = o.owners[4];
            mul.sixth = o.owners[5];
            mul.seventh = o.owners[6];
        }
        mul.amount              = o.amount;
    });
//   db().adjust_balance( o.to, o.amount );
   return mul.id;
} FC_CAPTURE_AND_RETHROW( (o) ) }
} } // graphene::chain
