#ifndef HOLOCRACY_H
#define HOLOCRACY_H

#include <eosio/eosio.hpp>
#include <eosio/asset.hpp>
#include <eosio/singleton.hpp>
#include <eosio/multi_index.hpp>

#include "bank.hpp"
#include "common.hpp"

using namespace eosio;
using std::vector;
using std::string;

class Holocracy {

    public:

        struct [[eosio::table, eosio::contract("hyphadao") ]] Role
        {
            uint64_t        role_id                 = 0;
            string          title                   ;
            string          description             ;
            string          content                 ;
            asset           hypha_salary            = asset { 0, common::S_HYPHA };
            asset           seeds_salary            = asset { 0, common::S_SEEDS };
            asset           voice_salary            = asset { 0, common::S_HVOICE };
            uint64_t        start_period            ;
            uint64_t        end_period              ;

            time_point      created_date            = current_block_time().to_time_point();
            time_point      updated_date            = current_block_time().to_time_point();

            uint64_t        primary_key()           const { return role_id; }
        };

        struct [[eosio::table, eosio::contract("hyphadao") ]] Assignment
        {
            uint64_t        assignment_id           = 0;
            name            assigned_account        ;
            uint64_t        role_id                 ;
            string          title                   ;
            string          description             ;
            string          content                 ;
            uint64_t        start_period            ;
            uint64_t        end_period              ;
            float           time_share              = 0.000000000000000;

            time_point      created_date            = current_block_time().to_time_point();
            time_point      updated_date            = current_block_time().to_time_point();

            uint64_t        primary_key()           const { return assignment_id; }
            uint64_t        by_assigned()           const { return assigned_account.value; }
            uint64_t        by_role()               const { return role_id; }
        };

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
            asspay_t (contract, contract.value),
            bank (contract) {}

        role_table          role_t;
        assignment_table    assignment_t;
        asspay_table        asspay_t;
        Bank                bank;
        name                contract;

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

        void newrole (  const string& title, 
                        const string& description,
                        const string& content,
                        const asset& hypha_salary,
                        const asset& seeds_salary,
                        const asset& voice_salary,
                        const uint64_t& start_period,
                        const uint64_t& end_period) {

            require_auth (contract);

            check (end_period >= start_period, "End period must be greater than or equal to start period.");

            role_t.emplace (contract, [&](auto &r) {
                r.role_id           = role_t.available_primary_key();
                r.title             = title;
                r.description       = description;
                r.content           = content;
                r.hypha_salary      = hypha_salary;
                r.seeds_salary      = seeds_salary;
                r.voice_salary      = voice_salary;
                r.start_period      = start_period;
                r.end_period        = end_period;
                r.created_date      = current_block_time().to_time_point();
            });
        }

        void delrole (const uint64_t& role_id) {

            require_auth (contract);

            auto ass_by_role = assignment_t.get_index<"byrole"_n>();
            check (ass_by_role.find(role_id) == ass_by_role.end(), 
                "Delete role failed. Role has existing assignments: " + std::to_string(role_id));

            auto r_itr = role_t.find (role_id);
            check (r_itr != role_t.end(), "Delete role failed. Role does not exist: " + std::to_string(role_id));
            role_t.erase (r_itr);
        }

        void newassign (const name&         assigned_account,
                        const uint64_t&     role_id,
                        const string&       title,
                        const string&       description,
                        const string&       content,
                        const uint64_t&     start_period,
                        const uint64_t&     end_period,
                        const float&        time_share) {
                                
            require_auth (contract);

            // ensure that the account doesn't already have this role
            auto sorted_by_assigned = assignment_t.get_index<"byassigned"_n>();
            auto a_itr = sorted_by_assigned.begin();
            while (a_itr != sorted_by_assigned.end()) {
                check (! (a_itr->role_id == role_id && a_itr->assigned_account == assigned_account), "Assigned account already has this role. Assigned account: " 
                    + assigned_account.to_string() + "; Role ID: " + std::to_string(role_id));    
                a_itr++;
            }

            assignment_t.emplace (contract, [&](auto &a) {
                a.assignment_id     = assignment_t.available_primary_key();
                a.assigned_account  = assigned_account;
                a.role_id           = role_id;
                a.title             = title;
                a.description       = description;
                a.content           = content;
                a.time_share        = time_share;
                a.start_period      = start_period;
                a.end_period        = end_period;
                a.created_date      = current_block_time().to_time_point();
            });
        }

        void delassign (const uint64_t&     assignment_id) {

            require_auth (contract);

            auto a_itr = assignment_t.find (assignment_id);
            check (a_itr != assignment_t.end(), 
                "Cannot delete assignment. Assignment ID does not exist: " + std::to_string(assignment_id));
            assignment_t.erase (a_itr);
        }

