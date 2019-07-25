#include "../../include/bank.hpp"

void Bank::reset () {
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

void Bank::reset_periods() {
    require_auth (contract);
    auto per_itr = period_t.begin();
    while (per_itr != period_t.end()) {
        per_itr = period_t.erase (per_itr);
    }
}

void Bank::set_config  (const name& hypha_token_contract, 
                        const name& preseeds_token_contract) {
    require_auth (contract);

    check (is_account(hypha_token_contract), "HYPHA token contract is not an account: " + hypha_token_contract.to_string());
    check (is_account(preseeds_token_contract), "HYPHA token contract is not an account: " + preseeds_token_contract.to_string());

    BankConfig bc = bankconfig_s.get_or_create (contract, BankConfig());
    bc.hypha_token_contract = hypha_token_contract;
    bc.preseeds_token_contract = preseeds_token_contract;
    bankconfig_s.set (bc, contract);
}
                        
void Bank::makepayment (const uint64_t& period_id, const name& recipient, 
                            const asset& quantity, const string& memo, 
                            const uint64_t& assignment_id) {
    
    BankConfig bc = bankconfig_s.get_or_create (contract, BankConfig());
   
    if (quantity.symbol == common::S_HVOICE) {
        action(
            permission_level{contract, "active"_n},
            "eosio.trail"_n, "issuetoken"_n,
            std::make_tuple(contract, recipient, quantity, false))
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

void Bank::addperiod (const time_point& start_date, const time_point& end_date, const string& phase) {

    period_t.emplace (contract, [&](auto &p) {
        p.period_id     = period_t.available_primary_key();
        p.start_date    = start_date;
        p.end_date      = end_date;
        p.phase         = phase;
    });
}

void Bank::issuetoken(const name& token_contract,
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
        std::make_tuple(to, token_amount, memo))
    .send();
}
