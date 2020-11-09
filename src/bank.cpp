#include <hyphadao.hpp>

using namespace hyphaspace;

void hyphadao::make_payment(const uint64_t &period_id,
                            const name &recipient,
                            const asset &quantity,
                            const string &memo,
                            const uint64_t &assignment_id,
                            const uint64_t &bypass_escrow)
{
    if (quantity.amount == 0)
    {
        return;
    }

    debug("Making payment to recipient: " + recipient.to_string() + ", quantity: " + quantity.to_string());

    config_table config_s(get_self(), get_self().value);
    Config c = config_s.get_or_create(get_self(), Config());

    if (quantity.symbol == common::S_HVOICE)
    {
        action(
            permission_level{get_self(), name("active")},
            c.names.at("telos_decide_contract"), name("mint"),
            std::make_tuple(recipient, quantity, memo))
            .send();
    }
    else if (quantity.symbol == common::S_SEEDS)
    {      
        if (bypass_escrow == 0)
        {
            action(
                permission_level{get_self(), name("active")},
                c.names.at("seeds_token_contract"), name("transfer"),
                std::make_tuple(get_self(), c.names.at("seeds_escrow_contract"), quantity, memo))
                .send();

            action(
                permission_level{get_self(), name("active")},
                c.names.at("seeds_escrow_contract"), name("lock"),
                std::make_tuple(name("event"),
                                get_self(),
                                recipient,
                                quantity,
                                name("golive"),
                                get_self(),
                                time_point(current_time_point().time_since_epoch() +
                                           current_time_point().time_since_epoch()), // long time from now
                                memo))
                .send();
        }
        else
        {
            action(
                permission_level{get_self(), name("active")},
                c.names.at("seeds_token_contract"), name("transfer"),
                std::make_tuple(get_self(), recipient, quantity, memo))
                .send();
        }
    }
    else if (quantity.symbol == common::S_HUSD)
    {
        issuetoken(c.names.at("husd_token_contract"), c.names.at("treasury_contract"), recipient, quantity, memo);
    }
    else
    {
        issuetoken(c.names.at("hypha_token_contract"), get_self(), recipient, quantity, memo);
    }

    payment_table payment_t(get_self(), get_self().value);
    payment_t.emplace(get_self(), [&](auto &p) {
        p.payment_id = payment_t.available_primary_key();
        p.payment_date = current_block_time().to_time_point();
        p.period_id = period_id;
        p.assignment_id = assignment_id;
        p.recipient = recipient;
        p.amount = quantity;
        p.memo = memo;
    });
}

void hyphadao::issuetoken(const name &token_contract,
                          const name &issuer,
                          const name &to,
                          const asset &token_amount,
                          const string &memo)
{
    // translate USD to HUSD - should be temporary fix
    asset new_asset = token_amount;
    if (token_amount.symbol.code().to_string() == "USD")
    {
        new_asset = asset(token_amount.amount, common::S_HUSD);
    }
    const asset updated_asset = new_asset;

    string debug_str = "";
    debug_str = debug_str + "Issue Token Event; ";
    debug_str = debug_str + "    Token Contract  : " + token_contract.to_string() + "; ";
    debug_str = debug_str + "    Issuer          : " + issuer.to_string() + "; ";
    debug_str = debug_str + "    Issue To        : " + to.to_string() + "; ";
    debug_str = debug_str + "    Issue Amount    : " + updated_asset.to_string() + ";";
    debug_str = debug_str + "    Memo            : " + memo + ".";

    action(
        permission_level{issuer, name("active")},
        token_contract, name("issue"),
        std::make_tuple(issuer, updated_asset, memo))
        .send();

    action(
        permission_level{issuer, name("active")},
        token_contract, name("transfer"),
        std::make_tuple(issuer, to, updated_asset, memo))
        .send();

    debug(debug_str);
}
