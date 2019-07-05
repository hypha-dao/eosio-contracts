cmake_minimum_required(VERSION 3.13)
project(hyphadao)

set(EOSIO_WASM_OLD_BEHAVIOR "Off")
find_package(eosio.cdt)

add_subdirectory(hyphadao)
add_subdirectory(eosiotoken)
