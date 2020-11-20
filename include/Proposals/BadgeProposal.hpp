#pragma once

#include <document_graph.hpp>
#include "Proposal.hpp"

namespace hyphaspace {

    class BadgeProposal : public hyphaspace::Proposal
    {

    public:
        BadgeProposal (document_graph graph);

        document_graph::document propose(const name &proposer, std::vector<document_graph::content_group> &content_groups);
        void close(document_graph::document proposal);

    protected:
        std::vector<document_graph::content_group> propose_impl(const name &proposer, std::vector<document_graph::content_group> &content_groups);
        document_graph::document close_impl(document_graph::document proposal);
        string GetBallotContent (document_graph::content_group proposal_details);
        name GetProposalType ();

    private:
        void check_coefficient(document_graph::content_group &content_group, const string &coefficient_key);

    };
}