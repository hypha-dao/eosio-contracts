#include <hyphadao.hpp>

using namespace hyphaspace;

void hyphadao::check_coefficient(document_graph::content_group &content_group, const string &coefficient_key)
{
    document_graph::flexvalue coefficient_x10000 = _document_graph.get_content(content_group, coefficient_key, false);
    if (coefficient_x10000 != _document_graph.DOES_NOT_EXIST)
    {
        check(std::holds_alternative<int64_t>(coefficient_x10000), "fatal error: coefficient must be an int64_t type: " + coefficient_key);
        check(std::get<int64_t>(coefficient_x10000) >= 7000 &&
                  std::get<int64_t>(coefficient_x10000) <= 13000,
              "fatal error: coefficient_x10000 must be between 7000 and 13000, inclusive: " + coefficient_key);
    }
}

document_graph::document hyphadao::create_votetally_doc(const name &proposer, std::vector<document_graph::content_group> &content_groups)
{
    std::vector<document_graph::content_group> option_groups = _document_graph.get_content_groups_of_type(content_groups, common::GROUP_TYPE_OPTION, false);

    // if options are not included in proposal doc, default to pass/fail
    if (option_groups.size() == 0)
    {
        document_graph::content_group pass_option_cg = document_graph::content_group{};
        pass_option_cg.push_back(_document_graph.new_content("content_group_type", common::GROUP_TYPE_OPTION));
        pass_option_cg.push_back(_document_graph.new_content("option_key", string("Pass")));
        pass_option_cg.push_back(_document_graph.new_content("vote_power", asset{0, common::S_HVOICE}));
        pass_option_cg.push_back(_document_graph.new_content("description", string("Voting for this option is a vote to pass/approve the proposal")));

        document_graph::content_group fail_option_cg = document_graph::content_group{};
        fail_option_cg.push_back(_document_graph.new_content("content_group_type", common::GROUP_TYPE_OPTION));
        fail_option_cg.push_back(_document_graph.new_content("option_key", string("Fail")));
        fail_option_cg.push_back(_document_graph.new_content("vote_power", asset{0, common::S_HVOICE}));
        fail_option_cg.push_back(_document_graph.new_content("description", string("Voting for this option is a vote to fail/disapprove the proposal")));

        option_groups.push_back(pass_option_cg);
        option_groups.push_back(fail_option_cg);
    }
    return _document_graph.create_document(proposer, option_groups);
}

document_graph::content_group hyphadao::create_system_group(const name &proposer,
                                                            const name &proposal_type,
                                                            const string &decide_title,
                                                            const string &decide_desc,
                                                            const string &decide_content)

{
    // create the system content_group and populate with system details
    
    name ballot_id = register_ballot(proposer, decide_title, decide_desc, decide_content);

    document_graph::content_group system_cg = document_graph::content_group{};
    system_cg.push_back(_document_graph.new_content("content_group_label", "system"));
    system_cg.push_back(_document_graph.new_content(common::CLIENT_VERSION, get_setting<string>(common::CLIENT_VERSION)));
    system_cg.push_back(_document_graph.new_content(common::CONTRACT_VERSION, get_setting<string>(common::CONTRACT_VERSION)));
    system_cg.push_back(_document_graph.new_content("ballot_id", ballot_id));
    system_cg.push_back(_document_graph.new_content("proposer", proposer));
    system_cg.push_back(_document_graph.new_content(common::TYPE, proposal_type));
    return system_cg;
}

document_graph::document hyphadao::propose_badge(const name &proposer, std::vector<document_graph::content_group> &content_groups)
{
    // grab the proposal details - enforce required (strict) inputs
    document_graph::content_group proposal_details = _document_graph.get_content_group(content_groups, common::DETAILS, true);

    // check coefficients
    // TODO: move coeffecient thresholds to be configuration values
    check_coefficient(proposal_details, "husd_coefficient_x10000");
    check_coefficient(proposal_details, "hypha_coefficient_x10000");
    check_coefficient(proposal_details, "hvoice_coefficient_x10000");
    check_coefficient(proposal_details, "seeds_coefficient_x10000");

    // handle ballot type - specific functionality
    document_graph::flexvalue ballot_type = _document_graph.get_content(proposal_details, common::BALLOT_TYPE, false);
    document_graph::document vote_tally_document;
    bool add_vote_tally_edge = false;
    name ballot_type_name;

    if (ballot_type == _document_graph.DOES_NOT_EXIST)
    {
        // default to telos decide
        ballot_type_name = name(common::BALLOT_TYPE_TELOS_DECIDE);
    } 
    else 
    {
        check(std::holds_alternative<name>(ballot_type), "fatal error: ballot_type must be a name type");
        ballot_type_name = std::get<name>(ballot_type);
    }
    
    switch (ballot_type_name.value)
    {
    case common::BALLOT_TYPE_OPTIONS.value:
        vote_tally_document = create_votetally_doc(proposer, content_groups);
        add_vote_tally_edge = true;
        break;

    case common::BALLOT_TYPE_TELOS_DECIDE.value:
        // TODO: apply time-frame for expiration of badge
        content_groups.push_back(create_system_group(proposer,
                                                        common::BADGE_NAME,
                                                        std::get<string>(_document_graph.get_content(proposal_details, common::TITLE, true)),
                                                        std::get<string>(_document_graph.get_content(proposal_details, common::DESCRIPTION, true)),
                                                        std::get<string>(_document_graph.get_content(proposal_details, common::ICON, true))));
        break;

    default:
        check(false, "Unknown ballot type: " + ballot_type_name.to_string());
    }
    
    // creates the document, or the graph NODE
    document_graph::document proposal_doc = _document_graph.create_document(proposer, content_groups);

    // the proposer OWNS the proposal; this creates the graph EDGE
    _document_graph.create_edge(get_member_doc(proposer).hash, proposal_doc.hash, common::OWNS);

    // the proposal was PROPOSED_BY proposer; this creates the graph EDGE
    _document_graph.create_edge(proposal_doc.hash, get_member_doc(proposer).hash, common::OWNED_BY);

    // the DHO also links to the document as a proposal, another graph EDGE
    _document_graph.create_edge(get_root(), proposal_doc.hash, common::PROPOSAL);

    if (add_vote_tally_edge)
    {
        _document_graph.create_edge(proposal_doc.hash, vote_tally_document.hash, common::VOTE_TALLY);
    }

    return proposal_doc;
}

