cmake_minimum_required(VERSION 3.13)
project(hyphadac)

set(EOSIO_WASM_OLD_BEHAVIOR "Off")
find_package(eosio.cdt)

add_subdirectory(hyphadac)
add_subdirectory(eosiotoken)
add_subdirectory(telos.tfvt)
add_subdirectory(eosio.trail)