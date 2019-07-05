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
         uint64_t       proposal_id             = 0;
         name           proposer                ; 
         name           proposal_name           ;
         string         notes                   ;
         uint64_t       ballot_id               ;
         transaction    transaction             ;
         uint8_t        proposal_type           = 0;
         uint64_t       proposal_fk             ;

		   uint64_t       primary_key() const { return proposal_id; }
         uint64_t       by_proposal_fk() const { return proposal_fk; }
      };

      struct [[eosio::table, eosio::contract("hyphadao") ]] RoleProposal
      {
         uint64_t       proposal_id             = 0;
         name           proposer                ; 
         name           role_name               ;
         string         info_url                ;
         string         description             ;
         asset          hypha_salary            = asset { 0, common::S_HYPHA };
         asset          preseeds_salary         = asset { 0, common::S_PRESEEDS };
         asset          voice_salary            = asset { 0, common::S_HVOICE };
         uint8_t        status                  ;

         time_point     created_date            = current_block_time().to_time_point();
         time_point     executed_date           ;

		   uint64_t primary_key() const { return proposal_id; }
      };

      struct [[eosio::table, eosio::contract("hyphadao") ]] AssignmentProposal
      {
         uint64_t       proposal_id             ;
         name           proposer                ;
         name           assigned_account        ;
         name           role_name               ;
         string         info_url                ;
         string         notes                   ;
         uint64_t       start_period            ;
         float          time_share              = 0.000000000000000;
         uint8_t        status                  ;

         time_point     created_date            = current_block_time().to_time_point();
         time_point     executed_date           ;

         uint64_t       primary_key()           const { return proposal_id; }
         uint64_t       by_assigned()           const { return assigned_account.value; }
         uint64_t       by_role()               const { return role_name.value; }
      };

      struct [[eosio::table, eosio::contract("hyphadao") ]] PayoutProposal
      {
         uint64_t       proposal_id             ;
         name           recipient               ;
         string         notes             ;
         string         info_url                ;
         asset          hypha_value             = asset { 0, common::S_HYPHA };
         asset          preseeds_value          = asset { 0, common::S_PRESEEDS };
         asset          voice_value             = asset { 0, common::S_HVOICE };
         uint8_t        status                  = common::OPEN;
         
         time_point     created_date            = current_block_time().to_time_point();
         time_point     executed_date           ;
         time_point     contribution_date       ;

         uint64_t       primary_key()           const { return proposal_id; }
      };
      
      typedef multi_index<"proposals"_n, Proposal> proposal_table;
      typedef multi_index<"payoutprops"_n, PayoutProposal> payoutprop_table;
      typedef multi_index<"roleprops"_n, RoleProposal> roleprop_table;
      typedef multi_index<"assprops"_n, AssignmentProposal> assprop_table;

      // typedef singleton<"config"_n, DAOConfig> config_table;
      // typedef multi_index<"config"_n, DAOConfig> config_table_placeholder;
      
      ACTION reset ();
      ACTION init ();
      ACTION eraseprop (const uint64_t& proposal_id);
      ACTION setconfig (const name&     hypha_token_contract);

      ACTION proposerole (const name& proposer,
                           const name& role_name,
                           const string& info_url,
                           const string& description,
                           const asset& hypha_salary,
                           const asset& preseeds_salary,
                           const asset& voice_salary);

      ACTION newrole (  const uint64_t& proposal_id);

      // ACTION updaterole (  const name& role_name, 
      //                      const string& description,
      //                      const asset& hypha_salary,
      //                      const asset& preseeds_salary,
      //                      const asset& voice_salary); 

      ACTION propassign (const name&       proposer,
                        const name&        assigned_account,
                        const name&        role_name,
                        const string&      info_url,
                        const string&      notes,
                        const uint64_t&      start_period,
                        const float&       time_share);

      ACTION assign (const uint64_t& 		proposal_id);

      ACTION proppayout (  const name&   	   proposer,
                           const name&       recipient,
                           const string&     notes,
                           const string&     info_url,
                           const asset&      hypha_value,
                           const asset&      preseeds_value, 
						         const asset& 		voice_value,
                           const time_point& contribution_date);

      ACTION makepayout (  const uint64_t&   proposal_id);
      
      ACTION makeprop(const name& proposer, 
                        const string& info_url,
                        const name& proposal_name,
                        const string& notes,
                        const uint8_t& proposal_type,
                        const uint64_t& proposal_fk,
                        const transaction& trx);

      ACTION closeprop(const name& holder, const uint64_t& proposal_id);


      //NOTE: sends inline actions to register and initialize HVOICE token registry
      ACTION inithvoice(const string initial_info_link);

      // NOTE: sends inline actions to register and initialize STEWARD token registry
      ACTION initsteward(const string initial_info_link);

      ACTION nominate(const name& nominee, const name& nominator);
      ACTION makeelection(const name& holder, const string& info_url);
      ACTION addcand(const name& nominee, const string& info_url);
      ACTION removecand(const name& candidate);
      ACTION endelection(const name& holder);
      ACTION removemember(const name& member_to_remove);

   private:
      Board board = Board (get_self());
      Holocracy holocracy = Holocracy (get_self());
      Bank bank = Bank (get_self());

      void qualify_proposer (const name& proposer);
};

#endif