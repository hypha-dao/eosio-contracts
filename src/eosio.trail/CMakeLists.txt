cmake_minimum_required(VERSION 3.13)
project(eosio.trail)

set(EOSIO_WASM_OLD_BEHAVIOR "Off")
find_package(eosio.cdt)

add_contract( eosio.trail eosio.trail eosio.trail.cpp )
target_include_directories( eosio.trail PUBLIC ${CMAKE_SOURCE_DIR}/../include )
