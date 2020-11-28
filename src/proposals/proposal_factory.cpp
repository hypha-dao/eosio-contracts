#include <proposals/proposal_factory.hpp>
#include <proposals/proposal.hpp>
#include <proposals/badge_proposal.hpp>
#include <proposals/badge_assignment_proposal.hpp>
#include <proposals/role_proposal.hpp>
#include <proposals/assignment_proposal.hpp>
#include <common.hpp>

namespace hypha
{
    //We don't have to specify the namespace hypha:: since we are already inside that namespace

    Proposal* ProposalFactory::Factory(hyphadao &dao, const name &proposal_type)
    {
        switch (proposal_type.value)
        {
        case common::BADGE_NAME.value:
            return new BadgeProposal(dao);
        
        case common::ASSIGN_BADGE.value:
            return new BadgeAssignmentProposal(dao);

        case common::ROLE_NAME.value:
            return new RoleProposal(dao);
        
        case common::ASSIGNMENT.value:
            return new AssignmentProposal(dao);
        }

        check(false, "Unknown proposal_type");
        return nullptr;
    }
} // namespace hypha