project(hyphadao_project)
cmake_minimum_required(VERSION 3.17)

set(EOSIO_WASM_OLD_BEHAVIOR "Off")
find_package(eosio.cdt)

add_contract( hyphadao hyphadao 
                hyphadao.cpp 
                proposal.cpp 
                config.cpp 
                badge.cpp 
                membership.cpp 
                bank.cpp
                object.cpp 
                utility.cpp 
                calculation.cpp 
                decide.cpp
                document_graph/document_graph.cpp 
                document_graph/edges.cpp 
                document_graph/accessors.cpp 
                document_graph/creators.cpp )

target_include_directories( hyphadao PUBLIC ${CMAKE_SOURCE_DIR}/../include )
target_compile_options(hyphadao PUBLIC -R${CMAKE_CURRENT_SOURCE_DIR}/resources)
# target_ricardian_directory( hyphadao ${CMAKE_SOURCE_DIR}/../resources )