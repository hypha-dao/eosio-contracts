#ifndef HYPHADAO_H
#define HYPHADAO_H

#include <eosio/eosio.hpp>
#include <eosio/asset.hpp>
#include <eosio/singleton.hpp>
#include <eosio/multi_index.hpp>
#include <eosio/transaction.hpp>

#include "bank.hpp"
#include "common.hpp"
#include "trail.hpp"
#include <cryptoutil.hpp>

using namespace eosio;
using cryptoutil::hash;
using std::string;

CONTRACT hyphadao : public contract {
   public:
      using contract::contract;

      struct [[eosio::table, eosio::contract("hyphadao") ]] Config 
      {
         // required configurations:
         // names : telos_decide_contract, hypha_token_contract, seeds_token_contract, last_ballot_id
         // ints  : voting_duration_sec
         map<string, name>          names             ;
         map<string, string>        strings           ;
         map<string, asset>         assets            ;
         map<string, time_point>    time_points       ;
         map<string, uint64_t>      ints              ;
         map<string, transaction>   trxs              ;
         map<string, float>         floats            ;
      };

      typedef singleton<"config"_n, Config> config_table;
      typedef multi_index<"config"_n, Config> config_table_placeholder;

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

      struct [[eosio::table, eosio::contract("hyphadao") ]] AssignmentPayout
      {
         uint64_t        ass_payment_id          ;
         uint64_t        assignment_id           ;
         name            recipient               ;
         uint64_t        period_id               ;
         vector<asset>   payments                ;
         time_point      payment_date            ;

         uint64_t        primary_key()           const { return ass_payment_id; }
         uint64_t        by_assignment()         const { return assignment_id; }
         uint64_t        by_period ()            const { return period_id; }
         uint64_t        by_recipient()          const { return recipient.value; }
      };

      typedef multi_index<"asspayouts"_n, AssignmentPayout,
         indexed_by<"byassignment"_n,
               const_mem_fun<AssignmentPayout, uint64_t, &AssignmentPayout::by_assignment>>,
         indexed_by<"byperiod"_n,
               const_mem_fun<AssignmentPayout, uint64_t, &AssignmentPayout::by_period>>,
         indexed_by<"byrecipient"_n,
               const_mem_fun<AssignmentPayout, uint64_t, &AssignmentPayout::by_recipient>>
      > asspay_table;

      // // scope: proposal, proparchive, roles, assignments
      // struct [[eosio::table, eosio::contract("hyphadao") ]] BUObject
      // {
      //    uint64_t                   id                ;
         
      //    // core maps
      //    map<string, name>          names             ;
      //    map<string, string>        strings           ;
      //    map<string, asset>         assets            ;
      //    map<string, time_point>    time_points       ;
      //    map<string, uint64_t>      ints              ;
      //    map<string, transaction>   trxs              ;
      //    map<string, float>         floats            ;
      //    uint64_t                   primary_key()     const { return id; }

      //    // indexes
      //    uint64_t                   by_owner()        const { return names.at("owner").value; }
      //    uint64_t                   by_type ()        const { return names.at("type").value; }
       

      //    // timestamps
      //    time_point                 created_date    = current_time_point();
      //    time_point                 updated_date    = current_time_point();
      //    uint64_t    by_created () const { return created_date.sec_since_epoch(); }
      //    uint64_t    by_updated () const { return updated_date.sec_since_epoch(); }
      // };

      // typedef multi_index<"backupobjs"_n, BUObject,
      //    indexed_by<"bycreated"_n, const_mem_fun<BUObject, uint64_t, &BUObject::by_created>>,
      //    indexed_by<"byupdated"_n, const_mem_fun<BUObject, uint64_t, &BUObject::by_updated>>,
      //    indexed_by<"byowner"_n, const_mem_fun<BUObject, uint64_t, &BUObject::by_owner>>,
      //    indexed_by<"bytype"_n, const_mem_fun<BUObject, uint64_t, &BUObject::by_type>>
         
      // > backup_object_table;


      // scope: proposal, proparchive, role, assignment
      struct [[eosio::table, eosio::contract("hyphadao") ]] Object
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
         uint64_t                   by_owner()        const { return names.at("owner").value; }
         uint64_t                   by_type ()        const { return names.at("type").value; }
         uint64_t                   by_fk()           const { return ints.at("fk"); }
       
         // timestamps
         time_point                 created_date    = current_time_point();
         time_point                 updated_date    = current_time_point();
         uint64_t    by_created () const { return created_date.sec_since_epoch(); }
         uint64_t    by_updated () const { return updated_date.sec_since_epoch(); }
      };

      typedef multi_index<"objects"_n, Object,
         indexed_by<"bycreated"_n, const_mem_fun<Object, uint64_t, &Object::by_created>>, // index 2
         indexed_by<"byupdated"_n, const_mem_fun<Object, uint64_t, &Object::by_updated>>, // 3
         indexed_by<"byowner"_n, const_mem_fun<Object, uint64_t, &Object::by_owner>>, // 4
         indexed_by<"bytype"_n, const_mem_fun<Object, uint64_t, &Object::by_type>>, // 5
         indexed_by<"byfk"_n, const_mem_fun<Object, uint64_t, &Object::by_fk>> // 6
      > object_table;

      struct [[eosio::table, eosio::contract("hyphadao") ]] Debug
      {
         uint64_t    debug_id;
         string      notes;
         time_point  created_date = current_time_point();
         uint64_t    primary_key()  const { return debug_id; }
      };

      typedef multi_index<"debugs"_n, Debug> debug_table;

      ACTION create ( const name&                    scope,
                     const map<string, name> 		  names,
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
      ACTION eraseobjs (const name& scope);
      ACTION eraseobj (const name& scope,
                        const uint64_t&   id);
      ACTION togglepause ();
      ACTION addowner (const name& scope);
      ACTION updtrxs ();
      // ACTION backupobjs (const name& scope);
      // ACTION erasebackups (const name& scope);
      // ACTION restoreobjs (const name& scope);

      ACTION setconfig (const map<string, name> 		  names,
                        const map<string, string>       strings,
                        const map<string, asset>        assets,
                        const map<string, time_point>   time_points,
                        const map<string, uint64_t>     ints,
                        const map<string, float>        floats,
                        const map<string, transaction>  trxs);

      ACTION setlastballt (const name& last_ballot_id);

      ACTION clrdebugs (const uint64_t& starting_id, const uint64_t& batch_size);

      ACTION addperiod (const time_point& start_time, 
                        const time_point& end_time, 
                        const string& phase);
      ACTION remperiods (const uint64_t& begin_period_id, 
                         const uint64_t& end_period_id);

      ACTION remapply (const name& applicant);

      // These actions are executed only on approval of a proposal. 
      // To introduce a new proposal type, we would add another action to the below.
      ACTION newrole    (  const uint64_t&   proposal_id);
      ACTION assign     (  const uint64_t& 	proposal_id);
      ACTION makepayout (  const uint64_t&   proposal_id);
      ACTION exectrx    (  const uint64_t&   proposal_id);
      
      // anyone can call closeprop, it executes the transaction if the voting passed
      ACTION closeprop(const uint64_t& proposal_id);

      // users can claim their salary pay
      ACTION payassign(const uint64_t& assignment_id, const uint64_t& period_id);
            
      // temporary hack (?) - keep a list of the members, although true membership is governed by token holdings
      ACTION removemember(const name& member_to_remove);
      ACTION addmember (const name& member);
      
   private:
      Bank bank = Bank (get_self());

      void defcloseprop (const uint64_t& proposal_id);
      void qualify_proposer (const name& proposer);
      name register_ballot (const name& proposer, 
								      const map<string, string>& strings);

      uint64_t get_next_sender_id()
      {
         config_table      config_s (get_self(), get_self().value);
   	   Config c = config_s.get_or_create (get_self(), Config());   
         uint64_t return_senderid = c.ints.at("last_sender_id");
         return_senderid++;
         c.ints["last_sender_id"] = return_senderid;
         config_s.set (c, get_self());
         return return_senderid;
      }

      void debug (const string& notes) {
         debug_table d_t (get_self(), get_self().value);
         d_t.emplace (get_self(), [&](auto &d) {
            d.debug_id = d_t.available_primary_key();
            d.notes = notes;
         });
      }

      void change_scope (const name& current_scope, const uint64_t& id, const name& new_scope, const bool& remove_old) {

         object_table o_t_current (get_self(), current_scope.value);
	      auto o_itr_current = o_t_current.find(id);
	      check (o_itr_current != o_t_current.end(), "Scope: " + current_scope.to_string() + "; Object ID: " + std::to_string(id) + " does not exist.");

         object_table o_t_new (get_self(), new_scope.value);
	      o_t_new.emplace (get_self(), [&](auto &o) {
            o.id                          = o_t_new.available_primary_key();
            o.names                       = o_itr_current->names;
            o.names["prior_scope"]        = current_scope;
            o.assets                      = o_itr_current->assets;
            o.strings                     = o_itr_current->strings;
            o.floats                      = o_itr_current->floats;
            o.time_points                 = o_itr_current->time_points;
            o.ints                        = o_itr_current->ints;
            o.ints["prior_id"]            = o_itr_current->id;  
            o.trxs                        = o_itr_current->trxs;
	      });

         if (remove_old) {
            o_t_current.erase (o_itr_current);
         }
      }

      asset adjust_asset (const asset& original_asset, const float& adjustment) {
         return asset { static_cast<int64_t> (original_asset.amount * adjustment), original_asset.symbol };
      }  

      bool is_paused () {
         config_table      config_s (get_self(), get_self().value);
   	   Config c = config_s.get_or_create (get_self(), Config());   
         check (c.ints.find ("paused") != c.ints.end(), "Contract does not have a pause configuration. Assuming it is paused. Please contact administrator.");
         
         uint64_t paused = c.ints.at("paused");
         return paused == 1;
      }
};

#endif