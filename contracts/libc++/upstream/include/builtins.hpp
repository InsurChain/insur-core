#pragma once

#include <cstdio>
#include <type_traits>

/**
 * EOS-specific header.
 *
 * Implimentation for compiler builtin functions. Theoretically Clang should
 * generate inline code for calls to these functions. And the generated code should
 * not contain any synchronization because WebAssembly doesn't support multithreading.
 * Instead of that Clang complains that it was unable to find atomic instructions.
 * So, this file contains a workaround.
 * The builtins can be used with any types. But this file contains only functions needed
 * for libc++. To use the builtins with other types add implimentations to this file.
 * (I've tried to write a template which should work for any types, but it didn't work out) 
 */

inline int __sync_lock_test_and_set( int* ptr, int val ) {
   auto old = *ptr;
   *ptr = val;
   return old;
}

inline int __sync_fetch_and_add( int* ptr, int val ) {
   auto old = *ptr;
   *ptr += val;
   return old;
}

inline int __sync_add_and_fetch( int* ptr, int val ) {
   return *ptr += val;
}

inline int32_t __sync_lock_test_and_set( int32_t* ptr, int32_t val ) {
   auto old = *ptr;
   *ptr = val;
   return old;
}

inline int32_t __sync_fetch_and_add( int32_t* ptr, int32_t val ) {
   auto old = *ptr;
   *ptr += val;
   return old;
}

inline int32_t __sync_add_and_fetch( int32_t* ptr, int32_t val ) {
   return *ptr += val;
}

inline std::unexpected_handler __sync_lock_test_and_set( std::unexpected_handler* ptr, std::unexpected_handler val ) {
   auto old = *ptr;
   *ptr = val;
   return old;
}

inline std::unexpected_handler __sync_fetch_and_add( std::unexpected_handler* ptr, std::unexpected_handler val ) {
   auto old = *ptr;
   *reinterpret_cast<char*>( *ptr ) = size_t(val);
   return old;
}
