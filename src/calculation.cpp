#include <hyphadao.hpp>

using namespace hyphaspace;

// retrieve the seeds price as of a specific point in time
float hyphadao::get_seeds_price_usd (const time_point& price_time_point) 
{
    price_history_tables ph_t (name("tlosto.seeds"), name("tlosto.seeds").value);

    // start at the end and decrement until the input time point is greater than the date on the price history table
    // assume price history table is in proper sequence; it does not have an index on the date
    auto ph_itr = ph_t.rbegin();
    while ( ph_itr->id > ph_t.begin()->id && 
            price_time_point.sec_since_epoch() < ph_itr->date.sec_since_epoch()) {
        ph_itr++;
    }

    asset seeds_usd = ph_itr->seeds_usd;
    float seeds_usd_float = (float) seeds_usd.amount / (float) pow(10,seeds_usd.symbol.precision());
    return (float)1 / ( seeds_usd_float ); 
}

// get the current SEEDS price
float hyphadao::get_seeds_price_usd () 
{
    configtables c_t(name("tlosto.seeds"), name("tlosto.seeds").value);
    configtable config_t = c_t.get();

    float seeds_price_usd = (float)1 / ((float) config_t.seeds_per_usd.amount / (float)10000); 
    return (float)1 / ((float) config_t.seeds_per_usd.amount / (float)config_t.seeds_per_usd.symbol.precision());
}

// this is called on assignment payment claims, to retrieve the SEEDS amount to be paid from the time of the period being claimed
asset hyphadao::get_seeds_amount (const asset &usd_amount, 
                                    const time_point &price_time_point, 
                                    const float &time_share, 
                                    const float &deferred_perc) 
{
    asset adjusted_usd_amount = adjust_asset(adjust_asset(usd_amount, deferred_perc), time_share);

    config_table config_s(get_self(), get_self().value);
    Config c = config_s.get_or_create(get_self(), Config());
    float seeds_deferral_coeff = get_float(c.ints, "seeds_deferral_factor_x100");

    float seeds_price = get_seeds_price_usd(price_time_point);
    debug("get_seeds_amount: INPUT: USD Amount: " + usd_amount.to_string() +
        ", INPUT: deferred_perc: " + std::to_string(deferred_perc) +
        ", INPUT: time_share: " + std::to_string(time_share) +
        ", INPUT: seeds_deferral_factor_x100: " + std::to_string(seeds_deferral_coeff) +
        ", CALC: adjusted_usd_amount: " + adjusted_usd_amount.to_string() +
        ", CALC: get_seeds_price_usd: " + std::to_string(seeds_price));

    return adjust_asset(asset{static_cast<int64_t>(adjusted_usd_amount.amount * (float) 100 * (float) seeds_deferral_coeff), common::S_SEEDS}, (float)1 / (float)seeds_price);
}

map<string, asset> hyphadao::get_assets(const asset &usd_amount,
                                        const float &deferred_perc, 
                                        const time_point &price_time_point) 
{
    map<string, asset> assets;

    config_table config_s(get_self(), get_self().value);
    Config c = config_s.get_or_create(get_self(), Config());
    float hypha_deferral_coeff = get_float(c.ints, "hypha_deferral_factor_x100");
    asset deferred_adjusted_usd_amount = adjust_asset(usd_amount, deferred_perc);

    assets["hypha_amount"] = adjust_asset(asset{deferred_adjusted_usd_amount.amount, common::S_HYPHA}, hypha_deferral_coeff);
    assets["hvoice_amount"] = asset{usd_amount.amount, common::S_HVOICE};
    assets["husd_amount"] = asset{usd_amount.amount - deferred_adjusted_usd_amount.amount, common::S_HUSD};
    assets["seeds_escrow_amount"] = get_seeds_amount (	usd_amount, 
                                                        price_time_point, 
                                                        float(1.0000000000000),
                                                        deferred_perc);

    debug("Calculations for get_assets: INPUT: USD Amount: " + usd_amount.to_string() +
            ", INPUT: deferred_perc: " + std::to_string(deferred_perc) +
            ", CALC: hypha_amount: " + assets["hypha_amount"].to_string() +
            ", CALC: husd_amount: " + assets["husd_amount"].to_string() +
            ", CALC: seeds_escrow_amount: " + assets["seeds_escrow_amount"].to_string() +
            ", CALC: hvoice_amount: " + assets["hvoice_amount"].to_string());

    return assets;            
}

