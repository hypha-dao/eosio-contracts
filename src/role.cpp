#include <hyphadao.hpp>

using namespace hyphaspace;

document_graph::document hyphadao::propose_role(const name &proposer, std::vector<document_graph::content_group> &content_groups)
{
    // grab the proposal details - enforce required (strict) inputs
    document_graph::content_group proposal_details = _document_graph.get_content_group(content_groups, common::DETAILS, true);

    content_groups.push_back(create_system_group(proposer,
                                                common::BADGE_NAME,
                                                std::get<string>(_document_graph.get_content(proposal_details, common::TITLE, true)),
                                                std::get<string>(_document_graph.get_content(proposal_details, common::DESCRIPTION, true)),
                                                std::get<string>(_document_graph.get_content(proposal_details, common::ICON, true))));

    // creates the document, or the graph NODE
    document_graph::document proposal_doc = _document_graph.create_document(proposer, content_groups);

    // the proposer OWNS the proposal; this creates the graph EDGE
    _document_graph.create_edge(get_member_doc(proposer).hash, proposal_doc.hash, common::OWNS);

    // the proposal was PROPOSED_BY proposer; this creates the graph EDGE
    _document_graph.create_edge(proposal_doc.hash, get_member_doc(proposer).hash, common::OWNED_BY);

    // the DHO also links to the document as a proposal, another graph EDGE
    _document_graph.create_edge(get_root(), proposal_doc.hash, common::PROPOSAL);

    return proposal_doc;
}

void hyphadao::create_role(const document_graph::document &role)
{
    _document_graph.create_edge(get_root(), role.hash, common::ROLE_NAME);
}

document_graph::document hyphadao::propose_role_assignment(const name &proposer,
                                                            std::vector<document_graph::content_group> &content_groups)
{
    // grab the proposal details - enforce required (strict) inputs
    document_graph::content_group details = _document_graph.get_content_group(content_groups, common::DETAILS, true);

    // assignee must exist
    name assignee = std::get<name>(_document_graph.get_content(details, common::ASSIGNEE, true));

    // assignee must be a DHO member
    verify_membership (assignee);

    // TODO: Additional input cleansing
    // start_period and end_period must be valid, no more than X periods in between

    // assignment proposal must link to a valid role
    document_graph::document role = _document_graph.get_document(std::get<checksum256>(_document_graph.get_content(details, common::ROLE_STRING, true)));
    string role_title = std::get<string>(_document_graph.get_content(role, common::DETAILS, common::TITLE, true));

    // role in the proposal must be of type: role
    check(std::get<name>(_document_graph.get_content(role, common::SYSTEM, common::TYPE, true)) == common::ROLE_NAME,
          "role document hash provided in assignment proposal is not of type role");

    content_groups.push_back(create_system_group(proposer,
                                                 common::ASSIGN_BADGE,
                                                 std::get<string>(_document_graph.get_content(details, common::TITLE, true)),
                                                 std::get<string>(_document_graph.get_content(details, common::DESCRIPTION, true)),
                                                 "Assign role " + role_title + " to " + assignee.to_string()));

    // creates the document, or the graph NODE
    document_graph::document proposal_doc = _document_graph.create_document(proposer, content_groups);
    auto member_doc_hash = get_member_doc(proposer).hash;

    // update graph edges:
    //    member    ---- owns       ---->   proposal
    //    proposal  ---- ownedby    ---->   member
    //    root      ---- proposal   ---->   proposal
    //    proposal  ---- role       ---->   role

    // the proposer OWNS the proposal; this creates the graph EDGE
    //  member    ---- owns       ----> proposal
    _document_graph.create_edge(member_doc_hash, proposal_doc.hash, common::OWNS);

    // the proposal was PROPOSED_BY proposer; this creates the graph EDGE
    //  proposal  ---- ownedby    ----> member
    _document_graph.create_edge(proposal_doc.hash, member_doc_hash, common::OWNED_BY);

    //  proposal ---- role       ----> role
    _document_graph.create_edge(proposal_doc.hash, role.hash, common::ROLE_NAME);

    //  root        ---- proposal   ---->   proposal
    _document_graph.create_edge(get_root(), proposal_doc.hash, common::PROPOSAL);

    return proposal_doc;
}

void hyphadao::assign_role(const document_graph::document &role_assignment)
{
    document_graph::content_group details = _document_graph.get_content_group(role_assignment, common::DETAILS, true);
    document_graph::document role = _document_graph.get_document(std::get<checksum256>(_document_graph.get_content(
                    details, common::ROLE_STRING, true)));
    name assignee = std::get<name>(_document_graph.get_content(details, common::ASSIGNEE, true));
    checksum256 member_doc_hash = get_member_doc(assignee).hash;

    // update graph edges:
    //  member          ---- assigned           ---->   role_assignment
    //  role_assignment ---- assignee           ---->   member
    //  role_assignment ---- role               ---->   role
    //  role            ---- role_assignment    ---->   role_assignment

    // what about periods?  

    //  member          ---- assigned           ---->   role_assignment
    _document_graph.create_edge(member_doc_hash, role_assignment.hash, common::ASSIGNED);

    //  role_assignment ---- assignee           ---->   member
    _document_graph.create_edge(role_assignment.hash, member_doc_hash, common::ASSIGNEE_NAME);

    //  role_assignment ---- role               ----> role
    _document_graph.create_edge(role_assignment.hash, role.hash, common::ROLE_NAME);

    //  role            ---- role_assignment    ----> role_assignment
    _document_graph.create_edge(role.hash, role_assignment.hash, common::ASSIGNMENT);
}