#pragma once 

#include <eosio/name.hpp>

#include "proposal.hpp"

namespace hyphaspace {

    class AssignmentProposal : public Proposal
    {
    
    public:
        using Proposal::Proposal;

        document_graph::document propose(const name &proposer, std::vector<document_graph::content_group> &content_groups);
        void close(document_graph::document proposal);

    protected:

        std::vector<document_graph::content_group> propose_impl(const name &proposer, std::vector<document_graph::content_group> &content_groups) override;
        document_graph::document pass_impl(document_graph::document proposal) override;
        string GetBallotContent (document_graph::content_group proposal_details) override;
        name GetProposalType () override;

    };
}