
#include <document_graph.hpp>

namespace hyphaspace
{

    class Proposal
    {
    public:
        document_graph::document propose(const name &proposer, std::vector<document_graph::content_group> &content_groups);
        void close(document_graph::document proposal);

        Proposal *Factory(const name &proposal_type)
        {
        }

    private:
        document_graph::content_group create_system_group(const name &proposer,
                                                          const name &proposal_type,
                                                          const string &decide_title,
                                                          const string &decide_desc,
                                                          const string &decide_content)

        {
            // create the system content_group and populate with system details
            config_table config_s(get_self(), get_self().value);
            Config c = config_s.get_or_create(get_self(), Config());

            name ballot_id = register_ballot(proposer, decide_title, decide_desc, decide_content);

            document_graph::content_group system_cg = document_graph::content_group{};
            system_cg.push_back(_document_graph.new_content("content_group_label", "system"));
            system_cg.push_back(_document_graph.new_content("client_version", get_string(c.strings, "client_version")));
            system_cg.push_back(_document_graph.new_content("contract_version", get_string(c.strings, "contract_version")));
            system_cg.push_back(_document_graph.new_content("ballot_id", ballot_id));
            system_cg.push_back(_document_graph.new_content("proposer", proposer));
            system_cg.push_back(_document_graph.new_content(common::TYPE, proposal_type));
            return system_cg;
        }
    };

    document_graph::document Proposal::propose(const name &proposer, std::vector<document_graph::content_group> &content_groups)
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


class RoleProposal : public hyphaspace::Proposal
{

public:
};

} // namespace hyphaspace
