#include <proposals/assignment_proposal.hpp>
#include <document_graph.hpp>
#include <hyphadao.hpp>

namespace hyphaspace
{

    std::vector<document_graph::content_group> AssignmentProposal::propose_impl(const name &proposer, std::vector<document_graph::content_group> &content_groups)
    {
        // grab the proposal details - enforce required (strict) inputs
        document_graph::content_group details = m_dao._document_graph.get_content_group(content_groups, common::DETAILS, true);

        // assignee must exist
        name assignee = std::get<name>(m_dao._document_graph.get_content(details, common::ASSIGNEE, true));

        // assignee must be a DHO member
        verify_membership(assignee);

        // TODO: Additional input cleansing
        // start_period and end_period must be valid, no more than X periods in between

        // assignment proposal must link to a valid role
        document_graph::document role = m_dao._document_graph.get_document(std::get<checksum256>(m_dao._document_graph.get_content(details, common::ROLE_STRING, true)));

        // role in the proposal must be of type: role
        if (std::get<name>(document_graph::get_content(role, common::SYSTEM, common::TYPE, true)) != common::ROLE_NAME)
        {
            string role_title = std::get<string>(document_graph::get_content(role, common::DETAILS, common::TITLE, true));
            check(false, "role document hash provided in assignment proposal is not of type: role");
        }

        return content_groups;
    }

    document_graph::document AssignmentProposal::pass_impl(document_graph::document proposal)
    {
        document_graph::content_group details = m_dao._document_graph.get_content_group(proposal, common::DETAILS, true);

        document_graph::document role = m_dao._document_graph.get_document(std::get<checksum256>(m_dao._document_graph.get_content(
            details, common::ROLE_STRING, true)));

        name assignee = std::get<name>(m_dao._document_graph.get_content(details, common::ASSIGNEE, true));
        checksum256 member_doc_hash = m_dao.get_member_doc(assignee).hash;

        // update graph edges:
        //  member          ---- assigned           ---->   role_assignment
        //  role_assignment ---- assignee           ---->   member
        //  role_assignment ---- role               ---->   role
        //  role            ---- role_assignment    ---->   role_assignment

        // what about periods?

        //  member          ---- assigned           ---->   role_assignment
        m_dao._document_graph.create_edge(member_doc_hash, proposal.hash, common::ASSIGNED);

        //  role_assignment ---- assignee           ---->   member
        m_dao._document_graph.create_edge(proposal.hash, member_doc_hash, common::ASSIGNEE_NAME);

        //  role_assignment ---- role               ----> role
        m_dao._document_graph.create_edge(proposal.hash, role.hash, common::ROLE_NAME);

        //  role            ---- role_assignment    ----> role_assignment
        m_dao._document_graph.create_edge(role.hash, proposal.hash, common::ASSIGNMENT);

        // I don't think we need this one:
        // m_dao._document_graph.create_edge(m_dao.get_root(m_dao._document_graph.contract), proposal.hash, common::ASSIGNMENT);
        return proposal;
    }

    string AssignmentProposal::GetBallotContent(document_graph::content_group proposal_details)
    {
        return std::get<string>(m_dao._document_graph.get_content(proposal_details, common::TITLE, true));
    }

    name AssignmentProposal::GetProposalType()
    {
        return common::ASSIGNMENT;
    }

} // namespace hyphaspace