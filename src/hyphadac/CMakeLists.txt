cmake_minimum_required(VERSION 3.13)
project(hyphadac)

set(EOSIO_WASM_OLD_BEHAVIOR "Off")
find_package(eosio.cdt)

add_contract( hyphadac hyphadac hyphadac.cpp )
target_include_directories( hyphadac PUBLIC ${CMAKE_SOURCE_DIR}/../include )