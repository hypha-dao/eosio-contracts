#include <Proposals/RoleProposal.hpp>
#include <document_graph.hpp>
#include <hyphadao.hpp>
namespace hyphaspace
{

    RoleProposal::RoleProposal (document_graph graph) : Proposal { graph } { }

    std::vector<document_graph::content_group> RoleProposal::propose_impl(const name &proposer, std::vector<document_graph::content_group> &content_groups)
    {
        return content_groups;
    }

    document_graph::document RoleProposal::close_impl(document_graph::document proposal)
    {
        m_graph.create_edge(hyphadao::get_root(m_graph.contract), proposal.hash, common::ROLE_NAME);
        return proposal;
    }

    string GetBallotContent (document_graph::content_group proposal_details)
    {
        return std::get<string>(document_graph::get_content(proposal_details, common::TITLE, true));
    }
    
    name GetProposalType () 
    {
        return common::ROLE_NAME;
    }

} // namespace hyphaspace