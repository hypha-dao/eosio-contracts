#ifndef HYPHADAC
#define HYPHADAC

#include <eosio/eosio.hpp>
#include <eosio/asset.hpp>
#include <eosio/singleton.hpp>
#include <eosio/multi_index.hpp>

#include "common.hpp"

using namespace eosio;
using std::string;

CONTRACT hyphadac : public contract {
   public:
      using contract::contract;

      hyphadac(name self, name code, datastream<const char*> ds);

      ~hyphadac();

      #pragma region native

      struct permission_level_weight {
         permission_level  permission;
         uint16_t          weight;
      };

      struct key_weight {
         eosio::public_key  key;
         uint16_t           weight;
      };

      struct wait_weight {
         uint32_t           wait_sec;
         uint16_t           weight;
      };

      struct authority {
         uint32_t                              threshold = 0;
         std::vector<key_weight>               keys;
         std::vector<permission_level_weight>  accounts;
         std::vector<wait_weight>              waits;
     };

      #pragma endregion native
      
      struct [[ eosio::table, eosio::contract("hyphadac") ]] Config
      {
         // TODO: should this be a time type instead of uint32_t
         uint32_t       period_length           = 60 * 60 * 24 * 28;
         name           hypha_token_contract    = "hyphatoken"_n;
         name           voice_token_contract    = "hyphatoken"_n;
         name           preseeds_token_contract = "hyphatoken"_n;
      };

      struct [[ eosio::table, eosio::contract("hyphadac") ]] VotingConfig
      {
         // TODO: should this be a time type instead of uint32_t
         name        publisher;
         uint8_t     max_board_seats               = 9; // NOTE: adjustable by board members
         uint8_t     open_seats                    = 9;
         uint64_t    open_election_id              = 0;
         uint32_t    holder_quorum_divisor         = 5;
         uint32_t    board_quorum_divisor          = 2;
         uint32_t    issue_duration                = 360;
         uint32_t    start_delay                   = 1;
         uint32_t    leaderboard_duration          = 360;
         uint32_t    election_frequency            = 360*3;
         uint32_t    last_board_election_time      ;
         bool        is_active_election            = false;

        uint64_t primary_key() const { return publisher.value; }
      };

      struct [[eosio::table, eosio::contract("hyphadac") ]] Period
      {
         // TODO: test uint64_t
         uint64_t         period_id               ;
         time_point       start_date              = current_block_time().to_time_point();
         time_point       end_date                = current_block_time().to_time_point();
      };

      struct [[eosio::table, eosio::contract("hyphadac") ]] Role
      {
         name           role_name               ;
         string         description             ;
         asset          hypha_salary            = asset { 0, common::S_HYPHA };
         asset          preseeds_salary         = asset { 0, common::S_PRESEEDS };
         asset          voice_salary            = asset { 0, common::S_HVOICE };
         time_point     created_date            = current_block_time().to_time_point();
         time_point     updated_date            = current_block_time().to_time_point();

         uint64_t       primary_key()           const { return role_name.value; }
      };

      struct [[eosio::table, eosio::contract("hyphadac") ]] Assignment
      {
         uint64_t       assignment_id           = 0;
         name           assigned_account        ;
         name           role_name               ;
         string         notes                   ;
         time_point     start_date              = current_block_time().to_time_point();
         time_point     end_date                = current_block_time().to_time_point();
         float          time_share              = 0.000000000000000;

         time_point     created_date            = current_block_time().to_time_point();
         time_point     updated_date            = current_block_time().to_time_point();

         uint64_t       primary_key()           const { return assignment_id; }
         uint64_t       by_assigned()           const { return assigned_account.value; }
         uint64_t       by_role()               const { return role_name.value; }
      };

      struct [[eosio::table, eosio::contract("hyphadac") ]] Proposal
      {
         // uint64_t       proposal_id             = 0;
         name           proposer                ; 
         name           proposal_name           ;
         string         notes                   ;
         uint64_t       ballot_id               ;
         transaction    transaction             ;

		   uint64_t primary_key() const { return proposer.value; }
         // uint64_t       primary_key()           const { return proposal_id; }
      };

      struct [[eosio::table, eosio::contract("hyphadac") ]] Contribution
      {
         uint64_t       contribution_id         ;
         name           contributor             ;
         string         description             ;
         asset          hypha_value             = asset { 0, common::S_HYPHA };
         asset          preseeds_value          = asset { 0, common::S_PRESEEDS };
         string         status                  = common::OPEN;
         
         time_point       created_date            = current_block_time().to_time_point();
         time_point       contribution_date       ;
         time_point       approved_date           ;
         time_point       paid_date               ;

         uint64_t         primary_key()           const { return contribution_id; }
      };

      struct [[eosio::table]] Nominee {
        name      nominee;
        uint64_t  primary_key() const { return nominee.value; }
      };

      struct [[eosio::table]] Steward {
         name     member;
         uint64_t primary_key() const { return member.value; }
      };

      typedef multi_index<"roles"_n, Role> role_table;

      typedef multi_index<"assignments"_n, Assignment,
         indexed_by<"byassigned"_n,
            const_mem_fun<Assignment, uint64_t, &Assignment::by_assigned>>,
         indexed_by<"byrole"_n,
            const_mem_fun<Assignment, uint64_t, &Assignment::by_role>>
      > assignment_table;

      typedef multi_index<"contribs"_n, Contribution> contribution_table;
      typedef multi_index<"proposals"_n, Proposal> proposal_table;

      typedef multi_index<"periods"_n, Period> period_table;

      typedef multi_index<"nominees"_n, Nominee> nominee_table;
      typedef multi_index<"stewards"_n, Steward> steward_table;

      typedef singleton<"config"_n, Config> config_table;
      typedef multi_index<"config"_n, Config> config_table_placeholder;
      
      typedef singleton<"vconfig"_n, VotingConfig> voting_config_table;
      typedef multi_index<"vconfig"_n, VotingConfig> voting_config_table_placeholder;
      
      voting_config_table voting_configs;
      VotingConfig _config;

      ACTION reset ();

      ACTION setconfig (const uint32_t period_length, 
                        const name     hypha_token_contract);

      ACTION proposerole (const name proposer,
                           const name role_name,
                           const string info_url,
                           const string description,
                           const asset hypha_salary,
                           const asset preseeds_salary,
                           const asset voice_salary);

      ACTION newrole (  const name role_name, 
                        const string description,
                        const asset hypha_salary,
                        const asset preseeds_salary,
                        const asset voice_salary);

      ACTION updaterole (  const name role_name, 
                           const string description,
                           const asset hypha_salary,
                           const asset preseeds_salary,
                           const asset voice_salary); 

      ACTION propassign (const name       proposer,
                        const name        assigned_account,
                        const name        role_name,
                        const string      info_url,
                        const string      notes,
                        const time_point  start_date,
                        const float       time_share);

      ACTION assign (const name        assigned_account,
                     const name        role_name,
                     const string      info_url,
                     const string      notes,
                     const time_point  start_date,
                     const float       time_share);

      ACTION contribute (  const name        contributor,
                           const string      notes,
                           const asset       hypha_value,
                           const asset       preseeds_value, 
                           const time_point  contribution_date);

      //NOTE: sends inline actions to register and initialize HVOICE token registry
      ACTION inithvoice(const string initial_info_link);

      // NOTE: sends inline actions to register and initialize STEWARD token registry
      ACTION initsteward(const string initial_info_link);

      ACTION nominate(const name nominee, const name nominator);

      ACTION makeissue(const name proposer, 
                        const string info_url,
                        const name proposal_name,
                        const string notes,
                        const transaction trx);

      ACTION closeissue(const name holder, const name proposer);

      ACTION makeelection(const name holder, const string info_url);

      ACTION addcand(name nominee, string info_link);

      ACTION removecand(name candidate);

      ACTION endelection(name holder);

      ACTION removemember(name member_to_remove);

   private:

      #pragma region Helper_Functions
      VotingConfig get_default_config();

      void add_steward(name nominee);

      void rmv_steward(name member);

      void addseats(name member, uint8_t num_seats);

      bool is_steward(name user);

      bool is_nominee(name user);

      bool is_hvoice_holder(name user);

      bool is_steward_holder(name user);

      bool is_term_expired();

      void remove_and_seize_all();

      void remove_and_seize(name member);

      void set_permissions(vector<permission_level_weight> perms);

      uint8_t get_occupied_seats();

      vector<permission_level_weight> perms_from_members();

      #pragma endregion Helper_Functions
};

#endif