        void payassign (const uint64_t& assignment_id, const uint64_t& period_id) {

            auto a_itr = assignment_t.find (assignment_id);
            check (a_itr != assignment_t.end(), "Cannot pay assignment. Assignment ID does not exist: " + std::to_string(assignment_id));

            require_auth (a_itr->assigned_account);

            auto r_itr = role_t.find (a_itr->role_id);
            check (r_itr != role_t.end(), "Cannot pay assignment. Role ID does not exist: " + std::to_string(a_itr->role_id));

            // Check that the assignment has not been paid for this period yet
            auto period_index = asspay_t.get_index<"byperiod"_n>();
            auto per_itr = period_index.find (period_id);
            while (per_itr->period_id == period_id && per_itr != period_index.end()) {
                check (per_itr->assignment_id != assignment_id, "Assignment ID has already been paid for this period. Period ID: " +
                    std::to_string(period_id) + "; Assignment ID: " + std::to_string(assignment_id));
                per_itr++;
            }

            // Check that the period has elapsed
            auto p_itr = bank.period_t.find (period_id);
            check (p_itr != bank.period_t.end(), "Cannot make payment. Period ID not found: " + std::to_string(period_id));
            check (p_itr->end_date.sec_since_epoch() < current_block_time().to_time_point().sec_since_epoch(), 
                "Cannot make payment. Period ID " + std::to_string(period_id) + " has not closed yet.");

            // debug ( "Assignment created date : " + a_itr->created_date.to_string() + "; Seconds    : " + std::to_string(a_itr->created_date.sec_since_epoch()));
            // debug ( "Period end              : " + p_itr->end_date.to_string() + ";  Seconds: " + std::to_string(p_itr->end_date.sec_since_epoch()));

            debug ( "Assignment created date Seconds    : " + std::to_string(a_itr->created_date.sec_since_epoch()));
            debug ( "Period end Seconds : " + std::to_string(p_itr->end_date.sec_since_epoch()));

            // check that the creation date of the assignment is before the end of the period
            check (a_itr->created_date.sec_since_epoch() < p_itr->end_date.sec_since_epoch(), 
                "Cannot make payment to assignment. Assignment was not approved before this period.");

            // check that pay period is between (inclusive) the start and end period of the role and the assignment
            check (a_itr->start_period <= period_id && a_itr->end_period >= period_id, "For assignment, period ID must be between " +
                std::to_string(a_itr->start_period) + " and " + std::to_string(a_itr->end_period) + " (inclusive). You tried: " + std::to_string(period_id));

            check (r_itr->start_period <= period_id && r_itr->end_period >= period_id, "For role, period ID must be between " +
                std::to_string(r_itr->start_period) + " and " + std::to_string(r_itr->end_period) + " (inclusive). You tried: " + std::to_string(period_id));

            float time_share_calc = a_itr->time_share;

            // pro-rate the payout if the assignment was created 
            if (a_itr->created_date.sec_since_epoch() > p_itr->start_date.sec_since_epoch()) {
                time_share_calc = time_share_calc * (float) ( (float) p_itr->end_date.sec_since_epoch() - a_itr->created_date.sec_since_epoch()) / 
                                    ( (float) p_itr->end_date.sec_since_epoch() - p_itr->start_date.sec_since_epoch());
            }

            // print ("Time share used in payout calculation: ", std::to_string(time_share_calc), "\n");
            asset hypha_payment = adjust_asset(r_itr->hypha_salary, time_share_calc);
            asset seeds_payment = adjust_asset(r_itr->seeds_salary, time_share_calc);
            asset voice_payment = adjust_asset(r_itr->voice_salary, time_share_calc);

            asspay_t.emplace (contract, [&](auto &a) {
                a.ass_payment_id        = asspay_t.available_primary_key();
                a.assignment_id         = assignment_id;
                a.recipient             = a_itr->assigned_account,
                a.period_id             = period_id;
                a.payment_date          = current_block_time().to_time_point();
                a.payments.push_back (hypha_payment);
                a.payments.push_back (seeds_payment);
                a.payments.push_back (voice_payment);
            });

            bank.makepayment (period_id, a_itr->assigned_account, hypha_payment, 
                "Payment for role " + std::to_string(a_itr->role_id) + "; Period ID: " + std::to_string(period_id),
                assignment_id);

            bank.makepayment (period_id, a_itr->assigned_account, seeds_payment, 
                "Payment for role " + std::to_string(a_itr->role_id) + "; Period ID: " + std::to_string(period_id),
                assignment_id);

            bank.makepayment (period_id, a_itr->assigned_account, voice_payment, 
                "Payment for role " + std::to_string(a_itr->role_id) + "; Period ID: " + std::to_string(period_id),
                assignment_id);
        }

        asset adjust_asset (const asset& original_asset, const float& adjustment) {
            return asset { static_cast<int64_t> (original_asset.amount * adjustment), original_asset.symbol };
        }   

        struct [[eosio::table, eosio::contract("hyphadao") ]] Debug
        {
            uint64_t    debug_id;
            string      notes;
            time_point  created_date = current_time_point();
            uint64_t    primary_key()  const { return debug_id; }
        };

        typedef multi_index<"debugs"_n, Debug> debug_table;

        void debug (const string& notes) {
            debug_table d_t (contract, contract.value);
            d_t.emplace (contract, [&](auto &d) {
                d.debug_id = d_t.available_primary_key();
                d.notes = notes;
            });
        }
};

#endif