#include <hyphadao.hpp>

using namespace hyphaspace;

void hyphadao::updassets (const uint64_t &proposal_id) 
{
    require_auth (get_self());

    object_table o_t(get_self(), name("proposal").value);
    auto p_itr = o_t.find(proposal_id);
    check (p_itr != o_t.end(), "Proposal ID does not exist: " + std::to_string(proposal_id));

    o_t.modify (p_itr, get_self(), [&](auto &p) {
        // merge calculated assets into map
        map<string, asset> calculated_assets = get_assets(p_itr->ints.at("role_id"), get_float(p_itr->ints, "deferred_perc_x100"), get_float(p_itr->ints, "time_share_x100"));
        std::map<string, asset>::const_iterator asset_itr;
        for (asset_itr = calculated_assets.begin(); asset_itr != calculated_assets.end(); ++asset_itr) {
            p.assets[asset_itr->first] = asset_itr->second;
        }
    });
}

void hyphadao::updassassets (const uint64_t &assignment_id) 
{
    require_auth (get_self());

    object_table o_t(get_self(), name("assignment").value);
    auto a_itr = o_t.find(assignment_id);
    check (a_itr != o_t.end(), "Assignment ID does not exist: " + std::to_string(assignment_id));

    o_t.modify (a_itr, get_self(), [&](auto &a) {

        auto instant_seeds_itr = a.assets.find ("seeds_instant_salary_per_phase");
        if (instant_seeds_itr != a.assets.end()) {
            a.assets.erase (instant_seeds_itr);
        }

        
        // merge calculated assets into map
        map<string, asset> calculated_assets = get_assets(a_itr->ints.at("role_id"), get_float(a_itr->ints, "deferred_perc_x100"), get_float(a_itr->ints, "time_share_x100"));
        std::map<string, asset>::const_iterator asset_itr;
        for (asset_itr = calculated_assets.begin(); asset_itr != calculated_assets.end(); ++asset_itr) {
            a.assets[asset_itr->first] = asset_itr->second;
        }
        a.updated_date = current_time_point();
    });
}

float hyphadao::get_seeds_price_usd () {
    configtables c_t(name("tlosto.seeds"), name("tlosto.seeds").value);
    configtable config_t = c_t.get();

    float seeds_price_usd = (float)1 / ((float) config_t.seeds_per_usd.amount / (float)10000); 
    return seeds_price_usd;
}

map<string, asset> hyphadao::get_assets(const asset &usd_amount,
                                        const float &deferred_perc) 
{
    map<string, asset> assets;

    config_table config_s(get_self(), get_self().value);
    Config c = config_s.get_or_create(get_self(), Config());
    float hypha_deferral_coeff = get_float(c.ints, "hypha_deferral_factor_x100");
    asset deferred_adjusted_usd_amount = adjust_asset(usd_amount, deferred_perc);

    assets["hypha_amount"] = adjust_asset(asset{deferred_adjusted_usd_amount.amount, common::S_HYPHA}, hypha_deferral_coeff);
    assets["hvoice_amount"] = asset{usd_amount.amount, common::S_HVOICE};
    assets["husd_amount"] = asset{usd_amount.amount - deferred_adjusted_usd_amount.amount, common::S_HUSD};

    float seeds_deferral_coeff = get_float(c.ints, "seeds_deferral_factor_x100");
    assets["seeds_escrow_amount"] = adjust_asset(asset{deferred_adjusted_usd_amount.amount * 100, common::S_SEEDS}, (float)1 / (float)get_seeds_price_usd() * seeds_deferral_coeff);

    debug("Calculations for get_assets: USD Amount: " + usd_amount.to_string() +
            ", deferred_perc: " + std::to_string(deferred_perc) +
            ", hypha_amount: " + assets["hypha_amount"].to_string() +
            ", husd_amount: " + assets["husd_amount"].to_string() +
            ", seeds_escrow_amount: " + assets["seeds_escrow_amount"].to_string() +
            ", hvoice_amount: " + assets["hvoice_amount"].to_string());

    return assets;            
}