document_graph::document hyphadao::propose_badge_assignment(const name &proposer,
                                                            std::vector<document_graph::content_group> &content_groups)
{
    // grab the proposal details - enforce required (strict) inputs
    document_graph::content_group details = _document_graph.get_content_group(content_groups, common::DETAILS, true);

    // badge assignee must exist
    name assignee = std::get<name>(_document_graph.get_content(details, common::ASSIGNEE, true));

    // badge assignee must be a DHO member
    verify_membership (assignee);

    // TODO: Additional input cleansing
    // start_period and end_period must be valid, no more than X periods in between

    // badge assignment proposal must link to a valid badge
    document_graph::document badge = _document_graph.get_document(std::get<checksum256>(_document_graph.get_content(details, common::BADGE_STRING, true)));
    string badge_title = std::get<string>(_document_graph.get_content(badge, common::DETAILS, common::TITLE, true));

    // badge in the proposal must be of type: badge
    check(std::get<name>(_document_graph.get_content(badge, common::SYSTEM, common::TYPE, true)) == common::BADGE_NAME,
          "badge document hash provided in assignment proposal is not of type badge");

    content_groups.push_back(create_system_group(proposer,
                                                 common::ASSIGN_BADGE,
                                                 std::get<string>(_document_graph.get_content(details, common::TITLE, true)),
                                                 std::get<string>(_document_graph.get_content(details, common::DESCRIPTION, true)),
                                                 "Assign badge " + badge_title + " to ")); // + assignee.to_string()));

    // creates the document, or the graph NODE
    document_graph::document proposal_doc = _document_graph.create_document(proposer, content_groups);
    auto member_doc_hash = get_member_doc(assignee).hash;

    // update graph edges:
    //    member    ---- owns       ---->   proposal
    //    proposal  ---- ownedby    ---->   member
    //    root      ---- proposal   ---->   proposal
    
    // the proposer OWNS the proposal; this creates the graph EDGE
    _document_graph.create_edge(member_doc_hash, proposal_doc.hash, common::OWNS);

    // the proposal was PROPOSED_BY proposer; this creates the graph EDGE
    _document_graph.create_edge(proposal_doc.hash, member_doc_hash, common::OWNED_BY);

    // the DHO also links to the document as a proposal, another graph EDGE
    _document_graph.create_edge(get_root(), proposal_doc.hash, common::PROPOSAL);

    return proposal_doc;
}

void hyphadao::assign_badge(const document_graph::document &badge_assignment)
{
    // document nodes we need: 
    //      badge, 
    //      badge_assignment, and 
    //      member 

    document_graph::content_group details = _document_graph.get_content_group(badge_assignment, common::DETAILS, true);
    document_graph::document badge = _document_graph.get_document(std::get<checksum256>(_document_graph.get_content(
                    details, common::BADGE_STRING, true)));
    name assignee = std::get<name>(_document_graph.get_content(details, common::ASSIGNEE, true));
    checksum256 member_doc_hash = get_member_doc(assignee).hash;

    // update graph edges:
    //    member            ---- holdsbadge     ---->   badge
    //    member            ---- badgeassign    ---->   badge_assignment
    //    badge             ---- heldby         ---->   member
    //    badge             ---- assignment     ---->   badge_assignment
    //    badge_assignment  ---- badge          ---->   badge

    // the assignee now HOLDS this badge, non-strict in case the member already has the badge
    _document_graph.create_edge(member_doc_hash, badge.hash, common::HOLDS_BADGE, false);

    // the assignee now is badgeassigned this badge assignment
    _document_graph.create_edge(member_doc_hash, badge_assignment.hash, common::ASSIGN_BADGE);

    // the badge also links back to the assignee, non-strict in case the member already has the badge
    _document_graph.create_edge(badge.hash, member_doc_hash, common::HELD_BY, false);

    _document_graph.create_edge(badge.hash, badge_assignment.hash, common::ASSIGNMENT);

    _document_graph.create_edge(badge_assignment.hash, badge.hash, common::BADGE_NAME);

}
