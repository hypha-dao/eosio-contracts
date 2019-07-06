#ifndef BANK_H
#define BANK_H

#include <eosio/eosio.hpp>
#include <eosio/asset.hpp>
#include <eosio/singleton.hpp>
#include <eosio/multi_index.hpp>

using namespace eosio;
using std::string;

class Bank {

    public:

        struct [[ eosio::table, eosio::contract("hyphadao") ]] BankConfig
        {
            name           hypha_token_contract    = "hyphatoken"_n;
            name           voice_token_contract    = "hyphatoken"_n;
            name           preseeds_token_contract = "hyphatoken"_n;
        };

        struct [[eosio::table, eosio::contract("hyphadao") ]] Period
        {
            uint64_t         period_id               ;
            time_point       start_date              ;
            time_point       end_date                ;

            uint64_t        primary_key()  const { return period_id; }
        };

        struct [[eosio::table, eosio::contract("hyphadao") ]] Payment
        {
            uint64_t        payment_id              ;
            time_point      payment_date            ;
            uint64_t        period_id               = 0;
            name            recipient               ;
            asset           amount                  ;
            string          memo                    ;

            uint64_t        primary_key()   const { return payment_id; }
        };

        typedef multi_index<"periods"_n, Period> period_table;
        typedef multi_index<"payments"_n, Payment> payment_table;
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

        // void reset ();
        // void set_config (const name& hypha_token_contract, 
        //                 const name& preseeds_token_contract);

        // void makepayment (const uint64_t& period_id, const name& recipient, 
        //                     const asset& quantity, const string& memo);

        // void issuetoken (const name& token_contract,
        //                 const name& to,
        //                 const asset& token_amount,
        //                 const string& memo);

        // void addperiod (const time_point& start_date, const time_point& end_date);


        void reset () {
            require_auth (contract);
            bankconfig_s.remove ();
            auto per_itr = period_t.begin();
            while (per_itr != period_t.end()) {
                per_itr = period_t.erase (per_itr);
            }

            auto pay_itr = payment_t.begin();
            while (pay_itr != payment_t.end()) {
                pay_itr = payment_t.erase (pay_itr);
            }
        }

        void set_config  (const name& hypha_token_contract, 
                                const name& preseeds_token_contract) {
            require_auth (contract);

            check (is_account(hypha_token_contract), "HYPHA token contract is not an account: " + hypha_token_contract.to_string());
            check (is_account(preseeds_token_contract), "HYPHA token contract is not an account: " + preseeds_token_contract.to_string());

            BankConfig bc = bankconfig_s.get_or_create (contract, BankConfig());
            bc.hypha_token_contract = hypha_token_contract;
            bc.preseeds_token_contract = preseeds_token_contract;
            bankconfig_s.set (bc, contract);
        }
                                
        void makepayment (const uint64_t& period_id, const name& recipient, 
                                    const asset& quantity, const string& memo) {
            
            BankConfig bc = bankconfig_s.get_or_create (contract, BankConfig());
            issuetoken (bc.hypha_token_contract, recipient, quantity, memo );
            payment_t.emplace (contract, [&](auto &p) {
                p.payment_id    = payment_t.available_primary_key();
                p.payment_date  = current_block_time().to_time_point();
                p.period_id     = period_id;
                p.recipient     = recipient;
                p.amount        = quantity;
                p.memo          = memo;
            });
        }

        void addperiod (const time_point& start_date, const time_point& end_date) {

            period_t.emplace (contract, [&](auto &p) {
                p.period_id     = period_t.available_primary_key();
                p.start_date    = start_date;
                p.end_date      = end_date;
            });
        }

        void issuetoken(const name& token_contract,
                            const name& to,
                            const asset& token_amount,
                            const string& memo)
        {
            //EMIT_PAYTOKEN_EVENT (token_contract, from, to, token_amount, memo);

            print ("\nIssue Token Event\n");
            print ("    Token Contract  : ", token_contract.to_string(), "\n");
            print ("    Issue To        : ", to.to_string(), "\n");
            print ("    Issue Amount    : ", token_amount.to_string(), "\n");
            print ("    Memo            : ", memo, "\n\n");

            action(
                permission_level{contract, "active"_n},
                token_contract, "issue"_n,
                std::make_tuple(to, token_amount, memo))
            .send();

            // asset a = asset { 100000000, common::S_PRESEEDS };

            // string memo1 { "memo" };
            // action(
            //     permission_level{"hyphadaobal1"_n, "active"_n},
            //     "hyphatoken12"_n, "issue"_n,
            //     std::make_tuple("hyphamember1"_n, a, memo1))
            // .send();
        }

};

#endif