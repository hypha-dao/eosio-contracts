cmake_minimum_required(VERSION 3.13)
project(eosiotoken)

set(EOSIO_WASM_OLD_BEHAVIOR "Off")
find_package(eosio.cdt)

add_contract( eosiotoken eosiotoken eosiotoken.cpp )
target_include_directories( eosiotoken PUBLIC ${CMAKE_SOURCE_DIR}/../include )
