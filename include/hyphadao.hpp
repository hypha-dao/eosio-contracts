#pragma once
#include <eosio/eosio.hpp>
#include <eosio/asset.hpp>
#include <eosio/singleton.hpp>
#include <eosio/multi_index.hpp>
#include <eosio/transaction.hpp>

#include <bank.hpp>
#include <common.hpp>
#include <trail.hpp>
#include <document_graph.hpp>

using namespace eosio;
using namespace std;

namespace hyphaspace
{
   CONTRACT hyphadao : public contract
   {
   public:
      hyphadao(name self, name code, datastream<const char *> ds);
      ~hyphadao();

      // v2 data structure will use variants for more generic support
      typedef std::variant<name, string, asset, time_point, uint64_t> flexvalue1;

      struct [[eosio::table, eosio::contract("hyphadao")]] Config
      {
         // required configurations:
         // names : telos_decide_contract, hypha_token_contract, seeds_token_contract, last_ballot_id
         // ints  : voting_duration_sec
         map<string, name> names;
         map<string, string> strings;
         map<string, asset> assets;
         map<string, time_point> time_points;
         map<string, uint64_t> ints;
         map<string, transaction> trxs;
         map<string, float> floats;
      };

      typedef singleton<name("config"), Config> config_table;
      typedef multi_index<name("config"), Config> config_table_placeholder;

      struct [[eosio::table, eosio::contract("hyphadao")]] Member
      {
         name member;
         uint64_t primary_key() const { return member.value; }
      };

      typedef multi_index<name("members"), Member> member_table;

      struct [[eosio::table, eosio::contract("hyphadao")]] Applicant
      {
         name applicant;
         string content;

         time_point created_date = current_time_point();
         time_point updated_date = current_time_point();

         uint64_t primary_key() const { return applicant.value; }
      };
      typedef multi_index<name("applicants"), Applicant> applicant_table;

      struct [[eosio::table, eosio::contract("hyphadao")]] AssignmentPayout
      {
         uint64_t ass_payment_id;
         uint64_t assignment_id;
         name recipient;
         uint64_t period_id;
         vector<asset> payments;
         time_point payment_date;

         uint64_t primary_key() const { return ass_payment_id; }
         uint64_t by_assignment() const { return assignment_id; }
         uint64_t by_period() const { return period_id; }
         uint64_t by_recipient() const { return recipient.value; }
      };

      typedef multi_index<name("asspayouts"), AssignmentPayout,
                          indexed_by<name("byassignment"),
                                     const_mem_fun<AssignmentPayout, uint64_t, &AssignmentPayout::by_assignment>>,
                          indexed_by<name("byperiod"),
                                     const_mem_fun<AssignmentPayout, uint64_t, &AssignmentPayout::by_period>>,
                          indexed_by<name("byrecipient"),
                                     const_mem_fun<AssignmentPayout, uint64_t, &AssignmentPayout::by_recipient>>>
          asspay_table;

      // scope: proposal, proparchive, role, assignment
      struct [[eosio::table, eosio::contract("hyphadao")]] Object
      {
         uint64_t id;

         // core maps
         map<string, name> names;
         map<string, string> strings;
         map<string, asset> assets;
         map<string, time_point> time_points;
         map<string, uint64_t> ints;
         map<string, transaction> trxs;
         map<string, float> floats;
         uint64_t primary_key() const { return id; }

         // indexes
         uint64_t by_owner() const { return names.at("owner").value; }
         uint64_t by_type() const { return names.at("type").value; }
         uint64_t by_fk() const { return ints.at("fk"); }

         // timestamps
         time_point created_date = current_time_point();
         time_point updated_date = current_time_point();
         uint64_t by_created() const { return created_date.sec_since_epoch(); }
         uint64_t by_updated() const { return updated_date.sec_since_epoch(); }
      };

      typedef multi_index<name("objects"), Object,                                                             // index 1
                          indexed_by<name("bycreated"), const_mem_fun<Object, uint64_t, &Object::by_created>>, // index 2
                          indexed_by<name("byupdated"), const_mem_fun<Object, uint64_t, &Object::by_updated>>, // 3
                          indexed_by<name("byowner"), const_mem_fun<Object, uint64_t, &Object::by_owner>>,     // 4
                          indexed_by<name("bytype"), const_mem_fun<Object, uint64_t, &Object::by_type>>,       // 5
                          indexed_by<name("byfk"), const_mem_fun<Object, uint64_t, &Object::by_fk>>            // 6
                          >
          object_table;

      struct [[eosio::table, eosio::contract("hyphadao")]] Debug
      {
         uint64_t debug_id;
         string notes;
         time_point created_date = current_time_point();
         uint64_t primary_key() const { return debug_id; }
      };

      typedef multi_index<name("debugs"), Debug> debug_table;

      struct [[eosio::table, eosio::contract("hyphadao")]] configtable
      {
         asset seeds_per_usd;
         asset tlos_per_usd;
         asset citizen_limit;
         asset resident_limit;
         asset visitor_limit;
         uint64_t timestamp;
      };
      typedef singleton<name("config"), configtable> configtables;
      typedef eosio::multi_index<name("config"), configtable> dump_for_config;

      struct [[eosio::table, eosio::contract("hyphadao")]] price_history_table {
         uint64_t id;
         asset seeds_usd;
         time_point date;

         uint64_t primary_key()const { return id; }
      };
      typedef eosio::multi_index<name("pricehistory"), price_history_table> price_history_tables;

      // struct [[eosio::table, eosio::contract("hyphadao")]] document
      // {
      //    uint64_t id;
      //    checksum256 hash;
      //    name creator;
      //    vector<document_graph::content_group> content_groups;

      //    vector<document_graph::certificate> certificates;
      //    uint64_t primary_key() const { return id; }
      //    uint64_t by_creator() const { return creator.value; }
      //    checksum256 by_hash() const { return hash; }

      //    time_point created_date = current_time_point();
      //    uint64_t by_created() const { return created_date.sec_since_epoch(); }

      //    EOSLIB_SERIALIZE(document, (id)(hash)(creator)(content_groups)(certificates)(created_date))
      // };

      typedef multi_index<name("documents"), document_graph::document,
                          indexed_by<name("idhash"), const_mem_fun<document_graph::document, checksum256, &document_graph::document::by_hash>>,
                          indexed_by<name("bycreator"), const_mem_fun<document_graph::document, uint64_t, &document_graph::document::by_creator>>,
                          indexed_by<name("bycreated"), const_mem_fun<document_graph::document, uint64_t, &document_graph::document::by_created>>>
          document_table;

      // scopes: badges, roles, assignments, members, specific member name, etc. 
      struct [[eosio::table, eosio::contract("hyphadao")]] DocumentIndex
      {
         uint64_t id;

         checksum256 document_hash;
         checksum256 by_hash() const { return document_hash; }

         time_point created_date = current_time_point();
         uint64_t by_created() const { return created_date.sec_since_epoch(); }

         uint64_t primary_key() const { return id; }
      };

      typedef multi_index<name("docindex"),DocumentIndex,
                              indexed_by<name("idhash"), const_mem_fun<DocumentIndex, checksum256, &DocumentIndex::by_hash>>,
                              indexed_by<name("bycreated"), const_mem_fun<DocumentIndex, uint64_t, &DocumentIndex::by_created>>>
               docindex_table;

      typedef multi_index<name("edges"), document_graph::edge,
                              indexed_by<name("fromnode"), const_mem_fun<document_graph::edge, checksum256, &document_graph::edge::by_from>>,
                              indexed_by<name("tonode"), const_mem_fun<document_graph::edge, checksum256, &document_graph::edge::by_to>>,
                              indexed_by<name("edgename"), const_mem_fun<document_graph::edge, uint64_t, &document_graph::edge::by_edge_name>>,
                              indexed_by<name("bycreated"), const_mem_fun<document_graph::edge, uint64_t, &document_graph::edge::by_created>>>
               edge_table;

      const uint64_t MICROSECONDS_PER_HOUR = (uint64_t)60 * (uint64_t)60 * (uint64_t)1000000;
      const uint64_t MICROSECONDS_PER_YEAR = MICROSECONDS_PER_HOUR * (uint64_t)24 * (uint64_t)365;

      ACTION create(const name &scope,
                     map<string, name> names,
                     map<string, string> strings,
                     map<string, asset> assets,
                     map<string, time_point> time_points,
                     map<string, uint64_t> ints,
                     const map<string, float> floats,
                     map<string, transaction> trxs);
      
      ACTION propose (const name& proposer, const name& proposal_type, std::vector<document_graph::content_group> &content_groups);

      ACTION created(const name &creator, const checksum256 &hash);

      ACTION edit(const name &scope,
                  const uint64_t &id,
                  const map<string, name> names,
                  map<string, string> strings,
                  map<string, asset> assets,
                  map<string, time_point> time_points,
                  const map<string, uint64_t> ints,
                  const map<string, float> floats,
                  map<string, transaction> trxs);
      
      ACTION copytodraft (const name& copier, const name &scope, const uint64_t &id);
      ACTION propdraft (const uint64_t& id);
      ACTION erasedraft (const uint64_t& id);

      ACTION propsuspend (const name &proposer, const name &scope, const uint64_t &id);
      ACTION withdraw (const name &withdrawer, const uint64_t &assignment_id, const string& notes);
      
      ACTION apply(const name &applicant, const string &content);

      ACTION enroll(const name &enroller,
                    const name &applicant,
                    const string &content);

      // Admin
      // ACTION reset ();
      ACTION resetperiods();
      ACTION resetscope(const name &scope);
      ACTION erasedoc(const name &scope,
                      const uint64_t &id);
      ACTION togglepause();
    
      ACTION recreate(const name &scope, const uint64_t &id);
      ACTION debugmsg(const string &message);
      ACTION updversion(const string &component, const string &version);
      ACTION updassets(const uint64_t &proposal_id);
      ACTION fixseedsprec (const uint64_t &proposal_id);
      ACTION changescope(const name &scope, const uint64_t &id, const vector<name> &new_scopes, const bool &remove_old);
      ACTION set (const name &scope, const uint64_t &id, const string& key, const flexvalue1& flexvalue);
      ACTION updassassets (const uint64_t &assignment_id);

      ACTION createdoc (const name& creator, const vector<document_graph::content_group> &content_groups);
      ACTION transform(const name &creator, const name &scope, const uint64_t &id);
      ACTION transscope (const name& creator, const name &scope, const uint64_t &starting_id, const uint64_t &batch_size);
      ACTION erasedocs (const name &scope);

      // document_graph
      ACTION erasedochash (const checksum256 &doc);
      ACTION erasealldocs (const string &notes);
      ACTION eraseedges (const string &notes); 

      
      // alerts Group
      ACTION setalert (const name &level, const string &content);
      ACTION remalert (const string &notes);

      // ACTION backupobjs (const name& scope);
      // ACTION erasebackups (const name& scope);
      // ACTION restoreobjs (const name& scope);

      ACTION setconfig(const map<string, name> names,
                       const map<string, string> strings,
                       const map<string, asset> assets,
                       const map<string, time_point> time_points,
                       const map<string, uint64_t> ints,
                       const map<string, float> floats,
                       const map<string, transaction> trxs);

      ACTION setconfigatt(const string& key, const hyphadao::flexvalue1& value);
      ACTION remconfigatt(const string& key);

      ACTION setlastballt(const name &last_ballot_id);

      ACTION clrdebugs(const uint64_t &starting_id, const uint64_t &batch_size);

      ACTION addperiod(const time_point &start_time,
                       const time_point &end_time,
                       const string &phase);
      ACTION remperiods(const uint64_t &begin_period_id,
                        const uint64_t &end_period_id);

      ACTION remapply(const name &applicant);

      // These actions are executed only on approval of a proposal.
      // To introduce a new proposal type, we would add another action to the below.
      ACTION newrole(const uint64_t &proposal_id);
      ACTION assign(const uint64_t &proposal_id);
      ACTION makepayout(const uint64_t &proposal_id);
      ACTION exectrx(const uint64_t &proposal_id);
      ACTION mergeobject(const uint64_t& proposal_id);
      ACTION suspend(const uint64_t &proposal_id);

      // anyone can call closeprop, it executes the transaction if the voting passed
      ACTION closeprop(const uint64_t &proposal_id);
      ACTION closedocprop (const checksum256 &proposal_hash);

      // users can claim their salary pay
      ACTION payassign(const uint64_t &assignment_id, const uint64_t &period_id);

      // temporary hack (?) - keep a list of the members, although true membership is governed by token holdings
      ACTION removemember(const name &member_to_remove);
      ACTION addmember(const name &member);

      // create the initial rootnode document
      ACTION createroot (const string &notes);
      ACTION makememdocs (const string &notes);

   private:
      Bank bank = Bank(get_self());
      document_graph _document_graph = document_graph(get_self());

      checksum256 get_root();

      void defcloseprop(const uint64_t &proposal_id);
      void qualify_owner(const name &proposer);

      name register_ballot(const name &proposer,
							      const map<string, string> &strings);

      name register_ballot(const name &proposer,
                           const string &title, const string &description, const string &content);

      bool did_pass (const name &ballot_id);

      void check_coefficient (document_graph::content_group &content_group, const string &coefficient_key);

      // badge-related functions
      document_graph::document propose_badge (const name& proposer, std::vector<document_graph::content_group> &content_groups);
      document_graph::document propose_badge_assignment (const name& proposer, std::vector<document_graph::content_group> &content_groups);
      void assign_badge (const document_graph::document &badge, const name &assignee);

      document_graph::content_group create_system_group (const name& proposer, 
                                                         const name& proposal_type, 
                                                         const string& decide_title,
                                                         const string& decide_desc,
                                                         const string& decide_content);

      document_graph::document get_member_doc (const name& member);
      document_graph::document get_member_doc (const name &creator, const name& member);

      uint64_t hash (std::string str); 
      uint64_t get_next_sender_id();
      void debug(const string &notes);
      // void change_scope(const name &current_scope, const uint64_t &id, const vector<name> &new_scopes, const bool &remove_old);
     
      asset adjust_asset(const asset &original_asset, const float &adjustment);
      float get_float(const std::map<string, uint64_t> ints, string key);

      bool is_paused();
      bool is_proposal_direct_assets (const map<string, asset> &assets);

      string get_string(const std::map<string, string> strings, string key);
      void checkx(const bool &condition, const string &message);
      void check_capacity(const uint64_t &role_id, const uint64_t &req_time_share_x100);

      uint64_t get_last_period_id();

      void new_object (const name &creator,
                        const name &scope,
                        const map<string, name> names,
                        const map<string, string> strings,
                        const map<string, asset> assets,
                        const map<string, time_point> time_points,
                        const map<string, uint64_t> ints,
                        const map<string, transaction> trxs);

      void new_document (const name &creator,
                        const name &scope, 
                        const map<string, name> names,
                        const map<string, string> strings,
                        const map<string, asset> assets,
                        const map<string, time_point> time_points,
                        const map<string, uint64_t> ints);

      void new_proposal(const name& owner,
                        map<string, name> &names,
                        map<string, string> &strings,
                        map<string, asset> &assets,
                        map<string, time_point> &time_points,
                        map<string, uint64_t> &ints,
                        map<string, transaction> &trxs) ;

      void merge (const name& scope, 
                  const uint64_t& id, 
                  const map<string, name> names,
                  const map<string, string> strings,
                  const map<string, asset> assets,
                  const map<string, time_point> time_points,
                  const map<string, uint64_t> ints,
                  const map<string, transaction> trxs);

      void event (const name &level,
                  const map<string, flexvalue1> &values);

      map<string, flexvalue1> variant_helper (const map<string, name> &names,
                            const map<string, string> &strings,
                            const map<string, asset> &assets,
                            const map<string, time_point> &time_points,
                            const map<string, uint64_t> &ints );

      map<string, asset> get_assets(const asset &usd_amount,
                                    const float &deferred_perc,
                                    const time_point &price_time_point);
                                 
      map<string, asset> get_assets(const uint64_t &role_id, 
                                    const float &deferred_perc, 
                                    const float &time_share_perc);

      float get_seeds_price_usd () ;
      float get_seeds_price_usd (const time_point& price_time_point);
      asset get_seeds_amount (const asset &usd_amount, const time_point &price_time_point, const float &time_share, const float &deferred_perc);
   };
} // namespace hyphasapce