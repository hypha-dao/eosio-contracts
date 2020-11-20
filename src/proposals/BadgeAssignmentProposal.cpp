#include <Proposals/BadgeAssignmentProposal.hpp>
#include <document_graph.hpp>
#include <hyphadao.hpp>

namespace hyphaspace
{
    BadgeAssignmentProposal::BadgeAssignmentProposal (document_graph graph) : Proposal { graph } { }

    std::vector<document_graph::content_group> BadgeAssignmentProposal::propose_impl(const name &proposer, std::vector<document_graph::content_group> &content_groups)
    {
       // grab the proposal details - enforce required (strict) inputs
        document_graph::content_group details = m_graph.get_content_group(content_groups, common::DETAILS, true);

        // badge assignee must exist
        name assignee = std::get<name>(m_graph.get_content(details, common::ASSIGNEE, true));

        // badge assignee must be a DHO member
        hyphadao::verify_membership (assignee);

        // TODO: Additional input cleansing
        // start_period and end_period must be valid, no more than X periods in between

        // badge assignment proposal must link to a valid badge
        document_graph::document badge = m_graph.get_document(std::get<checksum256>(m_graph.get_content(details, common::BADGE_STRING, true)));
        string badge_title = std::get<string>(document_graph::get_content(badge, common::DETAILS, common::TITLE, true));

        // badge in the proposal must be of type: badge
        check(std::get<name>(document_graph::get_content(badge, common::SYSTEM, common::TYPE, true)) == common::BADGE_NAME,
            "badge document hash provided in assignment proposal is not of type badge");

        return content_groups;
    }

    document_graph::document BadgeAssignmentProposal::close_impl(document_graph::document proposal)
    {


        return proposal;
    }

    string GetBallotContent (document_graph::content_group proposal_details)
    {
        return std::get<string>(document_graph::get_content(proposal_details, common::TITLE, true));
    }
    
    name GetProposalType () 
    {
        return common::ASSIGN_BADGE;
    }

} // namespace hyphaspace