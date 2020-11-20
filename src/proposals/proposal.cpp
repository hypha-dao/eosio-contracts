#include <Proposals/Proposal.hpp>
#include <document_graph.hpp>
#include <hyphadao.hpp>

namespace hyphaspace
{
    Proposal::Proposal(document_graph graph) : m_graph{graph} {}

    document_graph::document Proposal::propose(const name &proposer, std::vector<document_graph::content_group> &content_groups)
    {
        verify_membership (proposer) ;
        content_groups = propose_impl(proposer, content_groups);

        // grab the proposal details - enforce required (strict) inputs
        document_graph::content_group proposal_details = document_graph::get_content_group(content_groups, common::DETAILS, true);

        content_groups.push_back(create_system_group(proposer,
                                                     GetProposalType(),
                                                     std::get<string>(document_graph::get_content(proposal_details, common::TITLE, true)),
                                                     std::get<string>(document_graph::get_content(proposal_details, common::DESCRIPTION, true)),
                                                     GetBallotContent(proposal_details)));

        // creates the document, or the graph NODE
        document_graph::document proposal_doc = m_graph.create_document(proposer, content_groups);

        // the proposer OWNS the proposal; this creates the graph EDGE
        m_graph.create_edge(hyphadao::get_member_doc(proposer).hash, proposal_doc.hash, common::OWNS);

        // the proposal was PROPOSED_BY proposer; this creates the graph EDGE
        m_graph.create_edge(proposal_doc.hash, hyphadao::get_member_doc(proposer).hash, common::OWNED_BY);

        // the DHO also links to the document as a proposal, another graph EDGE
        m_graph.create_edge(hyphadao::get_root(m_graph.contract), proposal_doc.hash, common::PROPOSAL);

        return proposal_doc;
    }

    void Proposal::close(document_graph::document proposal)
    {
        checksum256 root_hash = hyphadao::get_root(m_graph.contract);
        m_graph.remove_edge(root_hash, proposal.hash, common::PROPOSAL, true);

        name ballot_id = std::get<name>(m_graph.get_content(proposal, common::SYSTEM, common::BALLOT_ID, true));
        if (did_pass(ballot_id))
        {
            // INVOKE child class close logic
            close_impl(proposal);

            // if proposal passes, create an edge for PASSED_PROPS
            m_graph.create_edge(root_hash, proposal.hash, common::PASSED_PROPS);
        }
        else
        {
            // create edge for FAILED_PROPS
            m_graph.create_edge(root_hash, proposal.hash, common::FAILED_PROPS);
        }

        hyphadao::config_table config_s(m_graph.contract, m_graph.contract.value);
        hyphadao::Config c = config_s.get_or_create(m_graph.contract, hyphadao::Config());

        action(
            permission_level{m_graph.contract, name("active")},
            c.names.at("telos_decide_contract"), name("closevoting"),
            std::make_tuple(ballot_id, true))
            .send();
    }

    document_graph::content_group Proposal::create_system_group(const name &proposer,
                                                                const name &proposal_type,
                                                                const string &decide_title,
                                                                const string &decide_desc,
                                                                const string &decide_content)

    {
        // create the system content_group and populate with system details
        hyphadao::config_table config_s(m_graph.contract, m_graph.contract.value);
        hyphadao::Config c = config_s.get_or_create(m_graph.contract, hyphadao::Config());

        name ballot_id = register_ballot(proposer, decide_title, decide_desc, decide_content);

        document_graph::content_group system_cg = document_graph::content_group{};
        system_cg.push_back(m_graph.new_content("content_group_label", "system"));
        system_cg.push_back(m_graph.new_content("client_version", hyphadao::get_string(c.strings, "client_version")));
        system_cg.push_back(m_graph.new_content("contract_version", hyphadao::get_string(c.strings, "contract_version")));
        system_cg.push_back(m_graph.new_content("ballot_id", ballot_id));
        system_cg.push_back(m_graph.new_content("proposer", proposer));
        system_cg.push_back(m_graph.new_content(common::TYPE, proposal_type));
        return system_cg;
    }

    void Proposal::verify_membership(const name &member)
    {
        // create hash to represent this member account
        std::vector<document_graph::content_group> member_cgs;
        document_graph::content_group member_cg = document_graph::content_group{};
        member_cg.push_back(document_graph::new_content("member", member));
        member_cgs.push_back(member_cg);
        checksum256 member_hash = document_graph::hash_document(member_cgs);

        // check to see if this member has a document saved
        document_graph::document member_doc = m_graph.get_document(member_hash);

        checksum256 root_hash = hyphadao::get_root(m_graph.contract);
        // verify that the member_hash is a MEMBER of the root_hash
        document_graph::edge_table e_t(m_graph.contract, m_graph.contract.value);
        auto itr = e_t.find(document_graph::edge_id(root_hash, member_hash, common::MEMBER));

        check(itr != e_t.end(), "account: " + member.to_string() + " is not a member of " + document_graph::readable_hash(root_hash));
    }

