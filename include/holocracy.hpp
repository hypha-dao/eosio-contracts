#ifndef HOLOCRACY_H
#define HOLOCRACY_H

#include <eosio/eosio.hpp>
#include <eosio/asset.hpp>
#include <eosio/singleton.hpp>
#include <eosio/multi_index.hpp>

#include "bank.hpp"
#include "common.hpp"

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
            string          info_url                ;
            uint64_t        start_period            ;
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

        Holocracy (const name& contract) :
            role_t (contract, contract.value), 
            contract (contract),
            assignment_t (contract, contract.value),
            bank (contract) {}

        role_table          role_t;
        assignment_table    assignment_t;
        Bank        bank;
        name        contract;

    //     void reset ();

    //     void newrole (  const name& role_name, 
    //                     const string& description,
    //                     const asset& hypha_salary,
    //                     const asset& preseeds_salary,
    //                     const asset& voice_salary);


    //     void newassign (const name&        assigned_account,
    //                     const name&        role_name,
    //                     const string&      info_url,
    //                     const string&      notes,
    //                     const uint64_t&     period_id,
    //                     const float&       time_share);

    //     void delrole    (const name&    role_name);
    //     void delassign  (const uint64_t&     assignment_id);

    //     void payassign (const uint64_t& assignment_id, const uint64_t& period_id);
    

    // private:
        

    //     asset adjust_asset (const asset& original_asset, const float& adjustment);


    void reset () {

        require_auth (contract);
        
        auto r_itr = role_t.begin();
        while (r_itr != role_t.end()) {
            r_itr = role_t.erase (r_itr);
        }

        auto a_itr = assignment_t.begin();
        while (a_itr != assignment_t.end()) {
            a_itr = assignment_t.erase (a_itr);
        }
    }

    void newrole (  const name& role_name, 
                                const string& description,
                                const asset& hypha_salary,
                                const asset& preseeds_salary,
                                const asset& voice_salary) {

        require_auth (contract);

        check (role_t.find (role_name.value) == role_t.end(), "Role name already exists: " + role_name.to_string());
        role_t.emplace (contract, [&](auto &r) {
            r.role_name     = role_name;
            r.description   = description;
            r.hypha_salary  = hypha_salary;
            r.preseeds_salary   = preseeds_salary;
            r.voice_salary      = voice_salary;
            r.created_date      = current_block_time().to_time_point();
        });
    }

    void delrole (const name& role_name) {

        require_auth (contract);

        auto ass_by_role = assignment_t.get_index<"byrole"_n>();
        check (ass_by_role.find(role_name.value) == ass_by_role.end(), "Delete role failed. Role has existing assignments: " + role_name.to_string());

        auto r_itr = role_t.find (role_name.value);
        check (r_itr != role_t.end(), "Delete role failed. Role does not exist: " + role_name.to_string());
        role_t.erase (r_itr);
    }

    void newassign (const name&        assigned_account,
                                const name&        role_name,
                                const string&      info_url,
                                const string&      notes,
                                const uint64_t&     start_period,
                                const float&       time_share) {
                            
        require_auth (contract);

        auto sorted_by_assigned = assignment_t.get_index<"byassigned"_n>();

        auto a_itr = sorted_by_assigned.begin();
        while (a_itr != sorted_by_assigned.end()) {
            check (a_itr->role_name != role_name, "Assigned account already has this role. Assigned account: " 
                + assigned_account.to_string() + "; Role name: " + role_name.to_string());    
            a_itr++;
        }

        assignment_t.emplace (contract, [&](auto &a) {
        a.assignment_id      = assignment_t.available_primary_key();
        a.assigned_account   = assigned_account;
        a.role_name          = role_name;
        a.info_url           = info_url;
        a.notes              = notes;
        a.time_share         = time_share;
        a.start_period       = start_period;
        a.created_date       = current_block_time().to_time_point();
    });
    }

    void delassign (const uint64_t&     assignment_id) {

        require_auth (contract);

        auto a_itr = assignment_t.find (assignment_id);
        check (a_itr != assignment_t.end(), "Cannot delete assignment. Assignment ID does not exist: " + std::to_string(assignment_id));
        assignment_t.erase (a_itr);
    }

    asset adjust_asset (const asset& original_asset, const float& adjustment) {
        return asset { static_cast<int64_t> (original_asset.amount * adjustment), original_asset.symbol };
    }

    void payassign (const uint64_t& assignment_id, const uint64_t& period_id) {

        require_auth (contract);

        auto a_itr = assignment_t.find (assignment_id);
        check (a_itr != assignment_t.end(), "Cannot pay assignment. Assignment ID does not exist: " + std::to_string(assignment_id));

        auto r_itr = role_t.find (a_itr->role_name.value);
        check (r_itr != role_t.end(), "Cannot pay assignment. Role does not exist: " + a_itr->role_name.to_string());

        bank.makepayment (period_id, a_itr->assigned_account, adjust_asset(r_itr->hypha_salary, a_itr->time_share), 
            "Payment for role " + a_itr->role_name.to_string() + "; Period ID: " + std::to_string(period_id));

        bank.makepayment (period_id, a_itr->assigned_account, adjust_asset(r_itr->preseeds_salary, a_itr->time_share), 
            "Payment for role " + a_itr->role_name.to_string() + "; Period ID: " + std::to_string(period_id));
    }

};

#endif