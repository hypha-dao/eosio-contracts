#pragma once 

#include <eosio/name.hpp>
#include "proposal.hpp"

namespace hypha {

    // class hyphadao;
    // class Proposal;

    class ProposalFactory
    {
    public:
        static Proposal* Factory(const eosio::name &contract, const eosio::name &proposal_type);
    };
}


