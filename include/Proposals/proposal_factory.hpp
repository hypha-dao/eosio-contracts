#pragma once 

#include <eosio/name.hpp>
#include <document_graph.hpp>
#include <hyphadao.hpp>
#include "proposal.hpp"

namespace hyphaspace {

 class ProposalFactory
    {
    public:
        static hyphaspace::Proposal* Factory(const hyphaspace::hyphadao &dao, const name &proposal_type);
    };
}


