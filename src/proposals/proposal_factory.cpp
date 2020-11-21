#include <proposals/proposal_factory.hpp>
#include <proposals/proposal.hpp>
#include <proposals/badge_proposal.hpp>
#include <proposals/role_proposal.hpp>
#include <common.hpp>

namespace hyphaspace
{
    hyphaspace::Proposal* ProposalFactory::Factory(const hyphaspace::hyphadao &dao, const name &proposal_type)
    {
        switch (proposal_type.value)
        {
        case common::BADGE_NAME.value:
            return new BadgeProposal(dao);
        case common::ASSIGN_BADGE.value:

        case common::ROLE_NAME.value:
            return new RoleProposal(dao);
        case common::ASSIGNMENT.value:
        }
    }
} // namespace hyphaspace