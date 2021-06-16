cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD 17)

project(bitcoin VERSION 0.1)

add_definitions(-DDBG_PRINT=Verbose)

set(CURL_LIBRARY "-lcurl") 
find_package(CURL REQUIRED)

include_directories(${CURL_INCLUDE_DIR})

set(CORE_FILES Block.cpp
    BlockHeader.cpp
    load.cpp
    Outpoint.cpp
    ProtocolElement.cpp
    hashlib/sha256.cpp
    hashlib/ripemd160.cpp
    Transaction.cpp
    TransactionInput.cpp
    TransactionOutput.cpp
    Witness.cpp
    vm/BitcoinVM.cpp
        vm/opcodes.h vm/variable.cpp vm/variable.h)
add_executable(testinit ${CORE_FILES}
    tests/test_download.cpp
)

target_link_libraries(testinit ${CURL_LIBRARIES})

target_include_directories(testinit PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

add_executable(bitcointest ${CORE_FILES}
        tests/test.cpp
        tests/testtransactioninput.cpp
        tests/testwitness.cpp
        tests/testblock.cpp
        tests/testblockheader.cpp
        tests/testload.cpp
        tests/testoutpoint.cpp
        tests/testtransaction.cpp
        tests/testtransactionoutput.cpp tests/vm/testscript.cpp tests/vm/testvariable.cpp)

target_include_directories(bitcointest PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(bitcointest ${CURL_LIBRARIES})