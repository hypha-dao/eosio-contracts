
#include <hyphadao.hpp>

using namespace hyphaspace;

void hyphadao::check_coefficient (document_graph::content_group &content_group, const string &coefficient_key)
{
    document_graph::flexvalue coefficient_x10000 = _document_graph.get_content (content_group, coefficient_key, false);
    if (coefficient_x10000 != _document_graph.DOES_NOT_EXIST)
    {
        check(std::holds_alternative<int64_t>(coefficient_x10000), "fatal error: coefficient must be an int64_t type: " + coefficient_key);
        check(std::get<int64_t>(coefficient_x10000) >= 9000 &&  
                std::get<int64_t>(coefficient_x10000) <= 11000, "fatal error: coefficient_x10000 must be between 9000 and 11000, inclusive: " + coefficient_key);
    }   
}

document_graph::content_group hyphadao::create_system_group (const name& proposer, 
                                                            const name& proposal_type, 
                                                            const string& decide_title,
                                                            const string& decide_desc,
                                                            const string& decide_content)
                                                             
{
    name ballot_id = register_ballot(proposer, decide_title, decide_desc, decide_content);

    // create the system content_group and populate with system details
    config_table config_s(get_self(), get_self().value);
    Config c = config_s.get_or_create(get_self(), Config());
    document_graph::content_group system_cg = document_graph::content_group {};
    system_cg.push_back (_document_graph.new_content("content_group_label", "system"));
    system_cg.push_back (_document_graph.new_content("client_version", get_string(c.strings, "client_version")));
    system_cg.push_back (_document_graph.new_content("contract_version", get_string(c.strings, "contract_version")));
    system_cg.push_back (_document_graph.new_content("ballot_id", ballot_id));
    system_cg.push_back (_document_graph.new_content("proposer", proposer));
    system_cg.push_back (_document_graph.new_content(common::TYPE, proposal_type));
    return system_cg;
}

document_graph::document hyphadao::propose_badge (const name& proposer, std::vector<document_graph::content_group> &content_groups)
{
    // grab the proposal details - enforce required (strict) inputs 
    document_graph::content_group proposal_details = _document_graph.get_content_group (content_groups, common::DETAILS, true);
    
    // check coefficients 
    // TODO: move coeffecient thresholds to be configuration values
    check_coefficient (proposal_details, "husd_coefficient_x10000");
    check_coefficient (proposal_details, "hypha_coefficient_x10000");
    check_coefficient (proposal_details, "hvoice_coefficient_x10000");
    check_coefficient (proposal_details, "seeds_coefficient_x10000");

    // TODO: apply time-frame for expiration of badge
    content_groups.push_back (create_system_group (proposer, 
                                common::BADGE_NAME, 
                                std::get<string>(_document_graph.get_content (proposal_details, common::TITLE, true)),
                                std::get<string>(_document_graph.get_content (proposal_details, common::DESCRIPTION, true)),
                                std::get<string>(_document_graph.get_content (proposal_details, common::ICON, true))));

    // creates the document, or the graph NODE
    document_graph::document proposal_doc = _document_graph.create_document (proposer, content_groups);

    // the proposer OWNS the proposal; this creates the graph EDGE 
    _document_graph.create_edge (get_member_doc(proposer).hash, proposal_doc.hash, common::OWNS);

    // the proposal was PROPOSED_BY proposer; this creates the graph EDGE 
    _document_graph.create_edge (proposal_doc.hash, get_member_doc(proposer).hash, common::OWNED_BY);

    // the DHO also links to the document as a proposal, another graph EDGE
    _document_graph.create_edge (get_root(), proposal_doc.hash, common::PROPOSAL);
           
    return proposal_doc;
}

document_graph::document hyphadao::propose_badge_assignment (const name& proposer, 
                                                            std::vector<document_graph::content_group> &content_groups)
{
    // grab the proposal details - enforce required (strict) inputs 
    document_graph::content_group details = _document_graph.get_content_group (content_groups, common::DETAILS, true);

    // badge assignee must exist    
    name assignee = std::get<name>(_document_graph.get_content (details, common::ASSIGNEE, true));

    // TODO: Additional input cleansing
    // badge assignee must be a DHO member
    // start_period and end_period must be valid, no more than X periods in between
    
    // badge assignment proposal must link to a valid badge
    document_graph::document badge = _document_graph.get_document (std::get<checksum256>(_document_graph.get_content (details, common::BADGE_STRING, true)));
    string badge_title = std::get<string>(_document_graph.get_content (badge, common::DETAILS, common::TITLE, true));

    // badge in the proposal must be of type: badge
    check ( std::get<name>(_document_graph.get_content (badge, common::SYSTEM, common::TYPE, true)) == common::BADGE_NAME, 
        "badge document hash provided in assignment proposal is not of type badge");

    content_groups.push_back (create_system_group (proposer, 
                            common::ASSIGN_BADGE, 
                            std::get<string>(_document_graph.get_content (details, common::TITLE, true)),
                            std::get<string>(_document_graph.get_content (details, common::DESCRIPTION, true)),
                            "Assign badge " + badge_title + " to " + assignee.to_string()));

    // creates the document, or the graph NODE
    document_graph::document proposal_doc = _document_graph.create_document (proposer, content_groups);

    // the proposer OWNS the proposal; this creates the graph EDGE 
    _document_graph.create_edge (get_member_doc(proposer).hash, proposal_doc.hash, common::OWNS);

    // the proposal was PROPOSED_BY proposer; this creates the graph EDGE 
    _document_graph.create_edge (proposal_doc.hash, get_member_doc(proposer).hash, common::OWNED_BY);

    // the DHO also links to the document as a proposal, another graph EDGE
    _document_graph.create_edge (get_root(), proposal_doc.hash, common::PROPOSAL);
           
    return proposal_doc;
}

void hyphadao::assign_badge (const document_graph::document &badge, const name &assignee)
{
    // update graph edges
    // the assignee has EARNED this badge
    _document_graph.create_edge (get_member_doc(assignee).hash, badge.hash, common::HOLDS_BADGE);

    // the badge also links back to the assignee
    _document_graph.create_edge (badge.hash, get_member_doc(assignee).hash, common::HELD_BY);
}

