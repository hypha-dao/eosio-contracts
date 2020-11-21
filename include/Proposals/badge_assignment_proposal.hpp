#pragma once

#include <eosio/name.hpp>
#include <document_graph.hpp>
#include <hyphadao.hpp>
#include "proposal.hpp"

namespace hyphaspace {

    class BadgeAssignmentProposal : public hyphaspace::Proposal
    {

    public:
        BadgeAssignmentProposal (hyphaspace::hyphadao dao);

        document_graph::document propose(const name &proposer, std::vector<document_graph::content_group> &content_groups);
        void close(document_graph::document proposal);

    protected:
        std::vector<document_graph::content_group> propose_impl(const name &proposer, std::vector<document_graph::content_group> &content_groups);
        document_graph::document close_impl(document_graph::document proposal);

        string GetBallotContent (document_graph::content_group proposal_details);
        name GetProposalType ();

    };
}