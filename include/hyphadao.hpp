#ifndef HYPHADAO_H
#define HYPHADAO_H

#include <eosio/eosio.hpp>
#include <eosio/asset.hpp>
#include <eosio/singleton.hpp>
#include <eosio/multi_index.hpp>

#include "common.hpp"
#include "board.hpp"
#include "holocracy.hpp"

using namespace eosio;
using std::string;

CONTRACT hyphadao : public contract {
   public:
      using contract::contract;

      struct [[eosio::table, eosio::contract("hyphadao") ]] Proposal
      {
         // uint64_t       proposal_id             = 0;
         name           proposer                ; 
         name           proposal_name           ;
         string         notes                   ;
         uint64_t       ballot_id               ;
         transaction    transaction             ;
         uint8_t        proposal_type           = 0;
         uint64_t       proposal_fk             ;

		   uint64_t       primary_key() const { return proposer.value; }
         uint64_t       by_proposal_fk() const { return proposal_fk; }
      };

      struct [[eosio::table, eosio::contract("hyphadao") ]] RoleProposal
      {
         uint64_t       proposal_id             = 0;
         name           proposer                ; 
         name           role_name               ;
         string         description             ;
         asset          hypha_salary            = asset { 0, common::S_HYPHA };
         asset          preseeds_salary         = asset { 0, common::S_PRESEEDS };
         asset          voice_salary            = asset { 0, common::S_HVOICE };
         uint8_t        status                  ;

         time_point     created_date            = current_block_time().to_time_point();
         time_point     approved_date           ;

		   uint64_t primary_key() const { return proposer.value; }
      };

      struct [[eosio::table, eosio::contract("hyphadao") ]] AssignmentProposal
      {
         uint64_t       assignment_id           = 0;
         name           assigned_account        ;
         name           role_name               ;
         string         notes                   ;
         time_point     start_date              = current_block_time().to_time_point();
         float          time_share              = 0.000000000000000;
         uint8_t        status                  ;

         time_point     created_date            = current_block_time().to_time_point();
         time_point     approved_date           = current_block_time().to_time_point();

         uint64_t       primary_key()           const { return assignment_id; }
         uint64_t       by_assigned()           const { return assigned_account.value; }
         uint64_t       by_role()               const { return role_name.value; }
      };

      struct [[eosio::table, eosio::contract("hyphadao") ]] PayoutProposal
      {
         uint64_t       contribution_id         ;
         name           contributor             ;
         string         description             ;
         asset          hypha_value             = asset { 0, common::S_HYPHA };
         asset          preseeds_value          = asset { 0, common::S_PRESEEDS };
         uint8_t        status                  = common::OPEN;
         
         time_point     created_date            = current_block_time().to_time_point();
         time_point     contribution_date       ;
         time_point     approved_date           ;
         
         uint64_t       primary_key()           const { return contribution_id; }
      };

      typedef multi_index<"contribs"_n, Contribution> contribution_table;
      typedef multi_index<"proposals"_n, Proposal> proposal_table;
      typedef singleton<"config"_n, DAOConfig> config_table;
      typedef multi_index<"config"_n, DAOConfig> config_table_placeholder;
      
      ACTION reset ();

      ACTION setconfig (const uint32_t& period_length, 
                        const name*     hypha_token_contract);

      ACTION proposerole (const name& proposer,
                           const name& role_name,
                           const string& info_url,
                           const string& description,
                           const asset& hypha_salary,
                           const asset& preseeds_salary,
                           const asset& voice_salary);

      ACTION newrole (  const name& role_name, 
                        const string& description,
                        const asset& hypha_salary,
                        const asset& preseeds_salary,
                        const asset& voice_salary);

      ACTION updaterole (  const name& role_name, 
                           const string& description,
                           const asset& hypha_salary,
                           const asset& preseeds_salary,
                           const asset& voice_salary); 

      ACTION propassign (const name&       proposer,
                        const name&        assigned_account,
                        const name&        role_name,
                        const string&      info_url,
                        const string&      notes,
                        const time_point&  start_date,
                        const float&       time_share);

      ACTION assign (const name&        assigned_account,
                     const name&        role_name,
                     const string&      info_url,
                     const string&      notes,
                     const time_point&  start_date,
                     const float&       time_share);

      ACTION propcontrib ( const name&        proposer,
                           const name&        contributor,
                           const string&      notes,
                           const asset&       hypha_value,
                           const asset&       preseeds_value, 
                           const time_point&  contribution_date);

      ACTION paycontrib (  const name&        contributor,
                           const string&      notes,
                           const asset&       hypha_value,
                           const asset&       preseeds_value, 
                           const time_point&  contribution_date);

      
      ACTION makeissue(const name proposer, 
                        const string info_url,
                        const name proposal_name,
                        const string notes,
                        const transaction trx);

      ACTION closeissue(const name holder, const name proposer);


      //NOTE: sends inline actions to register and initialize HVOICE token registry
      ACTION inithvoice(const string initial_info_link);

      // NOTE: sends inline actions to register and initialize STEWARD token registry
      ACTION initsteward(const string initial_info_link);

      ACTION nominate(const name& nominee, const name& nominator);
      ACTION makeelection(const name& holder, const string& info_url);
      ACTION addcand(const name& nominee, const string& info_link);
      ACTION removecand(const name& candidate);
      ACTION endelection(const name& holder);
      ACTION removemember(const name& member_to_remove);

   private:

      Board board = Board (get_self());
      Holocracy holocracy = Holocracy (get_self());
      Bank bank = Bank (get_self());
};

#endif