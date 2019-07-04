#include "../../include/bank.hpp"

void bank::reset () {
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

void bank::set_config  (const name& hypha_token_contract, 
                        const name& preseeds_token_contract) {
    require_auth (contract.get_self());

    BankConfig bc = bankconfig_s.get_or_create (contract, BankConfig());
    bc.hypha_token_contract = hypha_token_contract;
    bc.preseeds_token_contract = preseeds_token_contract;
    bankconfig_s.set (bc, contract);
}
                        
void bank::makepayment (const uint64_t& period_id, const name& recipient, 
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

void bank::addperiod (const time_point& start_date, const time_point& end_date) {

    period_t.emplace (contract, [&](auto &p) {
        p.period_id     = period_t.available_primary_key();
        p.start_date    = start_date;
        p.end_date      = end_date;
    });
}

void bank::issuetoken(const name token_contract,
                    const name to,
                    const asset token_amount,
                    const string memo)
{
    EMIT_PAYTOKEN_EVENT (token_contract, from, to, token_amount, memo);

    action(
        permission_level{get_self(), "owner"_n},
        token_contract, "issue"_n,
        std::make_tuple(to, token_amount, memo))
    .send();
}
