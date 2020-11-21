#include <proposals/badge_proposal.hpp>
#include <document_graph.hpp>
#include <hyphadao.hpp>
namespace hyphaspace
{
    BadgeProposal::BadgeProposal (hyphaspace::hyphadao dao) : Proposal { dao } { }

    std::vector<document_graph::content_group> BadgeProposal::propose_impl(const name &proposer, std::vector<document_graph::content_group> &content_groups)
    {
        // grab the proposal details - enforce required (strict) inputs
        document_graph::content_group proposal_details = m_dao._document_graph.get_content_group(content_groups, common::DETAILS, true);

        // check coefficients
        // TODO: move coeffecient thresholds to be configuration values
        check_coefficient(proposal_details, "husd_coefficient_x10000");
        check_coefficient(proposal_details, "hypha_coefficient_x10000");
        check_coefficient(proposal_details, "hvoice_coefficient_x10000");
        check_coefficient(proposal_details, "seeds_coefficient_x10000");

        return content_groups;
    }

    document_graph::document BadgeProposal::close_impl(document_graph::document proposal)
    {
        m_dao._document_graph.create_edge(hyphadao::get_root(m_dao._document_graph.contract), proposal.hash, common::BADGE_NAME);
        return proposal;
    }

    void BadgeProposal::check_coefficient(document_graph::content_group &content_group, const string &coefficient_key)
    {
        document_graph::flexvalue coefficient_x10000 = document_graph::get_content(content_group, coefficient_key, false);
        if (coefficient_x10000 != document_graph::not_found())
        {
            check(std::holds_alternative<int64_t>(coefficient_x10000), "fatal error: coefficient must be an int64_t type: " + coefficient_key);
            check(std::get<int64_t>(coefficient_x10000) >= 7000 &&
                      std::get<int64_t>(coefficient_x10000) <= 13000,
                  "fatal error: coefficient_x10000 must be between 7000 and 13000, inclusive: " + coefficient_key);
        }
    }

    string GetBallotContent (document_graph::content_group proposal_details)
    {
        return std::get<string>(m_dao._document_graph.get_content(proposal_details, common::ICON, true));
    }
    
    name GetProposalType () 
    {
        return common::BADGE_NAME;
    }

} // namespace hyphaspace