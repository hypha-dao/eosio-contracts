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

        struct [[ eosio::table, eosio::contract("hyphadac") ]] BankConfig
        {
            name           hypha_token_contract    = "hyphatoken"_n;
            name           voice_token_contract    = "hyphatoken"_n;
            name           preseeds_token_contract = "hyphatoken"_n;
        };

        struct [[eosio::table, eosio::contract("hyphadac") ]] Period
        {
            uint64_t         period_id               ;
            time_point       start_date              = 0;
            time_point       end_date                = 0;
        };

        struct [[eosio::table, eosio::contract("hyphadac") ]] Payment
        {
            uint64_t        payment_id               ;
            time_point      payment_date           = 0;
            uint64_t        period_id               = 0;
            name            recipient               = 0;
            asset           amount              ;
            string          memo                ;

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
            period_t (contract, contract.value) {}

        void reset ();
        void set_config (const name& hypha_token_contract, 
                        const name& preseeds_token_contract);

        void makepayment (const uint64_t& period_id, const name& recipient, 
                            const asset& quantity, const string& memo);

        void addperiod (const time_point& start_date, const time_point& end_date);

}

#endif