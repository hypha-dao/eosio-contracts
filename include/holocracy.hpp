#ifndef HOLOCRACY_H
#define HOLOCRACY_H

#include <eosio/eosio.hpp>
#include <eosio/asset.hpp>
#include <eosio/singleton.hpp>
#include <eosio/multi_index.hpp>

#include "bank.hpp"

using namespace eosio;
using std::string;

class Holocracy {

    public:

        struct [[eosio::table, eosio::contract("hyphadao") ]] Role
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

        struct [[eosio::table, eosio::contract("hyphadao") ]] Assignment
        {
            uint64_t       assignment_id           = 0;
            name           assigned_account        ;
            name           role_name               ;
            string         notes                   ;
            time_point     start_date              = current_block_time().to_time_point();
            float          time_share              = 0.000000000000000;

            time_point     created_date            = current_block_time().to_time_point();
            time_point     updated_date            = current_block_time().to_time_point();

            uint64_t       primary_key()           const { return assignment_id; }
            uint64_t       by_assigned()           const { return assigned_account.value; }
            uint64_t       by_role()               const { return role_name.value; }
        };

        typedef multi_index<"roles"_n, Role> role_table;

        typedef multi_index<"assignments"_n, Assignment,
            indexed_by<"byassigned"_n,
                const_mem_fun<Assignment, uint64_t, &Assignment::by_assigned>>,
            indexed_by<"byrole"_n,
                const_mem_fun<Assignment, uint64_t, &Assignment::by_role>>
        > assignment_table;

        Holocracy (const name& contract);

        role_table          role_t;
        assignment_table    assignment_t;
        bank                bankClass;

        void reset ();

        void newrole (  const name& role_name, 
                        const string& description,
                        const asset& hypha_salary,
                        const asset& preseeds_salary,
                        const asset& voice_salary);


        void newassign (const name&        assigned_account,
                        const name&        role_name,
                        const string&      info_url,
                        const string&      notes,
                        const float&       time_share);

        void delrole    (const name&    role_name);
        void delassign  (const uint64_t&     assignment_id);

        void payassign (const uint64_t& assignment_id, const uint64_t& period_id);

}