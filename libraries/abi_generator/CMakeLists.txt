# Find an installed build of LLVM
find_package(LLVM 4.0 REQUIRED CONFIG)
set( CMAKE_CXX_STANDARD 11 )

file(GLOB HEADERS "include/graphene/abi_generator/*.hpp")

set(SOURCES ${HEADERS})

add_library( abi_generator
             abi_generator.cpp
             ${HEADERS} )

target_include_directories(abi_generator
                           PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include"
                                  "${CMAKE_CURRENT_SOURCE_DIR}/../chain/include"
                                  "${CMAKE_CURRENT_SOURCE_DIR}/../chain/include/protocol"
                                  "${CMAKE_CURRENT_SOURCE_DIR}/../fc/include"
                                  "${CMAKE_CURRENT_SOURCE_DIR}/../db/include"
                                  "/usr/local/opt/openssl/include"
                                  "${LLVM_INCLUDE_DIRS}")

target_link_libraries(abi_generator
  clangRewrite
  clangTooling
  clangToolingCore
  clangFrontend
  clangDriver
  clangSerialization
  clangParse
  clangSema
  clangAnalysis
  clangAST
  clangBasic
  clangEdit
  clangLex
)

target_link_libraries(abi_generator
  LLVMCodeGen # Analysis, Core, MC, Scalar, Support, Target, TransformUtils
  LLVMipo
  LLVMScalarOpts
  LLVMInstCombine
  LLVMTransformUtils
  LLVMTarget # Analysis, MC, Core, Support
  LLVMAnalysis # Core, Support
  LLVMOption # Support
  LLVMMCDisassembler # MC, Support
  LLVMMCParser # MC, Support
  LLVMMC # Object, Support
  LLVMProfileData # Core, Support, Object
  LLVMObject # BitReader, Core, Support
  LLVMBitReader # Core, Support
  LLVMCore # BinaryFormat, Support
  LLVMSupport # Demangle
  LLVMDemangle
  LLVMMC
  fc
  graphene_db
  graphene_chain
)
