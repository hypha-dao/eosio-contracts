#pragma once
#include <eosio/eosio.hpp>
#include <eosio/asset.hpp>
#include <eosio/singleton.hpp>
#include <eosio/multi_index.hpp>
#include <eosio/transaction.hpp>
#include <eosio/name.hpp>

#include <common.hpp>
#include <trail.hpp>
#include <document_graph/content_group.hpp>
#include <document_graph/document_graph.hpp>
/**
 * We don't need to declare this on hpp since we are not using it anywhere here, it
 * should go into cpp
#include <proposals/proposal_factory.hpp>
#include <proposals/proposal.hpp>
*/
using namespace eosio;
using namespace std;

namespace hypha
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

      struct [[eosio::table, eosio::contract("hyphadao")]] price_history_table
      {
         uint64_t id;
         asset seeds_usd;
         time_point date;

         uint64_t primary_key() const { return id; }
      };
      typedef eosio::multi_index<name("pricehistory"), price_history_table> price_history_tables;

      struct [[eosio::table, eosio::contract("hyphadao")]] Period
      {
         uint64_t period_id;
         time_point start_date;
         time_point end_date;
         string phase;

         uint64_t primary_key() const { return period_id; }
      };

      struct [[eosio::table, eosio::contract("hyphadao")]] Payment
      {
         uint64_t payment_id;
         time_point payment_date;
         uint64_t period_id = 0;
         uint64_t assignment_id = -1;
         name recipient;
         asset amount;
         string memo;

         uint64_t primary_key() const { return payment_id; }
         uint64_t by_period() const { return period_id; }
         uint64_t by_recipient() const { return recipient.value; }
         uint64_t by_assignment() const { return assignment_id; }
      };

      typedef multi_index<name("periods"), Period> period_table;

      typedef multi_index<name("payments"), Payment,
                          indexed_by<name("byperiod"), const_mem_fun<Payment, uint64_t, &Payment::by_period>>,
                          indexed_by<name("byrecipient"), const_mem_fun<Payment, uint64_t, &Payment::by_recipient>>,
                          indexed_by<name("byassignment"), const_mem_fun<Payment, uint64_t, &Payment::by_assignment>>>
          payment_table;

      struct asset_batch
      {
         asset hypha = asset{0, common::S_HYPHA};
         asset d_seeds = asset{0, common::S_SEEDS};
         asset seeds = asset{0, common::S_SEEDS};
         asset voice = asset{0, common::S_HVOICE};
         asset husd = asset{0, common::S_HUSD};
      };

      const uint64_t MICROSECONDS_PER_HOUR = (uint64_t)60 * (uint64_t)60 * (uint64_t)1000000;
      const uint64_t MICROSECONDS_PER_YEAR = MICROSECONDS_PER_HOUR * (uint64_t)24 * (uint64_t)365;

      /// **********************************
      ///  Generation 1 Actions
      /// **********************************

      // ACTION create(const name &scope,
      //               map<string, name> names,
      //               map<string, string> strings,
      //               map<string, asset> assets,
      //               map<string, time_point> time_points,
      //               map<string, uint64_t> ints,
      //               const map<string, float> floats,
      //               map<string, transaction> trxs);

      // ACTION edit(const name &scope,
      //             const uint64_t &id,
      //             const map<string, name> names,
      //             map<string, string> strings,
      //             map<string, asset> assets,
      //             map<string, time_point> time_points,
      //             const map<string, uint64_t> ints,
      //             const map<string, float> floats,
      //             map<string, transaction> trxs);

      ACTION setconfig(const map<string, name> names,
                       const map<string, string> strings,
                       const map<string, asset> assets,
                       const map<string, time_point> time_points,
                       const map<string, uint64_t> ints,
                       const map<string, float> floats,
                       const map<string, transaction> trxs);
      ACTION setconfigatt(const string &key, const hyphadao::flexvalue1 &value);
      ACTION remconfigatt(const string &key);
      ACTION setlastballt(const name &last_ballot_id);
      ACTION togglepause();

      // These actions are executed only on approval of a proposal.
      // To introduce a new proposal type, we would add another action to the below.
      ACTION newrole(const uint64_t &proposal_id);
      ACTION assign(const uint64_t &proposal_id);
      ACTION makepayout(const uint64_t &proposal_id);
      ACTION suspend(const uint64_t &proposal_id);

      // anyone can call closeprop, it executes the transaction if the voting passed
      ACTION closeprop(const uint64_t &proposal_id);

      // ACTION copytodraft(const name &copier, const name &scope, const uint64_t &id);
      // ACTION propdraft(const uint64_t &id);
      // ACTION erasedraft(const uint64_t &id);
      // ACTION recreate(const name &scope, const uint64_t &id);

      ACTION propsuspend(const name &proposer, const name &scope, const uint64_t &id);
      ACTION withdraw(const name &withdrawer, const uint64_t &assignment_id, const string &notes);

      ACTION removemember(const name &member_to_remove);
      ACTION addmember(const name &member);

      // data object handling
      ACTION transform(const name &creator, const name &scope, const uint64_t &id);
      ACTION transscope(const name &creator, const name &scope, const uint64_t &starting_id, const uint64_t &batch_size);
      ACTION resetscope(const name &scope);
      ACTION erasedoc(const name &scope, const uint64_t &id);
      ACTION changescope(const name &scope, const uint64_t &id, const vector<name> &new_scopes, const bool &remove_old);

      // migration related actions
      // ACTION backupobjs (const name& scope);
      // ACTION erasebackups (const name& scope);
      // ACTION restoreobjs (const name& scope);
      /// **********************************
      ///  END - Generation 1 Actions
      /// **********************************

      /// **********************************
      ///  BEGIN - Generation 2 Actions
      /// **********************************

      ACTION propose(const name &proposer, const name &proposal_type, ContentGroups &content_groups);
      ACTION closedocprop(const checksum256 &proposal_hash);

      // ADMIN
      // ACTION erasedochash(const checksum256 &doc);
      // ACTION erasedocs(const uint64_t &begin_id, const uint64_t &batch_size);
      // ACTION erasealldocs(const string &notes);
      // ACTION eraseedges(const string &notes);
      // ACTION erasedocbyid(const uint64_t &id);
      // create the initial rootnode document
      ACTION createroot(const string &notes);
      // make member documents from the members table
      ACTION makememdocs(const string &notes);
      // ACTION createdoc(const name &creator, const vector<ContentGroup> &content_groups);

      /// **********************************
      ///  END - Generation 2 Actions
      /// **********************************

      // membership actions
      ACTION apply(const name &applicant, const string &content);
      ACTION enroll(const name &enroller, const name &applicant, const string &content);
      ACTION remapply(const name &applicant);

      // Admin
      // ACTION reset ();
      ACTION debugmsg(const string &message);
      ACTION clrdebugs(const uint64_t &starting_id, const uint64_t &batch_size);
      ACTION updversion(const string &component, const string &version);
      // ACTION updassets(const uint64_t &proposal_id);
      ACTION set(const name &scope, const uint64_t &id, const string &key, const flexvalue1 &flexvalue);

      // alerts Group
      ACTION setalert(const name &level, const string &content);
      ACTION remalert(const string &notes);

      // Calendar actions
      ACTION addperiod(const time_point &start_time, const time_point &end_time, const string &phase);

      // users can claim their salary pay
      ACTION payassign(const checksum256 &assignment_hash, const uint64_t &period_id);

      DocumentGraph m_documentGraph = DocumentGraph(get_self());
      
      static checksum256 get_root(const name &contract);
      // static void verify_membership(const name &member);

      static asset adjust_asset(const asset &original_asset, const float &adjustment);
      static string get_string(const std::map<string, string> strings, string key);

   private:
      // bank-related functions
      // void remove_periods(const uint64_t &begin_period_id, const uint64_t &end_period_id);
      void make_payment(const uint64_t &period_id, const name &recipient, const asset &quantity, const string &memo, const uint64_t &assignment_id, const uint64_t &bypass_escrow);
      void issuetoken(const name &token_contract, const name &issuer, const name &to, const asset &token_amount, const string &memo);

      uint64_t get_last_period_id();

      float get_seeds_price_usd();
      float get_seeds_price_usd(const time_point &price_time_point);
      asset get_seeds_amount(const asset &usd_amount, const time_point &price_time_point, const float &time_share, const float &deferred_perc);

      // Generation 2 - document graph related
      Document create_votetally_doc(const name &proposer, ContentGroups &content_groups);
      // bool did_pass(const name &ballot_id);

      // void check_coefficient(ContentGroup & content_group, const string &coefficient_key);
      asset apply_coefficient(const Document &badge, const asset &base, const string &coefficient_key);
      asset_batch apply_badge_coefficients(const uint64_t period_id, const name &member, const asset_batch ab);
      vector<Document> get_current_badges(const uint64_t &period_id, const name &member);

      checksum256 get_root();

      void qualify_owner(const name &proposer);

      // config related
      float get_float(const std::map<string, uint64_t> ints, string key);
      bool is_paused();
      uint64_t get_next_sender_id();

      // Utilities
      uint64_t hash(std::string str);
      void debug(const string &notes);
      void debugx(const string &message);
      void check_capacity(const uint64_t &role_id, const uint64_t &req_time_share_x100);

      // void new_object(const name &creator,
      //                 const name &scope,
      //                 const map<string, name> names,
      //                 const map<string, string> strings,
      //                 const map<string, asset> assets,
      //                 const map<string, time_point> time_points,
      //                 const map<string, uint64_t> ints,
      //                 const map<string, transaction> trxs);

      void new_document(const name &creator,
                        const name &scope,
                        const map<string, name> names,
                        const map<string, string> strings,
                        const map<string, asset> assets,
                        const map<string, time_point> time_points,
                        const map<string, uint64_t> ints);

      // void new_proposal(const name &owner,
      //                   map<string, name> &names,
      //                   map<string, string> &strings,
      //                   map<string, asset> &assets,
      //                   map<string, time_point> &time_points,
      //                   map<string, uint64_t> &ints,
      //                   map<string, transaction> &trxs);

      // void merge(const name &scope,
      //            const uint64_t &id,
      //            const map<string, name> names,
      //            const map<string, string> strings,
      //            const map<string, asset> assets,
      //            const map<string, time_point> time_points,
      //            const map<string, uint64_t> ints,
      //            const map<string, transaction> trxs);

      void event(const name &level, const map<string, flexvalue1> &values);

        };
} // namespace hypha