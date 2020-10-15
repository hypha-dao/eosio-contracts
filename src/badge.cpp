
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

document_graph::content_group hyphadao::create_system_group (const name& proposer, const name& proposal_type, std::vector<document_graph::content_group> &content_groups)
{
    // grab the proposal details - enforce required (strict) inputs 
    document_graph::content_group proposal_details = _document_graph.get_content_group (content_groups, "details", true);
    // register the ballot with the title, desc, and icon (strictly required, for now?)
    document_graph::flexvalue title = _document_graph.get_content (proposal_details, "title", true);
    document_graph::flexvalue description = _document_graph.get_content (proposal_details, "description", true);
    document_graph::flexvalue icon = _document_graph.get_content (proposal_details, "icon", true);
    name ballot_id = register_ballot(proposer, std::get<string>(title), std::get<string>(description), std::get<string>(icon));

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
    
    content_groups.push_back (create_system_group (proposer, common::BADGE_NAME, content_groups));

    // creates the document, or the graph NODE
    document_graph::document proposal_doc = _document_graph.create_document (proposer, content_groups);

    // the proposer OWNS the proposal, this creates the graph EDGE 
    _document_graph.create_edge (get_member_doc(proposer).hash, proposal_doc.hash, common::OWNS);

    // the DHO also links to the document as a proposal, another graph EDGE
    _document_graph.create_edge (get_member_doc(get_self()).hash, proposal_doc.hash, common::PROPOSED_BY);
           
    return proposal_doc;
}

document_graph::document hyphadao::propose_badge_assignment (const name& proposer, std::vector<document_graph::content_group> &content_groups)
{
    // grab the proposal details - enforce required (strict) inputs 
    document_graph::content_group details = _document_graph.get_content_group (content_groups, common::DETAILS, true);

    // badge assignee must exist
    // TODO: badge assignee must be a DHO member
    check ( std::holds_alternative<name>(_document_graph.get_content (details, common::ASSIGNEE, true)), 
        "badge assignment proposal must have an assignee content item in the details content group");
    
    // badge assignment proposal must link to a valid badge
    document_graph::document badge = _document_graph.get_document (std::get<checksum256>(_document_graph.get_content (details, common::BADGE_STRING, true)));

    // badge in the proposal must be of type: badge
    check ( std::get<name>(_document_graph.get_content (details, common::TYPE, true)) == common::BADGE_NAME, 
        "badge document hash provided in assignment proposal is not of type badge");

    content_groups.push_back(create_system_group (proposer, common::ASSIGN_BADGE, content_groups));
    return _document_graph.create_document (proposer, content_groups);
}

void hyphadao::assign_badge (const document_graph::document &badge, const name &assignee)
{
    // update graph edges
    // the assignee has EARNED this badge
    _document_graph.create_edge (get_member_doc(assignee).hash, badge.hash, common::HOLDS_BADGE);

    // the badge also links back to the assignee
    _document_graph.create_edge (badge.hash, get_member_doc(assignee).hash, common::HELD_BY);
}