// this is used when calculating the assets for an assignment proposal
map<string, asset> hyphadao::get_assets(const uint64_t &role_id, 
                                        const float &deferred_perc, 
                                        const float &time_share_perc) 
{
    map<string, asset> assets;

    // global ratios
    config_table config_s(get_self(), get_self().value);
    Config c = config_s.get_or_create(get_self(), Config());
    // float seeds_deferral_coeff = get_float(c.ints, "seeds_deferral_factor_x100");
    float hypha_deferral_coeff = get_float(c.ints, "hypha_deferral_factor_x100");

    object_table o_t_role(get_self(), name("role").value);
    auto o_itr_role = o_t_role.find(role_id);
    checkx(o_itr_role != o_t_role.end(), "Role ID: " + std::to_string(role_id) + " does not exist.");

    // NEW: Hard copy the annual_usd_salary from the role to the assignment as a period amount
    asset usd_salary_value_per_phase = adjust_asset(o_itr_role->assets.at("annual_usd_salary"), common::PHASE_TO_YEAR_RATIO);
    assets["usd_salary_value_per_phase"] = usd_salary_value_per_phase;

    // calculate HUSD salary amount
    // 1. normalize annual salary to the time commitment of this proposal
    // 2. multiply (1) by 0.02026 to calculate a single moon phase; avg. phase is 7.4 days, 49.36 phases per year
    // 3. multiply (2) by 1 - deferral perc
    asset commitment_adjusted_usd_annual = adjust_asset(o_itr_role->assets.at("annual_usd_salary"), time_share_perc);
    asset commitment_adjusted_usd_phase = adjust_asset(commitment_adjusted_usd_annual, common::PHASE_TO_YEAR_RATIO);
    asset nondeferred_and_commitment_adj_usd_phase = adjust_asset(commitment_adjusted_usd_phase, (float)1 - (float)deferred_perc);

    // convert symbol
    asset husd_salary_per_phase = asset{nondeferred_and_commitment_adj_usd_phase.amount, common::S_HUSD};
    assets["husd_salary_per_phase"] = husd_salary_per_phase;

    // calculate HYPHA phase salary amount
    asset deferred_and_commitment_adj_usd_phase = adjust_asset(commitment_adjusted_usd_phase, (float)deferred_perc);
    assets["hypha_salary_per_phase"] = adjust_asset(asset{deferred_and_commitment_adj_usd_phase.amount, common::S_HYPHA}, hypha_deferral_coeff);

    // calculate HVOICE phase salary amount, which is $1.00 USD == 2 HVOICE
    assets["hvoice_salary_per_phase"] = asset{commitment_adjusted_usd_phase.amount * 2, common::S_HVOICE};

    debug("Calculations for get_assets: INPUT: Role: " + std::to_string(role_id) +
            ", INPUT: deferred_perc: " + std::to_string(deferred_perc) +
            ", INPUT: time_share_perc: " + std::to_string(time_share_perc) +
            ", CALC: usd_salary_value_per_phase: " + assets["usd_salary_value_per_phase"].to_string() +
            ", CALC: hypha_salary_per_phase: " + assets["hypha_salary_per_phase"].to_string() +
            ", CALC: husd_salary_per_phase: " + assets["husd_salary_per_phase"].to_string() +
            ", CALC: hvoice_salary_per_phase: " + assets["hvoice_salary_per_phase"].to_string());

    return assets;
}

void hyphadao::fixseedsprec (const uint64_t &proposal_id) 
{
    require_auth(get_self());

    object_table o_t(get_self(), name("proposal").value);
    auto p_itr = o_t.find(proposal_id);
    check(p_itr != o_t.end(), "Proposal ID does not exist: " + std::to_string(proposal_id));

    std::map<string, asset>::const_iterator asset_itr;
    for (asset_itr = p_itr->assets.begin(); asset_itr != p_itr->assets.end(); ++asset_itr)
    {
        if (asset_itr->first == "seeds_escrow_amount" && 
            asset_itr->second.symbol.code().to_string() == "SEEDS" && 
            asset_itr->second.symbol.precision() == 2) {
                o_t.modify (p_itr, get_self(), [&](auto &o) {
                    o.assets["seeds_escrow_amount"] = asset { asset_itr->second.amount * 100, common::S_SEEDS };
                });
        }
    }
}

void hyphadao::updassets(const uint64_t &proposal_id)
{
    require_auth(get_self());

    object_table o_t(get_self(), name("proposal").value);
    auto p_itr = o_t.find(proposal_id);
    check(p_itr != o_t.end(), "Proposal ID does not exist: " + std::to_string(proposal_id));

    o_t.modify(p_itr, get_self(), [&](auto &p) {
        // merge calculated assets into map
        map<string, asset> calculated_assets = get_assets(p_itr->ints.at("role_id"), get_float(p_itr->ints, "deferred_perc_x100"), get_float(p_itr->ints, "time_share_x100"));
        std::map<string, asset>::const_iterator asset_itr;
        for (asset_itr = calculated_assets.begin(); asset_itr != calculated_assets.end(); ++asset_itr)
        {
            p.assets[asset_itr->first] = asset_itr->second;
        }
    });
}

void hyphadao::updassassets(const uint64_t &assignment_id)
{
    require_auth(get_self());

    object_table o_t(get_self(), name("assignment").value);
    auto a_itr = o_t.find(assignment_id);
    check(a_itr != o_t.end(), "Assignment ID does not exist: " + std::to_string(assignment_id));

    o_t.modify(a_itr, get_self(), [&](auto &a) {
        auto instant_seeds_itr = a.assets.find("seeds_instant_salary_per_phase");
        if (instant_seeds_itr != a.assets.end())
        {
            a.assets.erase(instant_seeds_itr);
        }

        auto escrow_seeds_itr = a.assets.find("seeds_escrow_salary_per_phase");
        if (escrow_seeds_itr != a.assets.end())
        {
            a.assets.erase(escrow_seeds_itr);
        }

        // merge calculated assets into map
        map<string, asset> calculated_assets = get_assets(a_itr->ints.at("role_id"), get_float(a_itr->ints, "deferred_perc_x100"), get_float(a_itr->ints, "time_share_x100"));
        std::map<string, asset>::const_iterator asset_itr;
        for (asset_itr = calculated_assets.begin(); asset_itr != calculated_assets.end(); ++asset_itr)
        {
            a.assets[asset_itr->first] = asset_itr->second;
        }
        a.updated_date = current_time_point();
    });
}
