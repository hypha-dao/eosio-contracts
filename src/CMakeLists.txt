project(hyphadao_project)
cmake_minimum_required(VERSION 3.17)

set(EOSIO_WASM_OLD_BEHAVIOR "Off")
find_package(eosio.cdt)

add_contract( hyphadao hyphadao 
                hyphadao.cpp 
                legacy_proposal.cpp 
                config.cpp 
                membership.cpp 
                bank.cpp
                object.cpp 
                utility.cpp 
                calculation.cpp 
                member.cpp
                proposals/proposal.cpp
                proposals/role_proposal.cpp
                proposals/badge_proposal.cpp
                proposals/badge_assignment_proposal.cpp
                proposals/assignment_proposal.cpp
                proposals/proposal_factory.cpp
                ../document-graph/src/document_graph/document_graph.cpp 
                ../document-graph/src/document_graph/document.cpp 
                ../document-graph/src/document_graph/content_group.cpp
                ../document-graph/src/document_graph/content.cpp
                ../document-graph/src/document_graph/edge.cpp
                ../document-graph/src/document_graph/util.cpp )

target_include_directories( hyphadao PUBLIC ${CMAKE_SOURCE_DIR}/../include ${CMAKE_SOURCE_DIR}/../../document-graph/include )

target_compile_options(hyphadao PUBLIC -R${CMAKE_CURRENT_SOURCE_DIR}/resources)
# target_ricardian_directory( hyphadao ${CMAKE_SOURCE_DIR}/../resources )