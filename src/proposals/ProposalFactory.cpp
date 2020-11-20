#include <Proposals/ProposalFactory.hpp>
#include <Proposals/Proposal.hpp>
#include <Proposals/BadgeProposal.hpp>
#include <Proposals/RoleProposal.hpp>
#include <common.hpp>

namespace hyphaspace
{
    hyphaspace::Proposal* ProposalFactory::Factory(const document_graph &graph, const name &proposal_type)
    {
        switch (proposal_type.value)
        {
        case common::BADGE_NAME.value:
            return new BadgeProposal(graph);
        case common::ASSIGN_BADGE.value:

        case common::ROLE_NAME.value:
            return new RoleProposal(graph);
        case common::ASSIGNMENT.value:
        }
    }
} // namespace hyphaspace