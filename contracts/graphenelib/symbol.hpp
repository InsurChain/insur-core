#pragma once
#include <graphenelib/serialize.hpp>
#include <graphenelib/print.hpp>
#include <graphenelib/system.h>
#include <tuple>
#include <limits>

namespace graphene {

   static constexpr uint64_t string_to_symbol( uint8_t precision, const char* str ) {
      uint32_t len = 0;
      while( str[len] ) ++len;

      uint64_t result = 0;
      for( uint32_t i = 0; i < len; ++i ) {
         if( str[i] < 'A' || str[i] > 'Z' ) {
            /// ERRORS?
         } else {
            result |= (uint64_t(str[i]) << (8*(1+i)));
         }
      }

      result |= uint64_t(precision);
      return result;
   }

   #define S(P,X) ::graphene:string_to_symbol(P,#X)

   typedef uint64_t symbol_name;

   static constexpr bool is_valid_symbol( symbol_name sym ) {
      sym >>= 8;
      for( int i = 0; i < 7; ++i ) {
         char c = (char)(sym & 0xff);
         if( !('A' <= c && c <= 'Z')  ) return false;
         sym >>= 8;
         if( !(sym & 0xff) ) {
            do {
              sym >>= 8;
              if( (sym & 0xff) ) return false;
              ++i;
            } while( i < 7 );
         }
      }
      return true;
   }

   static constexpr uint32_t symbol_name_length( symbol_name tmp ) {
      tmp >>= 8; /// skip precision
      uint32_t length = 0;
      while( tmp & 0xff && length <= 7) {
         ++length;
         tmp >>= 8;
      }

      return length;
   }

   struct contract_symbol_type {
      symbol_name value;

      contract_symbol_type() { }
      contract_symbol_type(symbol_name s): value(s) { }
      bool     is_valid()const  { return is_valid_symbol( value ); }
      uint64_t precision()const { return value & 0xff; }
      uint64_t name()const      { return value >> 8;   }
      uint32_t name_length()const { return symbol_name_length( value ); }

      operator symbol_name()const { return value; }

      void print(bool show_precision=true)const {
         if( show_precision ){
            ::graphene::print(precision());
            prints(",");
         }

         auto sym = value;
         sym >>= 8;
         for( int i = 0; i < 7; ++i ) {
            char c = (char)(sym & 0xff);
            if( !c ) return;
            prints_l(&c, 1 );
            sym >>= 8;
         }
      }

      GRAPHENE_SERIALIZE(contract_symbol_type, (value))
   };

} /// namespace graphene