map<string, asset> hyphadao::get_assets(const uint64_t &role_id, 
                                        const float &deferred_perc, 
                                        const float &time_share_perc) 
{
    map<string, asset> assets;

    // global ratios
    config_table config_s(get_self(), get_self().value);
    Config c = config_s.get_or_create(get_self(), Config());
    float seeds_deferral_coeff = get_float(c.ints, "seeds_deferral_factor_x100");
    float hypha_deferral_coeff = get_float(c.ints, "hypha_deferral_factor_x100");

    object_table o_t_role(get_self(), name("role").value);
    auto o_itr_role = o_t_role.find(role_id);
    checkx(o_itr_role != o_t_role.end(), "Role ID: " + std::to_string(role_id) + " does not exist.");

    // calculate HUSD salary amount
    // 1. normalize annual salary to the time commitment of this proposal
    // 2. multiply (1) by 0.02026 to calculate a single moon phase; avg. phase is 7.4 days, 49.36 phases per year
    // 3. multiply (2) by 1 - deferral perc
    asset commitment_adjusted_usd_annual = adjust_asset(o_itr_role->assets.at("annual_usd_salary"), time_share_perc);
    asset commitment_adjusted_usd_phase = adjust_asset(commitment_adjusted_usd_annual, common::PHASE_TO_YEAR_RATIO);
    asset nondeferred_and_commitment_adj_usd_phase = adjust_asset(commitment_adjusted_usd_phase, (float)1 - (float)deferred_perc);

    // convert symbol
    asset husd_salary_per_phase = asset{nondeferred_and_commitment_adj_usd_phase.amount, common::S_HUSD};

    // calculate HYPHA phase salary amount
    asset deferred_and_commitment_adj_usd_phase = adjust_asset(commitment_adjusted_usd_phase, (float)deferred_perc);
    assets["hypha_salary_per_phase"] = adjust_asset(asset{deferred_and_commitment_adj_usd_phase.amount, common::S_HYPHA}, hypha_deferral_coeff);

    // calculate HVOICE phase salary amount, which is $1.00 USD == 2 HVOICE
    assets["hvoice_salary_per_phase"] = asset{commitment_adjusted_usd_phase.amount * 2, common::S_HVOICE};

    // calculate Seeds escrow amount
    float seeds_escrow_ratio = seeds_deferral_coeff * deferred_perc;
    asset commitment_adjusted_seeds_phase = adjust_asset(asset{commitment_adjusted_usd_phase.amount * 100, common::S_SEEDS}, (float)1 / (float)get_seeds_price_usd());
    asset seeds_escrow_salary_per_phase = adjust_asset(commitment_adjusted_seeds_phase, seeds_escrow_ratio);
 
    assets["husd_salary_per_phase"] = husd_salary_per_phase;
    assets["seeds_escrow_salary_per_phase"] = seeds_escrow_salary_per_phase;

    debug("Calculations for get_assets: Role: " + std::to_string(role_id) +
            ", deferred_perc: " + std::to_string(deferred_perc) +
            ", time_share_perc: " + std::to_string(time_share_perc) +
            ", seeds_escrow_ratio: " + std::to_string(seeds_escrow_ratio) +
            ", hypha_salary_per_phase: " + assets["hypha_salary_per_phase"].to_string() +
            ", seeds_escrow_salary_per_phase: " + assets["seeds_escrow_salary_per_phase"].to_string() +
            ", husd_salary_per_phase: " + assets["husd_salary_per_phase"].to_string() +
            ", hvoice_salary_per_phase: " + assets["hvoice_salary_per_phase"].to_string());

    return assets;
}

