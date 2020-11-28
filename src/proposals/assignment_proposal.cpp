#include <proposals/assignment_proposal.hpp>
#include <document_graph.hpp>
#include <hyphadao.hpp>

namespace hypha
{

    ContentGroups AssignmentProposal::propose_impl(const name &proposer, ContentGroups &content_groups)
    {
        // grab the proposal details - enforce required (strict) inputs
        ContentGroup details = m_dao._document_graph.get_content_group(content_groups, common::DETAILS, true);

        // assignee must exist and be a DHO member
        name assignee = std::get<name>(m_dao._document_graph.get_content(details, common::ASSIGNEE, true));
        verify_membership(assignee);

        // TODO: Additional input cleansing
        // start_period and end_period must be valid, no more than X periods in between

        // assignment proposal must link to a valid role
        Document role = m_dao._document_graph.get_document(std::get<checksum256>(m_dao._document_graph.get_content(details, common::ROLE_STRING, true)));

        // role in the proposal must be of type: role
        if (std::get<name>(document_graph::get_content(role, common::SYSTEM, common::TYPE, true)) != common::ROLE_NAME)
        {
            string role_title = std::get<string>(document_graph::get_content(role, common::DETAILS, common::TITLE, true));
            check(false, "role document hash provided in assignment proposal is not of type: role");
        }

        // time_share_x100 is required and must be greater than zero and less than 100%
        int64_t time_share = std::get<int64_t>(m_dao._document_graph.get_content(details, common::TIME_SHARE, true));
        check(time_share > 0, common::TIME_SHARE + " must be greater than zero. You submitted: " + std::to_string(time_share));
        check(time_share <= 10000, common::TIME_SHARE + " must be less than or equal to 10000 (=100%). You submitted: " + std::to_string(time_share));

        // retrieve the minimum time_share from the role
        auto min_time_share = m_dao._document_graph.get_content(role, common::DETAILS, common::MIN_TIME_SHARE, false);
        if (min_time_share != document_graph::not_found()) {         
            check (time_share >= std::get<int64_t>(min_time_share), common::TIME_SHARE + " must be greater than or equal to the role configuration. Role value for " 
                + common::MIN_TIME_SHARE + " is " + std::to_string(std::get<int64_t>(min_time_share)) + ", and you submitted: " + std::to_string(time_share));        
        }

        // deferred_x100 is required and must be greater than or equal to zero and less than or equal to 10000 
        int64_t deferred = std::get<int64_t>(m_dao._document_graph.get_content(details, common::DEFERRED, true));
        check(deferred >= 0, common::DEFERRED + " must be greater than or equal to zero. You submitted: " + std::to_string(deferred));
        check(deferred <= 10000, common::DEFERRED + " must be less than or equal to 10000 (=100%). You submitted: " + std::to_string(deferred));

        // retrieve the minimum deferred from the role
        auto min_deferred = m_dao._document_graph.get_content(role, common::DETAILS, common::MIN_DEFERRED, false);
        if (min_deferred != document_graph::not_found()) {         
            check (deferred >= std::get<int64_t>(min_deferred), common::DEFERRED + " must be greater than or equal to the role configuration. Role value for " 
                + common::MIN_DEFERRED + " is " + std::to_string(std::get<int64_t>(min_deferred)) + ", and you submitted: " + std::to_string(deferred));        
        }

        // start_period and end_period are required and must be greater than or equal to zero, and end_period >= start_period
        int64_t start_period = std::get<int64_t>(m_dao._document_graph.get_content(details, common::START_PERIOD, true));
        check(start_period >= 0, common::START_PERIOD + " must be greater than or equal to zero. You submitted: " + std::to_string(start_period));
        int64_t end_period = std::get<int64_t>(m_dao._document_graph.get_content(details, common::END_PERIOD, true));
        check(end_period >= 0, common::END_PERIOD + " must be greater than or equal to zero. You submitted: " + std::to_string(end_period));
        check(end_period >= start_period, common::END_PERIOD + " must be greater than or equal to " + common::START_PERIOD + 
            ". You submitted: " + common::START_PERIOD + ": " + std::to_string(start_period) + 
            " and " + common::END_PERIOD + ": " + std::to_string(end_period));

        asset annual_usd_salary = std::get<asset>(m_dao._document_graph.get_content(role, common::DETAILS, common::ANNUAL_USD_SALARY, true));

        // ContentGroup details = m_dao._document_graph.get_content_group(content_groups, common::DETAILS, true);

        // add the USD period pay amount (this is used to calculate SEEDS at time of salary claim)
        details.push_back(m_dao._document_graph.new_content(common::USD_SALARY_PER_PERIOD, adjustAsset(annual_usd_salary, common::PHASE_TO_YEAR_RATIO)));

        // add remaining derived per period salary amounts to this document
        details.push_back(m_dao._document_graph.new_content(common::HUSD_SALARY_PER_PERIOD, calculateHusd(annual_usd_salary, time_share, deferred)));
        details.push_back(m_dao._document_graph.new_content(common::HYPHA_SALARY_PER_PERIOD, calculateHypha(annual_usd_salary, time_share, deferred)));
        details.push_back(m_dao._document_graph.new_content(common::HVOICE_SALARY_PER_PERIOD, calculateHvoice(annual_usd_salary, time_share)));

        return content_groups;
    }

