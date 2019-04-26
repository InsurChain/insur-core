#pragma once

using namespace fc;

namespace graphene { namespace chain { 

   class apply_context;
   class transaction_context;

   template<typename T>
   struct class_from_wasm {
      static auto value(apply_context& ctx) {
         return T(ctx);
      }
   };
   
   template<>
   struct class_from_wasm<transaction_context> {
      template <typename ApplyCtx>
      static auto &value(ApplyCtx& ctx) {
         return ctx.trx_context;
      }
   };

   template<>
   struct class_from_wasm<apply_context> {
      static auto &value(apply_context& ctx) {
         return ctx;
      }
   };

   template<typename T>
   struct array_ptr {
      explicit array_ptr (T * value) : value(value) {}

      typename std::add_lvalue_reference<T>::type operator*() const {
         return *value;
      }

      T *operator->() const noexcept {
         return value;
      }

      template<typename U>
      operator U *() const {
         return static_cast<U *>(value);
      }

      T *value;
   }; 

   struct null_terminated_ptr {
      explicit null_terminated_ptr(char* value) : value(value) {}

      typename std::add_lvalue_reference<char>::type operator*() const {
         return *value;
      }

      char *operator->() const noexcept {
         return value;
      }

      template<typename U>
      operator U *() const {
         return static_cast<U *>(value);
      }

      char *value;
   };

 } } 
