#ifndef BANK_H
#define BANK_H

#include <eosio/eosio.hpp>
#include <eosio/asset.hpp>
#include <eosio/singleton.hpp>
#include <eosio/multi_index.hpp>

#include "eosiotoken.hpp"
#include "common.hpp"

using namespace eosio;
using std::string;

class Bank {

    public:

        struct [[ eosio::table, eosio::contract("hyphadao") ]] BankConfig
        {
            name           hypha_token_contract     = "token.hypha"_n;
            name           seeds_token_contract     = "token.seeds"_n;
            name           voice_token_contract     = "trailservice"_n;             
        };

        struct [[eosio::table, eosio::contract("hyphadao") ]] Period
        {
            uint64_t         period_id               ;
            time_point       start_date              ;
            time_point       end_date                ;
            string           phase                   ;

            uint64_t        primary_key()  const { return period_id; }
        };

        struct [[eosio::table, eosio::contract("hyphadao") ]] Payment
        {
            uint64_t        payment_id              ;
            time_point      payment_date            ;
            uint64_t        period_id               = 0;
            uint64_t        assignment_id           = -1;
            name            recipient               ;
            asset           amount                  ;
            string          memo                    ;

            uint64_t        primary_key()   const { return payment_id; }
            uint64_t        by_period ()    const { return period_id; }
            uint64_t        by_recipient()  const { return recipient.value; }
            uint64_t        by_assignment() const { return assignment_id; }
        };

        typedef multi_index<"periods"_n, Period> period_table;

        typedef multi_index<"payments"_n, Payment,
            indexed_by<"byperiod"_n, const_mem_fun<Payment, uint64_t, &Payment::by_period>>,
            indexed_by<"byrecipient"_n, const_mem_fun<Payment, uint64_t, &Payment::by_recipient>>,
            indexed_by<"byassignment"_n, const_mem_fun<Payment, uint64_t, &Payment::by_assignment>>
        > payment_table;

        typedef singleton<"bankconfig"_n, BankConfig> bankconfig_table;
        typedef multi_index<"bankconfig"_n, BankConfig> bankconfig_table_placeholder;

        name                contract;
        period_table        period_t;
        payment_table       payment_t;
        bankconfig_table    bankconfig_s;

        Bank (const name& contract):
            contract (contract),
            payment_t (contract, contract.value),
            period_t (contract, contract.value),
            bankconfig_s (contract, contract.value) {}

       void reset () {
            require_auth (contract);
            bankconfig_s.remove ();
            
            auto pay_itr = payment_t.begin();
            while (pay_itr != payment_t.end()) {
                pay_itr = payment_t.erase (pay_itr);
            }
        }

        void reset_config () {
            require_auth (contract);
            bankconfig_s.remove ();
        }

        void remove_periods (const uint64_t& begin_period_id, 
                           const uint64_t& end_period_id) {
            require_auth (contract);

            auto p_itr = period_t.find (begin_period_id);
            check (p_itr != period_t.end(), "Begin period ID not found: " + std::to_string(begin_period_id));

            while (p_itr->period_id <= end_period_id) {
                p_itr = period_t.erase (p_itr);
            }
        }

        void reset_periods() {
            require_auth (contract);
            auto per_itr = period_t.begin();
            while (per_itr != period_t.end()) {
                per_itr = period_t.erase (per_itr);
            }
        }

        void set_config  (const name& hypha_token_contract, 
                            const name& seeds_token_contract,
                            const name& trail_contract) {
            require_auth (contract);

            check (is_account(hypha_token_contract), "HYPHA token contract is not an account: " + hypha_token_contract.to_string());
            check (is_account(seeds_token_contract), "HYPHA token contract is not an account: " + seeds_token_contract.to_string());

            BankConfig bc = bankconfig_s.get_or_create (contract, BankConfig());
            bc.hypha_token_contract = hypha_token_contract;
            bc.seeds_token_contract = seeds_token_contract;
            bc.voice_token_contract = trail_contract;
            bankconfig_s.set (bc, contract);
        }
                                
        void makepayment (const uint64_t& period_id, const name& recipient, 
                                    const asset& quantity, const string& memo, 
                                    const uint64_t& assignment_id) {
            
            BankConfig bc = bankconfig_s.get_or_create (contract, BankConfig());
        
            if (quantity.symbol == common::S_HVOICE) {
                action(
                    permission_level{contract, "active"_n},
                    bc.voice_token_contract, "mint"_n,
                    std::make_tuple(recipient, quantity, memo))
                .send();
            } else if (quantity.symbol == common::S_SEEDS) {
                action(
                    permission_level{contract, "active"_n},
                    bc.seeds_token_contract, "transfer"_n,
                    std::make_tuple(contract, recipient, quantity, memo))
                .send();
            } else {
                issuetoken (bc.hypha_token_contract, recipient, quantity, memo );
            }
        
            payment_t.emplace (contract, [&](auto &p) {
                p.payment_id    = payment_t.available_primary_key();
                p.payment_date  = current_block_time().to_time_point();
                p.period_id     = period_id;
                p.assignment_id = assignment_id;
                p.recipient     = recipient;
                p.amount        = quantity;
                p.memo          = memo;
            });
        }

        void addperiod (const time_point& start_date, const time_point& end_date, const string& phase) {

            period_t.emplace (contract, [&](auto &p) {
                p.period_id     = period_t.available_primary_key();
                p.start_date    = start_date;
                p.end_date      = end_date;
                p.phase         = phase;
            });
        }

        void issuetoken(const name& token_contract,
                            const name& to,
                            const asset& token_amount,
                            const string& memo)
        {
            print ("\nIssue Token Event\n");
            print ("    Token Contract  : ", token_contract.to_string(), "\n");
            print ("    Issue To        : ", to.to_string(), "\n");
            print ("    Issue Amount    : ", token_amount.to_string(), "\n");
            print ("    Memo            : ", memo, "\n\n");

            action(
                permission_level{contract, "active"_n},
                token_contract, "issue"_n,
                std::make_tuple(contract, token_amount, memo))
            .send();

            action(
                permission_level{contract, "active"_n},
                token_contract, "transfer"_n,
                std::make_tuple(contract, to, token_amount, memo))
            .send();
        }

        bool holds_hypha (const name& account) 
        {
            BankConfig bc = bankconfig_s.get_or_create (contract, BankConfig());

            eosiotoken::accounts a_t (bc.hypha_token_contract, account.value);
            auto a_itr = a_t.find (common::S_HYPHA.code().raw());
            if (a_itr == a_t.end()) {
                return false;
            } else if (a_itr->balance.amount > 0) {
                return true;
            } else {
                return false;
            }
        }

};

#endif