    bool Proposal::did_pass(const name &ballot_id)
    {
        hyphadao::config_table config_s(m_graph.contract, m_graph.contract.value);
        hyphadao::Config c = config_s.get_or_create(m_graph.contract, hyphadao::Config());

        trailservice::trail::ballots_table b_t(c.names.at("telos_decide_contract"), c.names.at("telos_decide_contract").value);
        auto b_itr = b_t.find(ballot_id.value);
        check(b_itr != b_t.end(), "ballot_id: " + ballot_id.to_string() + " not found.");

        trailservice::trail::treasuries_table t_t(c.names.at("telos_decide_contract"), c.names.at("telos_decide_contract").value);
        auto t_itr = t_t.find(common::S_HVOICE.code().raw());
        check(t_itr != t_t.end(), "Treasury: " + common::S_HVOICE.code().to_string() + " not found.");

        asset quorum_threshold = hyphadao::adjust_asset(t_itr->supply, 0.20000000);
        map<name, asset> votes = b_itr->options;
        asset votes_pass = votes.at(name("pass"));
        asset votes_fail = votes.at(name("fail"));
        asset votes_abstain = votes.at(name("abstain"));

        bool passed = false;
        if (b_itr->total_raw_weight >= quorum_threshold &&                 // must meet quorum
            hyphadao::adjust_asset(votes_pass, 0.2500000000) > votes_fail) // must have 80% of the vote power
        {
            return true;
        }
        else
        {
            return false;
        }
    }

    name Proposal::register_ballot(const name &proposer,
                                   const string &title, const string &description, const string &content)
    {
        check(has_auth(proposer) || has_auth(m_graph.contract), "Authentication failed. Must have authority from proposer: " +
                                                                    proposer.to_string() + "@active or " + m_graph.contract.to_string() + "@active.");
        hyphadao::config_table config_s(m_graph.contract, m_graph.contract.value);
        hyphadao::Config c = config_s.get_or_create(m_graph.contract, hyphadao::Config());

        // increment the ballot_id
        name new_ballot_id = name(c.names.at("last_ballot_id").value + 1);
        c.names["last_ballot_id"] = new_ballot_id;
        config_s.set(c, m_graph.contract);

        trailservice::trail::ballots_table b_t(c.names.at("telos_decide_contract"), c.names.at("telos_decide_contract").value);
        auto b_itr = b_t.find(new_ballot_id.value);
        check(b_itr == b_t.end(), "ballot_id: " + new_ballot_id.to_string() + " has already been used.");

        vector<name> options;
        options.push_back(name("pass"));
        options.push_back(name("fail"));
        options.push_back(name("abstain"));

        action(
            permission_level{m_graph.contract, name("active")},
            c.names.at("telos_decide_contract"), name("newballot"),
            std::make_tuple(
                new_ballot_id,
                name("poll"),
                m_graph.contract,
                common::S_HVOICE,
                name("1token1vote"),
                options))
            .send();

        //	  // default is to vote all tokens, not just staked tokens
        //    action (
        //       permission_level{m_graph.contract, "active"_n},
        //       c.telos_decide_contract, "togglebal"_n,
        //       std::make_tuple(new_ballot_id, "votestake"_n))
        //    .send();

        action(
            permission_level{m_graph.contract, name("active")},
            c.names.at("telos_decide_contract"), name("editdetails"),
            std::make_tuple(
                new_ballot_id,
                title,
                description.substr(0, std::min(description.length(), size_t(25))),
                content))
            .send();

        auto expiration = time_point_sec(current_time_point()) + c.ints.at("voting_duration_sec");

        action(
            permission_level{m_graph.contract, name("active")},
            c.names.at("telos_decide_contract"), name("openvoting"),
            std::make_tuple(new_ballot_id, expiration))
            .send();

        // TODO: add a record to the seeds scheduler to close the proposal
        return new_ballot_id;
    }

    name Proposal::register_ballot(const name &proposer,
                                   const map<string, string> &strings)
    {
        return register_ballot(proposer, strings.at("title"), strings.at("description"), strings.at("content"));
    }

} // namespace hyphaspace
