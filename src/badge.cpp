
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
    document_graph::content_group proposal_details = _document_graph.get_content_group (content_groups, "proposal_details", true);
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
    system_cg.push_back (_document_graph.new_content("proposal_type", proposal_type));
    return system_cg;
}

document_graph::document hyphadao::propose_badge (const name& proposer, std::vector<document_graph::content_group> &content_groups)
{
    // grab the proposal details - enforce required (strict) inputs 
    document_graph::content_group proposal_details = _document_graph.get_content_group (content_groups, "proposal_details", true);
    
    // check coefficients 
    // TODO: move coeffecient thresholds to be configuration values
    check_coefficient (proposal_details, "husd_coefficient_x10000");
    check_coefficient (proposal_details, "hypha_coefficient_x10000");
    check_coefficient (proposal_details, "hvoice_coefficient_x10000");
    check_coefficient (proposal_details, "seeds_coefficient_x10000");
    
    content_groups.push_back (create_system_group (proposer, common::BADGE, content_groups));

    document_graph::document proposal_doc = _document_graph.create_document (proposer, content_groups);

    // the proposer OWNS the proposal on the graph 
    _document_graph.create_edge (get_member_doc(proposer).hash, proposal_doc.hash, name("owns"));

    // the DHO also OWNS the proposal on the graph
    _document_graph.create_edge (get_member_doc(get_self()).hash, proposal_doc.hash, name("proposal"));
           
    return proposal_doc;
}

document_graph::document hyphadao::propose_badge_assignment (const name& proposer, std::vector<document_graph::content_group> &content_groups)
{
    // grab the proposal details - enforce required (strict) inputs 
    content_groups.push_back(create_system_group (proposer, common::ASSIGN_BADGE, content_groups));
    return _document_graph.create_document (proposer, content_groups);
}