    Document AssignmentProposal::pass_impl(Document proposal)
    {
        ContentGroup details = m_dao._document_graph.get_content_group(proposal, common::DETAILS, true);

        Document role = m_dao._document_graph.get_document(std::get<checksum256>(m_dao._document_graph.get_content(
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

    string AssignmentProposal::GetBallotContent(ContentGroup proposal_details)
    {
        return std::get<string>(m_dao._document_graph.get_content(proposal_details, common::TITLE, true));
    }

    name AssignmentProposal::GetProposalType()
    {
        return common::ASSIGNMENT;
    }

    asset AssignmentProposal::calculateTimeShareUsdPerPeriod(const asset &annualUsd, const int64_t &timeShare)
    {
        asset commitment_adjusted_usd_annual = adjustAsset(annualUsd, (float)(float)timeShare / (float)100);
        return adjustAsset(commitment_adjusted_usd_annual, common::PHASE_TO_YEAR_RATIO);
    }

    asset AssignmentProposal::calculateHusd(const asset &annualUsd, const int64_t &timeShare, const int64_t &deferred)
    {
        // calculate HUSD salary amount
        // 1. normalize annual salary to the time commitment of this proposal
        // 2. multiply (1) by 0.02026 to calculate a single moon phase; avg. phase is 7.4 days, 49.36 phases per year
        // 3. multiply (2) by 1 - deferral perc        
        asset nonDeferredTimeShareAdjUsdPerPeriod = adjustAsset(calculateTimeShareUsdPerPeriod(annualUsd, timeShare), (float)1 - ((float)deferred / (float)100));

        // convert symbol from USD to HUSD
        return asset{nonDeferredTimeShareAdjUsdPerPeriod.amount, common::S_HUSD};
    }

    asset AssignmentProposal::calculateHypha(const asset &annualUsd, const int64_t &timeShare, const int64_t &deferred)
    {
        // calculate HYPHA phase salary amount
        asset deferredTimeShareAdjUsdPerPeriod = adjustAsset(calculateTimeShareUsdPerPeriod(annualUsd, timeShare), (float)(float)deferred / (float)100);

        hyphadao::config_table config_s(m_dao._document_graph.contract, m_dao._document_graph.contract.value);
        hyphadao::Config c = config_s.get_or_create(m_dao._document_graph.contract, hyphadao::Config());

        float hypha_deferral_coeff = (float) c.ints[common::HYPHA_DEFERRAL_FACTOR] / (float) 100;
        return adjustAsset(asset{deferredTimeShareAdjUsdPerPeriod.amount, common::S_HYPHA}, hypha_deferral_coeff);
    }

    asset AssignmentProposal::calculateHvoice(const asset &annualUsd, const int64_t &timeShare)
    {
        return asset{calculateTimeShareUsdPerPeriod(annualUsd, timeShare).amount * 2, common::S_HVOICE};
    }
} // namespace hypha