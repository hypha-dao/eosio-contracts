#pragma once
#include <eosio/name.hpp>
#include <document_graph.hpp>

namespace hyphaspace {

    class hyphadao;

    class Proposal
    {

    public:

        Proposal (hyphadao& dao);
        virtual ~Proposal ();
        
        document_graph::document propose(const name &proposer, 
            std::vector<document_graph::content_group> &content_groups);
        
        void close(document_graph::document proposal);

        hyphadao& m_dao;
    protected: 

        virtual std::vector<document_graph::content_group> propose_impl(const name &proposer, 
            std::vector<document_graph::content_group> &content_groups) = 0;

        virtual document_graph::document pass_impl (document_graph::document proposal) = 0;

        virtual string GetBallotContent (document_graph::content_group proposal_details) = 0;

        virtual name GetProposalType () = 0;

        document_graph::content_group create_system_group(const name &proposer,
                                                        const name &proposal_type,
                                                        const string &decide_title,
                                                        const string &decide_desc,
                                                        const string &decide_content);

        void verify_membership (const name& member);

        bool did_pass(const name &ballot_id);

        name register_ballot(const name &proposer,
							   const map<string, string> &strings);

        name register_ballot(const name &proposer,
                            const string &title, const string &description, const string &content);
    };
}