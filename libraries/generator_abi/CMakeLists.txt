# Defines generator_abi library
project (generator_abi C)

# generate compile command database for external tools
set (CMAKE_EXPORT_COMPILE_COMMANDS "ON")

message ( STATUS "Configuring GeneratorABI" )
set(C_DEFINES, "-D__wasm__ -DQUAD_PRECISION")
set( CMAKE_C_FLAGS " -Wall ${CMAKE_C_FLAGS} ${C_DEFINES}" )
set ( generator_abi_sources
   fixsfti.c
   fixdfti.c
   fixunssfti.c
   fixunsdfti.c
)

file ( GLOB generator_abi_headers "${CMAKE_CURRENT_SOURCE_DIR}*.h" )
list( APPEND generator_abi_sources ${generator_abi_headers} )
add_library ( generator_abi STATIC ${generator_abi_sources} )
target_include_directories( generator_abi PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
install ( TARGETS
   generator_abi 

   RUNTIME DESTINATION bin
   LIBRARY DESTINATION lib
   ARCHIVE DESTINATION lib
)
