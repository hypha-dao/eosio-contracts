#pragma once

#include <eosio/name.hpp>
#include "proposal.hpp"

namespace hypha {

    class BadgeProposal : public Proposal
    {

    public:
        //BadgeProposal (hypha::hyphadao &dao);
        using Proposal::Proposal;

        Document propose(const name &proposer, ContentGroups &content_groups);
        void close(Document proposal);

    protected:
        ContentGroups propose_impl(const name &proposer, ContentGroups &content_groups);
        Document pass_impl(Document proposal);
        string GetBallotContent (ContentGroup proposal_details);
        name GetProposalType ();

    private:
        void checkCoefficient(Content &coefficient);

    };
}