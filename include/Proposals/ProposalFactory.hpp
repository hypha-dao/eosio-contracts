#pragma once 

#include "Proposal.hpp"
#include <document_graph.hpp>
#include <eosio/name.hpp>

namespace hyphaspace {

 class ProposalFactory
    {
    public:
        static hyphaspace::Proposal* Factory(const document_graph &graph, const name &proposal_type);
    };
}


