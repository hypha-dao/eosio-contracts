#include <hyphadao.hpp>

using namespace hyphaspace;

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

        string debug_str = "";

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
        else if (proposal_type == name("assignment") || proposal_type == name("payout"))
        {
            // global ratios
            configtables c_t(name("tlosto.seeds"), name("tlosto.seeds").value);
            configtable config_t = c_t.get();

            float seeds_price_usd = (float)1 / ((float) config_t.seeds_per_usd.amount / (float)10000); // get_float(c.ints, "seeds_usd_valuation_x100");
            float seeds_deferral_coeff = get_float(c.ints, "seeds_deferral_factor_x100");
            float hypha_deferral_coeff = get_float(c.ints, "hypha_deferral_factor_x100");
            // debug_str = debug_str + "Globals: seeds_per_usd: " + config_t.seeds_per_usd.to_string() + ", seeds_price_usd: " + std::to_string(seeds_price_usd) + ", seeds_deferral_coeff: " +
            //             std::to_string(seeds_deferral_coeff) + ", hypha_deferral_coeff: " + std::to_string(hypha_deferral_coeff) + ". ";

            float deferred_perc = get_float(ints, "deferred_perc_x100");
            float instant_husd_perc = get_float(ints, "instant_husd_perc_x100");
            if (deferred_perc == 1)
            {
                check(instant_husd_perc == 0, "HUSD percentage must be 0 if deferred percentage is 100%; Your proposal: deferred percentage is " + std::to_string(deferred_perc) + ", HUSD percentage is " + std::to_string(instant_husd_perc));
            }

            float hypha_ratio = (float)hypha_deferral_coeff * (float)deferred_perc;
            float seeds_escrow_ratio = seeds_deferral_coeff * deferred_perc;
            debug_str = debug_str + "Assignment/Payout: " +
                        ", deferred_perc: " + std::to_string(deferred_perc) +
                        ", hypha_ratio: " + std::to_string(hypha_ratio) +
                        ", seeds_escrow_ratio: " + std::to_string(seeds_escrow_ratio) +
                        ", instant_husd_perc: " + std::to_string(instant_husd_perc) + ". ";

            if (proposal_type == name("assignment"))
            {
                checkx(ints.find("role_id") != ints.end(), "Role ID is required when type is assignment.");
                checkx(ints.find("time_share_x100") != ints.end(), "time_share_x100 is a required field for assignment proposals.");
                checkx(ints.at("time_share_x100") > 0 && ints.at("time_share_x100") <= 10000, "time_share_x100 must be greater than zero and less than or equal to 100.");
                checkx(ints.find("start_period") != ints.end(), "start_period is a required field for assignment proposals.");
                checkx(ints.find("end_period") != ints.end(), "end_period is a required field for assignment proposals.");
                o.ints["fk"] = ints.at("role_id");

                object_table o_t_role(get_self(), name("role").value);
                auto o_itr_role = o_t_role.find(ints.at("role_id"));
                checkx(o_itr_role != o_t_role.end(), "Role ID: " + std::to_string(ints.at("role_id")) + " does not exist.");

                // role has enough remaining capacity
                // check_capacity(ints.at("role_id"), ints.at("time_share_x100"));

                // assignment proposal time_share is greater that or equal role minimum
                check(ints.at("time_share_x100") >= o_itr_role->ints.at("min_time_share_x100"), "Role ID: " +
                                                                                                    std::to_string(ints.at("role_id")) + " has a minimum commitment % (x100) of " + std::to_string(o_itr_role->ints.at("min_time_share_x100")) +
                                                                                                    "; proposal requests commitment % (x100) of: " + std::to_string(ints.at("time_share_x100")));

                // assignment proposal deferred pay % is greater that or equal role minimum
                check(ints.at("deferred_perc_x100") >= o_itr_role->ints.at("min_deferred_x100"), "Role ID: " +
                                                                                                     std::to_string(ints.at("role_id")) + " has a minimum deferred pay % (x100) of " + std::to_string(o_itr_role->ints.at("min_deferred_x100")) +
                                                                                                     "; proposal requests deferred % (x100) of: " + std::to_string(ints.at("deferred_perc_The x100")));

                // assignment ratios
                float time_share_perc = get_float(ints, "time_share_x100");

                debug_str = debug_str + "Assignment: time_share_perc: " + std::to_string(time_share_perc) + ", deferred_perc: " +
                            std::to_string(deferred_perc) + ", instant_husd_perc: " + std::to_string(instant_husd_perc) + ". ";

                // calculate HUSD salary amount
                // 1. normalize annual salary to the time commitment of this proposal
                // 2. multiply (1) by 0.02026 to calculate a single moon phase; avg. phase is 7.4 days, 49.36 phases per year
                // 3. multiply (2) by HUSD percent requested on this assignment proposal
                asset time_share_usd_annual = adjust_asset(o_itr_role->assets.at("annual_usd_salary"), time_share_perc);
                asset phase_usd_equiv = adjust_asset(time_share_usd_annual, common::PHASE_TO_YEAR_RATIO);
                asset phase_usd_salary = adjust_asset(phase_usd_equiv, (float)instant_husd_perc * ((float)1 - (float)deferred_perc));
                asset phase_husd_salary = asset{phase_usd_salary.amount, common::S_HUSD}; // convert symbol
                o.assets["husd_salary_per_phase"] = phase_husd_salary;

                debug_str = debug_str + "Calcs: time_share_usd_annual: " + time_share_usd_annual.to_string() + ", phase_usd_equiv: " +
                            phase_usd_equiv.to_string() + ", phase_husd_salary: " + phase_husd_salary.to_string() + ". ";

                //calculate HYPHA phase salary amount
                // float hypha_ratio = (float) hypha_deferral_coeff * (float) deferred_perc;
                o.assets["hypha_salary_per_phase"] = adjust_asset(asset{phase_usd_equiv.amount, common::S_HYPHA}, hypha_ratio);

                // calculate HVOICE phase salary amount, which is $1.00 USD == 1 HVOICE
                o.assets["hvoice_salary_per_phase"] = asset{phase_usd_equiv.amount * 2, common::S_HVOICE};

                // calculate instant SEEDS phase salary amount
                // 1. calculate amount of seeds based on the configured seeds price
                // 2. calculated deferred ratio as the deferral factor (1.3) * the deferred % of this assignment
                // 3. calculated the seeds to go to escrow each phase
                asset phase_seeds_equiv = adjust_asset(asset{phase_usd_equiv.amount * 100, common::S_SEEDS}, (float)1 / (float)seeds_price_usd);
                asset seeds_escrow_phase_salary = adjust_asset(phase_seeds_equiv, seeds_escrow_ratio);
                asset seeds_instant_phase_salary = adjust_asset(phase_seeds_equiv, (float)(1 - deferred_perc) * (float)(1 - instant_husd_perc));
                debug_str = debug_str + "Seeds calcs: phase_seeds_equiv: " + phase_seeds_equiv.to_string() + ", seeds_escrow_phase_salary: " +
                            seeds_escrow_phase_salary.to_string() + ", seeds_instant_phase_salary: " + seeds_instant_phase_salary.to_string() + ". ";

                o.assets["seeds_escrow_salary_per_phase"] = seeds_escrow_phase_salary;
                o.assets["seeds_instant_salary_per_phase"] = seeds_instant_phase_salary;
            }
            else if (proposal_type == name("payout"))
            {
                if (assets.find("usd_amount") != assets.end())
                {
                    // using USD amount + configured parameters
                    asset usd_amount = assets.at("usd_amount");

                    //calculate HYPHA amount
                    o.assets["hypha_amount"] = adjust_asset(asset{usd_amount.amount, common::S_HYPHA}, hypha_ratio);
                    o.assets["hvoice_amount"] = asset{usd_amount.amount, common::S_HVOICE};
                    asset husd_amount = adjust_asset(asset{usd_amount.amount, common::S_HUSD},
                                                     (float)((float)instant_husd_perc) * ((float)1 - (float)deferred_perc));
                    o.assets["husd_amount"] = husd_amount;

                    asset seeds_equiv_amount = adjust_asset(asset{usd_amount.amount * 100, common::S_SEEDS}, (float)1 / (float)seeds_price_usd);
                    asset seeds_escrow_amount = adjust_asset(seeds_equiv_amount, seeds_escrow_ratio);
                    asset seeds_instant_amount = adjust_asset(seeds_equiv_amount, (float)(1 - deferred_perc) * (float)(1 - instant_husd_perc));
                    debug_str = debug_str + "Payout: " +
                                ", husd_amount: " + husd_amount.to_string() +
                                ", hypha_amount: " + o.assets["hypha_amount"].to_string() +
                                ", hvoice_amount: " + o.assets["hvoice_amount"].to_string() +
                                ", seeds_equiv_amount: " + seeds_equiv_amount.to_string() +
                                ", seeds_escrow_amount: " + seeds_escrow_amount.to_string() +
                                ", seeds_instant_amount: " + seeds_instant_amount.to_string() + ". ";

                    o.assets["seeds_escrow_amount"] = seeds_escrow_amount;
                    o.assets["seeds_instant_amount"] = seeds_instant_amount;
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
        }
        debug(debug_str);
    });
}