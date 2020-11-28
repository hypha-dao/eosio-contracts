#include <proposals/badge_proposal.hpp>
#include <document_graph/content_group.hpp>
#include <hyphadao.hpp>

namespace hypha
{
    ContentGroups BadgeProposal::propose_impl(const name &proposer, ContentGroups &contentGroups)
    {
        // check coefficients
        // TODO: move coeffecient thresholds to be configuration values
        checkCoefficient(ContentWrapper::getContent(contentGroups, common::DETAILS, common::HUSD_COEFFICIENT));
        checkCoefficient(ContentWrapper::getContent(contentGroups, common::DETAILS, common::HYPHA_COEFFICIENT));
        checkCoefficient(ContentWrapper::getContent(contentGroups, common::DETAILS, common::HVOICE_COEFFICIENT));
        checkCoefficient(ContentWrapper::getContent(contentGroups, common::DETAILS, common::SEEDS_COEFFICIENT));

        return contentGroups;
    }

    Document BadgeProposal::pass_impl(Document proposal)
    {
        ContentGroups cgs = Document::rollup(Content(common::ROOT_NODE, m_contract));
        eosio::checksum256 rootNode = Document::hashContents(cgs);

        Edge rootBadgeEdge(m_contract, m_contract, rootNode, proposal.getHash(), common::BADGE_NAME);
        rootBadgeEdge.emplace();

        return proposal;
    }

    void BadgeProposal::checkCoefficient(Content coefficient)
    {
        eosio::check(std::holds_alternative<int64_t>(coefficient.value), "fatal error: coefficient must be an int64_t type: " + coefficient.label);
        eosio::check(std::get<int64_t>(coefficient.value) >= 7000 &&
                     std::get<int64_t>(coefficient.value) <= 13000,
                  "fatal error: coefficient_x10000 must be between 7000 and 13000, inclusive: " + coefficient.label);
    }

    string BadgeProposal::GetBallotContent (ContentGroups contentGroups)
    {
        return ContentWrapper::getString (contentGroups, common::DETAILS, common::ICON);
    }
    
    name BadgeProposal::GetProposalType () 
    {
        return common::BADGE_NAME;
    }

} // namespace hypha