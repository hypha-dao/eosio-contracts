project(hyphadao)

set(EOSIO_WASM_OLD_BEHAVIOR "Off")
find_package(eosio.cdt)

add_contract( hyphadao hyphadao hyphadao.cpp proposal.cpp config.cpp membership.cpp document.cpp utility.cpp calculation.cpp)
target_include_directories( hyphadao PUBLIC ${CMAKE_SOURCE_DIR}/../include )
target_ricardian_directory( hyphadao ${CMAKE_SOURCE_DIR}/../ricardian )