#pragma once 

#include <eosio/name.hpp>
#include <document_graph.hpp>

namespace hyphaspace {

    class hyphadao;
    class Proposal;

    class ProposalFactory
    {
    public:
        static Proposal* Factory(hyphadao &dao, const name &proposal_type);
    };
}


