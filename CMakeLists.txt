cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD 17)

project(bitcoin VERSION 0.1)

find_package(PkgConfig)
add_definitions(-DDBG_PRINT=Verbose)

set(CURL_LIBRARY "-lcurl")
find_package(CURL REQUIRED)
find_package(OpenSSL REQUIRED)

include_directories(${CURL_INCLUDE_DIR} ${OPENSSL_INCLUDE_DIR})

set(CORE_FILES src/Block.cpp
        src/BlockHeader.cpp
        src/load.cpp
        src/Outpoint.cpp
        src/ProtocolElement.cpp
        src/hashlib/sha256.cpp
        src/hashlib/ripemd160.cpp
        src/Transaction.cpp
        src/TransactionInput.cpp
        src/TransactionOutput.cpp
        src/Witness.cpp
        src/vm/BitcoinVM.cpp
        src/vm/variable.cpp
        src/crypto/util.cpp
        src/db/database.cpp
        )

add_executable(testinit ${CORE_FILES}
        tests/test_download.cpp
        )

target_link_libraries(testinit
        ${CURL_LIBRARIES}
        ${OPENSSL_LIBRARIES}
        sqlite3
        )

target_include_directories(testinit PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${CMAKE_CURRENT_SOURCE_DIR}/include
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
        tests/testtransactionoutput.cpp
        tests/vm/testscript.cpp
        tests/vm/testvariable.cpp
        tests/hashlib/testripemd160.cpp
        tests/crypto/test_open_ssl.cpp
        tests/testdatabase.cpp
        )

target_include_directories(bitcointest PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        )

target_link_libraries(bitcointest ${CURL_LIBRARIES}
        ${OPENSSL_LIBRARIES}
        sqlite3
        )
