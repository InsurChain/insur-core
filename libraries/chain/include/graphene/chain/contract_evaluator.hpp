#pragma once
#include <graphene/chain/protocol/operations.hpp>
#include <graphene/chain/evaluator.hpp>
#include <graphene/chain/database.hpp>

namespace graphene { namespace chain {

    class contract_call_evaluator: public evaluator<contract_call_evaluator>
    {
        public:
            typedef contract_call_opertion operation_type;
            void_result do_evaluate(const contract_call_operation & op)
            {
                try{
                    dlog("contract_call_evaluator do_evaluate");
                    return void_result();
                }FC_CAPTURE_AND_RETHROW((op)) }
            void_result do_apply(const contract_call_operation &op )
            {
                try{
                    dlog("contract_call_evaluator do_apply");
                    return void_result();
                }FC_CAPTURE_AND_RETHROW((op)) }

    }

} }
