#pragma once
#include <graphene/chain/protocol/operations.hpp>
#include <graphene/chain/evaluator.hpp>
#include <graphene/chain/database.hpp>

#include <graphene/chain/apply_context.hpp>
#include <graphene/chain/wasm_interface.hpp>
#include <graphene/chain/wast_to_wasm.hpp>

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
                    static const char wast_code[] = R"=====(
                    (module
                        (import "env" "_fwrite" (func $__fwrite(param i32 i32 i32 i32) (result i32)))
                        (import "env" "_stdout" (global $stdoutPtr i32))
                        (import "env" "memory" (memory 1))
                        (export "main" (func $main))

                        (data (i32.const 8) "Hello World!\n")

                        (func (export "establishStackSpace") (param i32 i32) (nop))

                        (func $main (result i32)
                          (local $stdout i32)
                          (set_local $stdout (i32.load align=4 (get_global $stdoutPtr)))

                          (return (call $__fwrite
                             (i32.const 8)          ;; void*ptr      =>Address of our string
                             (i32.const 1)          ;; size_t size   =>Data size
                             (i32.const 13)         ;; size_t nmemb  =>Length of our string 
                             (get_local $stdout)    ;; stream

                          )
                        )
                    )
                    )=====";
                    //auto wasm = graphene::chain::wast_to_wasm(wast_code);
                    std::vector<uint8_t> wasm = graphene::chain::wast_to_wasm(wast_code);
                    auto code_id = fc::sha256::hash(wast_code, (uint32_t) strlen(wast_code));

                    auto wasm_bytes = bytes(wasm.begin(),wasm.end());
                    shared_string wasm_code;
                    wasm_code.resize(wasm_bytes.size());
                    memcpy(wasm_code.data(), wasm_bytes.data(),wasm_bytes.size());
                    dlog("wast code ${c}",("c",wast_code));
                    try{
                        action a{1,1,{}};
                        apply_context ap{a};
                        wasm_interface(graphene::chain::wasm_interface::vm_type::binaryen).apply(code_id,wasm_code,ap);
                        dlog("wasm exec success");
                    }
                    catch(const wasm_exit&){
                        dlog("wasm exec failed!!");
                    }
                    return void_result();
                }FC_CAPTURE_AND_RETHROW((op)) }

    }

} }
