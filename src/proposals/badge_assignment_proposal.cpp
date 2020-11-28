#include <proposals/badge_assignment_proposal.hpp>
#include <document_graph/content_group.hpp>
// #include <hyphadao.hpp>
#include <common.hpp>

namespace hypha
{

    ContentGroups BadgeAssignmentProposal::propose_impl(const name &proposer, ContentGroups &contentGroups)
    {
        ContentWrapper badgeAssignment (contentGroups);

        // assignee must exist and be a DHO member
        name assignee = badgeAssignment.getName(common::DETAILS, common::ASSIGNEE);
        verify_membership(assignee);

        // TODO: Additional input cleansing
        // start_period and end_period must be valid, no more than X periods in between

        // badge assignment proposal must link to a valid badge
        Document badgeDocument (m_contract, badgeAssignment.getChecksum(common::DETAILS, common::BADGE_STRING));
        ContentWrapper badge (badgeDocument.content_groups);

        // badge in the proposal must be of type: badge
        eosio::check (badge.getName(common::SYSTEM, common::TYPE) == common::BADGE_NAME, "badge document hash provided in assignment proposal is not of type badge");
 
        return contentGroups;
    }

    Document BadgeAssignmentProposal::pass_impl(Document proposal)
    {
        // need to create edges here
        // TODO: create edges
        return proposal;
    }

    string BadgeAssignmentProposal::GetBallotContent(ContentGroups contentGroups)
    {
        return ContentWrapper::getString (contentGroups, common::DETAILS, common::TITLE);
    }

    name BadgeAssignmentProposal::GetProposalType()
    {
        return common::ASSIGN_BADGE;
    }

} // namespace hypha