void hyphadao::new_proposal(const map<string, name> &names,
                            const map<string, string> &strings,
                            const map<string, asset> &assets,
                            const map<string, time_point> &time_points,
                            const map<string, uint64_t> &ints,
                            const map<string, transaction> &trxs)
{
    object_table o_t(get_self(), name("proposal").value);
    o_t.emplace(get_self(), [&](auto &o) {
        o.id = o_t.available_primary_key();
        o.names = names;
        o.strings = strings;
        o.assets = assets;
        o.time_points = time_points;
        o.ints = ints;
        o.trxs = trxs;

        config_table config_s(get_self(), get_self().value);
        Config c = config_s.get_or_create(get_self(), Config());
        o.strings["client_version"] = get_string(c.strings, "client_version");
        o.strings["contract_version"] = get_string(c.strings, "contract_version");

        check(names.find("type") != names.end(), "Name value with the key of 'type' is required for proposals.");
        name proposal_type = names.at("type");

        o.names["ballot_id"] = register_ballot(names.at("owner"), strings);

        /* default trx_action_account to hyphadaomain */
        if (names.find("trx_action_contract") == names.end())
        {
            o.names["trx_action_contract"] = get_self();
        }

        if (names.find("trx_action_name") != names.end())
        { // this transaction executes if the proposal passes
            transaction trx(time_point_sec(current_time_point()) + (60 * 60 * 24 * 35));
            trx.actions.emplace_back(
                permission_level{get_self(), name("active")},
                o.names.at("trx_action_contract"), o.names.at("trx_action_name"),
                std::make_tuple(o.id));
            trx.delay_sec = 0;
            o.trxs["exec_on_approval"] = trx;
        }

        // input cleaning and calculations for the various proposal types
        if (proposal_type == name("role"))
        {
            // role logic/business rules
            check(ints.at("fulltime_capacity_x100") > 0, "fulltime_capacity_x100 must be greater than zero. You submitted: " + std::to_string(ints.at("fulltime_capacity_x100")));
            check(assets.at("annual_usd_salary").amount > 0, "annual_usd_salary must be greater than zero. You submitted: " + assets.at("annual_usd_salary").to_string());
        }
        else if (proposal_type == name("assignment"))
        {
            checkx(ints.find("role_id") != ints.end(), "Role ID is required when type is assignment.");
            uint64_t role_id = ints.at("role_id");
            
            object_table o_t_role(get_self(), name("role").value);
            auto o_itr_role = o_t_role.find(role_id);
            checkx(o_itr_role != o_t_role.end(), "Role ID: " + std::to_string(role_id) + " does not exist.");

            checkx(ints.find("time_share_x100") != ints.end(), "time_share_x100 is a required field for assignment proposals.");
            checkx(ints.at("time_share_x100") > 0 && ints.at("time_share_x100") <= 10000, "time_share_x100 must be greater than zero and less than or equal to 100.");
            checkx(ints.find("start_period") != ints.end(), "start_period is a required field for assignment proposals.");
            checkx(ints.find("end_period") != ints.end(), "end_period is a required field for assignment proposals.");
            o.ints["fk"] = role_id;

            // assignment proposal time_share is greater that or equal role minimum
            check(ints.at("time_share_x100") >= o_itr_role->ints.at("min_time_share_x100"), "Role ID: " +
                std::to_string(role_id) + " has a minimum commitment % (x100) of " + std::to_string(o_itr_role->ints.at("min_time_share_x100")) +
                "; proposal requests commitment % (x100) of: " + std::to_string(ints.at("time_share_x100")));

            // assignment proposal deferred pay % is greater that or equal role minimum
            check(ints.at("deferred_perc_x100") >= o_itr_role->ints.at("min_deferred_x100"), "Role ID: " +
                std::to_string(role_id) + " has a minimum deferred pay % (x100) of " + std::to_string(o_itr_role->ints.at("min_deferred_x100")));

            // merge calculated assets into map
            map<string, asset> calculated_assets = get_assets(role_id, get_float(ints, "deferred_perc_x100"), get_float(ints, "time_share_x100"));
            std::map<string, asset>::const_iterator asset_itr;
            for (asset_itr = calculated_assets.begin(); asset_itr != calculated_assets.end(); ++asset_itr) {
                o.assets[asset_itr->first] = asset_itr->second;
            }
        }
        else if (proposal_type == name("payout"))
        {
            if (assets.find("usd_amount") != assets.end())
            {
                // using USD amount + configured parameters
                asset usd_amount = assets.at("usd_amount");

                // merge calculated assets into map
                map<string, asset> calculated_assets = get_assets(usd_amount, get_float(ints, "deferred_perc_x100"));
                std::map<string, asset>::const_iterator asset_itr;
                for (asset_itr = calculated_assets.begin(); asset_itr != calculated_assets.end(); ++asset_itr) {
                    o.assets[asset_itr->first] = asset_itr->second;
                }
            }
            else
            {
                // Advanced mode - using the pre-calculated values
                // translate seeds_amount (from prior version to escrow seeds)
                o.assets = assets;
                if (assets.find("seeds_amount") != assets.end())
                {
                    o.assets["seeds_escrow_amount"] = assets.at("seeds_amount");
                    o.assets.erase(o.assets.find("seeds_amount"));
                }
            }
        }
        else if (proposal_type == name("edit") && names.at("original_scope") == name("assignment")) 
        {
            // if editing an assignment proposal, we need to recalculate the token quantities
            uint64_t assignment_id = ints.at("original_object_id");
            object_table o_t(get_self(), name("assignment").value);
            auto o_itr = o_t.find(assignment_id);
            check(o_itr != o_t.end(), "Assignment ID: " + std::to_string(assignment_id) + " does not exist.");

            uint64_t role_id = o_itr->ints.at("role_id");
            object_table o_t_role(get_self(), name("role").value);
            auto o_itr_role = o_t_role.find(role_id);
            check(o_itr_role != o_t_role.end(), "Role ID: " + std::to_string(role_id) + " does not exist.");

            // assignment proposal time_share is greater that or equal role minimum
            check(ints.at("time_share_x100") >= o_itr_role->ints.at("min_time_share_x100"), "Role ID: " +
                std::to_string(role_id) + " has a minimum commitment % (x100) of " + std::to_string(o_itr_role->ints.at("min_time_share_x100")) +
                "; proposal requests commitment % (x100) of: " + std::to_string(ints.at("time_share_x100")));

            // assignment proposal deferred pay % is greater that or equal role minimum
            check(ints.at("deferred_perc_x100") >= o_itr_role->ints.at("min_deferred_x100"), "Role ID: " +
                std::to_string(role_id) + " has a minimum deferred pay % (x100) of " + std::to_string(o_itr_role->ints.at("min_deferred_x100")));

            // merge calculated assets into map
            map<string, asset> calculated_assets = get_assets(role_id, get_float(ints, "deferred_perc_x100"), get_float(ints, "time_share_x100"));
            std::map<string, asset>::const_iterator asset_itr;
            for (asset_itr = calculated_assets.begin(); asset_itr != calculated_assets.end(); ++asset_itr) {
                o.assets[asset_itr->first] = asset_itr->second;
            }
        }
        event (name("high"), variant_helper(o.names, o.strings, o.assets, o.time_points, o.ints));
    });
}