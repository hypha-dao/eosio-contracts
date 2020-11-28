#include <proposals/badge_proposal.hpp>
#include <document_graph/content_group.hpp>
#include <hyphadao.hpp>

namespace hypha
{
    ContentGroups BadgeProposal::propose_impl(const name &proposer, ContentGroups &contentGroups)
    {
        // check coefficients
        // TODO: move coeffecient thresholds to be configuration values
        checkCoefficient(ContentGroupWrapper::getValue(contentGroups, common::DETAILS, common::HUSD_COEFFICIENT));
        checkCoefficient(ContentGroupWrapper::getValue(contentGroups, common::DETAILS, common::HYPHA_COEFFICIENT));
        checkCoefficient(ContentGroupWrapper::getValue(contentGroups, common::DETAILS, common::HVOICE_COEFFICIENT));
        checkCoefficient(ContentGroupWrapper::getValue(contentGroups, common::DETAILS, common::SEEDS_COEFFICIENT));

        return content_groups;
    }

    Document BadgeProposal::pass_impl(Document proposal)
    {
        m_dao._document_graph.create_edge(hyphadao::get_root(m_dao._document_graph.contract), proposal.hash, common::BADGE_NAME);
        return proposal;
    }

    void BadgeProposal::checkCoefficient(Content& coefficient)
    {
        eosio::check(std::holds_alternative<int64_t>(coefficient.value), "fatal error: coefficient must be an int64_t type: " + coefficient.label);
        eosio::check(std::get<int64_t>(coefficient.value) >= 7000 &&
                     std::get<int64_t>(coefficient.value) <= 13000,
                  "fatal error: coefficient_x10000 must be between 7000 and 13000, inclusive: " + coefficient.label);
    }

    string BadgeProposal::GetBallotContent (ContentGroup proposal_details)
    {
        return std::get<string>(m_dao._document_graph.get_content(proposal_details, common::ICON, true));
    }
    
    name BadgeProposal::GetProposalType () 
    {
        return common::BADGE_NAME;
    }

} // namespace hypha