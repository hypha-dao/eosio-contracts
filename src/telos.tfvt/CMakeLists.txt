cmake_minimum_required(VERSION 3.13)
project(hyphadac)

set(EOSIO_WASM_OLD_BEHAVIOR "Off")
find_package(eosio.cdt)

add_contract( telos.tfvt telos.tfvt telos.tfvt.cpp )
target_include_directories( telos.tfvt PUBLIC ${CMAKE_SOURCE_DIR}/../include )