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

      ACTION reset ();

      ACTION setconfig (const uint32_t period_length, 
                        const name     hypha_token_contract);

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

      ACTION assign (const name        assigned_account,
                     const name        role_name,
                     const time_point  start_date,
                     const float       time_share);

      ACTION contribute (  const name        contributor,
                           const string      description,
                           const asset       hypha_value,
                           const asset       preseeds_value, 
                           const time_point  contribution_date);


      struct [[ eosio::table, eosio::contract("hyphadac") ]] Config
      {
         // TODO: should this be a time type instead of uint32_t
         uint32_t       period_length           = 60 * 60 * 24 * 28;
         name           hypha_token_contract    = "hyphatoken"_n;
         name           voice_token_contract    = "hyphatoken"_n;
         name           preseeds_token_contract = "hyphatoken"_n;
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
         asset          voice_salary            = asset { 0, common::S_VOICE };
         time_point     created_date            = current_block_time().to_time_point();
         time_point     updated_date            = current_block_time().to_time_point();

         uint64_t       primary_key()           const { return role_name.value; }
      };

      struct [[eosio::table, eosio::contract("hyphadac") ]] Assignment
      {
         uint64_t       assignment_id           = 0;
         name           assigned_account        ;
         name           role_name               ;
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
         uint64_t       proposal_id             = 0;
         uint64_t       primary_key()           const { return proposal_id; }
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

      typedef multi_index<"roles"_n, Role> role_table;

      typedef multi_index<"assignments"_n, Assignment,
         indexed_by<"byassigned"_n,
            const_mem_fun<Assignment, uint64_t, &Assignment::by_assigned>>,
         indexed_by<"byrole"_n,
            const_mem_fun<Assignment, uint64_t, &Assignment::by_role>>
      > assignment_table;

      typedef multi_index<"contribs"_n, Contribution> contribution_table;
      typedef multi_index<"proposals"_n, Proposal> proposal_table;
      typedef singleton<"config"_n, Config> config_table;
      typedef multi_index<"config"_n, Config> config_table_placeholder;
      typedef multi_index<"periods"_n, Period> period_table;

   private:
};

#endif