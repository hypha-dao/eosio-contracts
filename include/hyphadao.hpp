#ifndef HYPHADAO_H
#define HYPHADAO_H

#include <eosio/eosio.hpp>
#include <eosio/asset.hpp>
#include <eosio/singleton.hpp>
#include <eosio/multi_index.hpp>
#include <eosio/transaction.hpp>

#include "common.hpp"
#include "holocracy.hpp"
#include "trail.hpp"
#include <cryptoutil.hpp>

using namespace eosio;
using cryptoutil::hash;
using std::string;

CONTRACT hyphadao : public contract {
   public:
      using contract::contract;

      struct [[ eosio::table, eosio::contract("hyphadao") ]] AppState 
      {
         uint64_t    last_sender_id;
      };

      typedef singleton<"appstates"_n, AppState> appstate_table;
      typedef eosio::multi_index<"appstates"_n, AppState> appstate_table_placeholder;

      struct [[eosio::table, eosio::contract("hyphadao") ]] DAOConfig 
      {
         name           trail_contract          ;
         name           last_ballot_id          = name("hypha1");
      };

      typedef singleton<"config"_n, DAOConfig> config_table;
      typedef multi_index<"config"_n, DAOConfig> config_table_placehoder;

      struct [[eosio::table, eosio::contract("hyphadao") ]] Member 
      {
         name           member                  ;
         uint64_t       primary_key() const { return member.value; }
      };

      typedef multi_index<"members"_n, Member> member_table;

      struct [[eosio::table, eosio::contract("hyphadao") ]] Applicant 
      {
         name           applicant                  ;
         string         content                    ;
         
         time_point  created_date = current_time_point();
         time_point  updated_date = current_time_point();

         uint64_t       primary_key() const { return applicant.value; }
      };
      typedef multi_index<"applicants"_n, Applicant> applicant_table;

      // scope: proposal type (name)
      struct [[eosio::table, eosio::contract("hyphadao") ]] Proposal
      {
         uint64_t                   id                ;
         
         // core maps
         map<string, name>          names             ;
         map<string, string>        strings           ;
         map<string, asset>         assets            ;
         map<string, time_point>    time_points       ;
         map<string, uint64_t>      ints              ;
         map<string, transaction>   trxs              ;
         map<string, float>         floats            ;
         uint64_t                   primary_key()     const { return id; }

         // indexes
         name                       proposer          ; 
         uint64_t                   by_proposer()     const { return proposer.value; }

         time_point                 created_date    = current_time_point();
         time_point                 updated_date    = current_time_point();
         uint64_t    by_created () const { return created_date.sec_since_epoch(); }
         uint64_t    by_updated () const { return updated_date.sec_since_epoch(); }

         uint64_t    by_type () const { return names.at("proposal_type").value; }
      };

      typedef multi_index<"proposals"_n, Proposal,
         indexed_by<"bycreated"_n, const_mem_fun<Proposal, uint64_t, &Proposal::by_created>>,
         indexed_by<"byupdated"_n, const_mem_fun<Proposal, uint64_t, &Proposal::by_updated>>,
         indexed_by<"byproposer"_n, const_mem_fun<Proposal, uint64_t, &Proposal::by_proposer>>,
         indexed_by<"bytype"_n, const_mem_fun<Proposal, uint64_t, &Proposal::by_type>>
      > proposal_table;

      struct [[eosio::table, eosio::contract("hyphadao") ]] Debug
      {
         uint64_t    debug_id;
         string      notes;
         time_point  created_date = current_time_point();
         uint64_t    primary_key()  const { return debug_id; }
      };

      typedef multi_index<"debugs"_n, Debug> debug_table;

      /*
         Comments in code may be outdated. See docs at https://docs.hypha.earth
         
         Proposal attributes (all proposals)

            - type: int
               - "status"
               - "start_period"
               - "end_period"
            
            - type: name
               - "ballot_id": used to interface to Trail

            - type: string
               - "title"  
               - "description"
               - "content"

            - type: asset
               - "hypha_amount"
               - "seeds_amount"
               - "hvoice_amount"

            - type: time_point_sec
               - "created_date"
               - "updated_date"

            - type: transaction
               - "transaction"

         Role proposals
            - scoped to "roles"_n.value

         Assignment Proposals:
            - scoped to "assignments"_n.value
            - type: float
               - "time_share"

            - type: name
               - "assigned_account"

            - type: int
               - "role_id"
         
         Payout Proposal:
            - scoped to "payouts"_n.value
            - type: name
               - "recipient"
            - type: time_point
               - "contribution_date"

      */
     ACTION propose (const map<string, name> 		   names,
                     const map<string, string>       strings,
                     const map<string, asset>        assets,
                     const map<string, time_point>   time_points,
                     const map<string, uint64_t>     ints,
                     const map<string, float>        floats,
                     const map<string, transaction>  trxs);
      
      ACTION apply (const name&     applicant, const string& content);

      ACTION enroll (const name& enroller,
                     const name& applicant, 
					      const string& content);

      // Admin
      ACTION reset ();
      ACTION resetperiods();
      ACTION eraseprop (const name&       proposal_type,
                        const uint64_t&   proposal_id);

      ACTION setconfig (const name&    hypha_token_contract,
                        const name&    trail_contract);
      ACTION setlastballt (const name& last_ballot_id);

      ACTION clrdebugs (const uint64_t& starting_id, const uint64_t& batch_size);

      ACTION addperiod (const time_point& start_time, 
                        const time_point& end_time, 
                        const string& phase);
      ACTION remperiods (const uint64_t& begin_period_id, 
                         const uint64_t& end_period_id);

      // These actions are executed only on approval of a proposal. 
      // To introduce a new proposal type, we would add another action to the below.
      ACTION newrole    (  const uint64_t&   proposal_id);
      ACTION assign     (  const uint64_t& 	proposal_id);
      ACTION makepayout (  const uint64_t&   proposal_id);
      ACTION exectrx    (  const uint64_t&   proposal_id);
      
      // anyone can call closeprop, it executes the transaction if the voting passed
      ACTION closeprop(const name& proposal_type, const uint64_t& proposal_id);

      // users can claim their salary pay
      ACTION payassign(const uint64_t& assignment_id, const uint64_t& period_id);
            
      // temporary hack - keep a list of the members, although true membership is governed by token holdings
      ACTION removemember(const name& member_to_remove);
      ACTION addmember (const name& member);
      
   private:
      Holocracy holocracy = Holocracy (get_self());
      Bank bank = Bank (get_self());

      void defcloseprop (const uint64_t& proposal_id);
      void qualify_proposer (const name& proposer);
      name register_ballot (const name& proposer, 
								      const map<string, string>& strings);

      uint64_t get_next_sender_id()
      {
         appstate_table a_t (get_self(), get_self().value);
         AppState state = a_t.get_or_create (get_self(), AppState());

         uint64_t return_senderid = state.last_sender_id;
         return_senderid++;
         state.last_sender_id = return_senderid;
         a_t.set (state, get_self());
         return return_senderid;
      }

      void debug (const string& notes) {
         debug_table d_t (get_self(), get_self().value);
         d_t.emplace (get_self(), [&](auto &d) {
            d.debug_id = d_t.available_primary_key();
            d.notes = notes;
         });
      }
};

#endif