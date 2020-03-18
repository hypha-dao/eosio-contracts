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
using std::map;

class Bank {

    public:

        struct [[eosio::table, eosio::contract("hyphadao") ]] Config 
        {
            // required configurations:
            // names : telos_decide_contract, hypha_token_contract, seeds_token_contract, last_ballot_id
            // ints  : voting_duration_sec
            map<string, name>          names             ;
            map<string, string>        strings           ;
            map<string, asset>         assets            ;
            map<string, time_point>    time_points       ;
            map<string, uint64_t>      ints              ;
            map<string, transaction>   trxs              ;
            map<string, float>         floats            ;
        };

        typedef singleton<"config"_n, Config> config_table;

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

        name                contract;
        period_table        period_t;
        payment_table       payment_t;
        config_table        config_s;

        Bank (const name& contract):
            contract (contract),
            payment_t (contract, contract.value),
            period_t (contract, contract.value),
            config_s (contract, contract.value) {}

       void reset () {
            require_auth (contract);
            
            auto pay_itr = payment_t.begin();
            while (pay_itr != payment_t.end()) {
                pay_itr = payment_t.erase (pay_itr);
            }
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
                          
        void makepayment (const uint64_t& period_id, 
                            const name& recipient, 
                            const asset& quantity, 
                            const string& memo, 
                            const uint64_t& assignment_id,
                            const uint64_t& bypass_escrow) {
    
            config_table      config_s (contract, contract.value);
   	        Config c = config_s.get_or_create (contract, Config());   
        
            if (quantity.symbol == common::S_HVOICE) {
                action(
                    permission_level{contract, "active"_n},
                    c.names.at("telos_decide_contract"), "mint"_n,
                    std::make_tuple(recipient, quantity, memo))
                .send();
            } else if (quantity.symbol == common::S_SEEDS) {
                
                if (bypass_escrow == 0) {
                    action(
                        permission_level{contract, "active"_n},
                        c.names.at("seeds_token_contract"), "transfer"_n,
                        std::make_tuple(contract, c.names.at("seeds_escrow_contract"), quantity, memo))
                    .send();

                    action(
                        permission_level{contract, "active"_n},
                        c.names.at("seeds_escrow_contract"), "lock"_n,
                        std::make_tuple("event"_n, 
                                        contract,
                                        recipient,
                                        quantity,
                                        "golive"_n,
                                        contract,
                                        time_point(current_time_point().time_since_epoch() + 
                                                    current_time_point().time_since_epoch()),  // long time from now
                                        memo))
                    .send();
                } else {
                    action(
                        permission_level{contract, "active"_n},
                        c.names.at("seeds_token_contract"), "transfer"_n,
                        std::make_tuple(contract, recipient, quantity, memo))
                    .send();
                }
            } else {   // handles HUSD and HYPHA
                // need to add steps in here about the deferments         
                issuetoken (c.names.at("hypha_token_contract"), recipient, quantity, memo );
            } 
            // else if (quantity.symbol == common::S_HUSD) {
            //     issuetoken (c.names.at("hypha_token_contract"), recipient, quantity, memo);
            // }
        
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
            config_table      config_s (contract, contract.value);
   	        Config c = config_s.get_or_create (contract, Config());   

            eosiotoken::accounts a_t (c.names.at("hypha_token_contract"), account.value);
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