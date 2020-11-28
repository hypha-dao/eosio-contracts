#pragma once
#include <eosio/name.hpp>
#include <document_graph/document_graph.hpp>

using std::string;
using eosio::name;

namespace hypha {

    class hyphadao;

    class Proposal
    {

    public:

        Proposal (name& contract);
        virtual ~Proposal ();
        
        Document propose(const eosio::name &proposer, ContentGroups &content_groups);
        
        void close(Document proposal);

        name& m_contract;
    protected: 

        virtual ContentGroups propose_impl(const name &proposer, 
            ContentGroups &content_groups) = 0;

        virtual Document pass_impl (Document proposal) = 0;

        virtual string GetBallotContent (ContentGroups contentGroups) = 0;

        virtual name GetProposalType () = 0;

        ContentGroup create_system_group(const name &proposer,
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
        asset adjustAsset(const asset &originalAsset, const float &adjustment);
    };
}