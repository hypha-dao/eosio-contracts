#include <hyphadao.hpp>

using namespace hypha;

void hyphadao::new_proposal(const name &owner,
                            map<string, name> &names,
                            map<string, string> &strings,
                            map<string, asset> &assets,
                            map<string, time_point> &time_points,
                            map<string, uint64_t> &ints,
                            map<string, transaction> &trxs)
{
    // hyphadao::object_table o_t(get_self(), name("proposal").value);
    // config_table config_s(get_self(), get_self().value);
    // Config c = config_s.get_or_create(get_self(), Config());
    // strings["client_version"] = get_string(c.strings, "client_version");
    // strings["contract_version"] = get_string(c.strings, "contract_version");

    // check(names.find("type") != names.end(), "Name value with the key of 'type' is required for proposals.");
    // name proposal_type = names.at("type");

    // names["ballot_id"] = register_ballot(names.at("owner"), strings);

    // /* default trx_action_account to hyphadaomain */
    // if (names.find("trx_action_contract") == names.end())
    // {
    //     names["trx_action_contract"] = get_self();
    // }

    // if (names.find("trx_action_name") != names.end())
    // { // this transaction executes if the proposal passes
    //     transaction trx(time_point_sec(current_time_point()) + (60 * 60 * 24 * 35));
    //     trx.actions.emplace_back(
    //         permission_level{get_self(), name("active")},
    //         names.at("trx_action_contract"), names.at("trx_action_name"),
    //         std::make_tuple(o_t.available_primary_key()));
    //     trx.delay_sec = 0;
    //     trxs["exec_on_approval"] = trx;
    // }

    // // input cleaning and calculations for the various proposal types
    // if (proposal_type == name("role"))
    // {
    //     // role logic/business rules
    //     check(ints.at("fulltime_capacity_x100") > 0, "fulltime_capacity_x100 must be greater than zero. You submitted: " + std::to_string(ints.at("fulltime_capacity_x100")));
    //     check(assets.at("annual_usd_salary").amount > 0, "annual_usd_salary must be greater than zero. You submitted: " + assets.at("annual_usd_salary").to_string());
    // }
    // else if (proposal_type == name("assignment"))
    // {
    //     checkx(ints.find("role_id") != ints.end(), "Role ID is required when type is assignment.");
    //     uint64_t role_id = ints.at("role_id");

    //     object_table o_t_role(get_self(), name("role").value);
    //     auto o_itr_role = o_t_role.find(role_id);
    //     checkx(o_itr_role != o_t_role.end(), "Role ID: " + std::to_string(role_id) + " does not exist.");

    //     checkx(ints.find("time_share_x100") != ints.end(), "time_share_x100 is a required field for assignment proposals.");
    //     checkx(ints.at("time_share_x100") > 0 && ints.at("time_share_x100") <= 10000, "time_share_x100 must be greater than zero and less than or equal to 100.");
    //     checkx(ints.find("start_period") != ints.end(), "start_period is a required field for assignment proposals.");
    //     checkx(ints.find("end_period") != ints.end(), "end_period is a required field for assignment proposals.");
    //     ints["fk"] = role_id;

    //     // assignment proposal time_share is greater that or equal role minimum
    //     check(ints.at("time_share_x100") >= o_itr_role->ints.at("min_time_share_x100"), "Role ID: " +
    //                                                                                         std::to_string(role_id) + " has a minimum commitment % (x100) of " + std::to_string(o_itr_role->ints.at("min_time_share_x100")) +
    //                                                                                         "; proposal requests commitment % (x100) of: " + std::to_string(ints.at("time_share_x100")));

    //     // assignment proposal deferred pay % is greater that or equal role minimum
    //     check(ints.at("deferred_perc_x100") >= o_itr_role->ints.at("min_deferred_x100"), "Role ID: " +
    //                                                                                          std::to_string(role_id) + " has a minimum deferred pay % (x100) of " + std::to_string(o_itr_role->ints.at("min_deferred_x100")));

    //     // merge calculated assets into map
    //     map<string, asset> calculated_assets = get_assets(role_id, get_float(ints, "deferred_perc_x100"), get_float(ints, "time_share_x100"));
    //     std::map<string, asset>::const_iterator asset_itr;
    //     for (asset_itr = calculated_assets.begin(); asset_itr != calculated_assets.end(); ++asset_itr)
    //     {
    //         assets[asset_itr->first] = asset_itr->second;
    //     }
    // }
    // else if (proposal_type == name("payout"))
    // {
    //     if (assets.find("usd_amount") != assets.end())
    //     {
    //         // using USD amount + configured parameters
    //         asset usd_amount = assets.at("usd_amount");

    //         check(ints.find("end_period") != ints.end(), "End period is required for payout/contribution proposals.");

    //         uint64_t period_id = ints.at("end_period");

    //         period_table period_t(get_self(), get_self().value);
    //         auto p_itr = period_t.find(period_id);
    //         check(p_itr != period_t.end(), "Cannot create proposal. End period ID not found: " + std::to_string(period_id));

    //         // merge calculated assets into map
    //         map<string, asset> calculated_assets = get_assets(usd_amount, get_float(ints, "deferred_perc_x100"), p_itr->end_date);
    //         std::map<string, asset>::const_iterator asset_itr;
    //         for (asset_itr = calculated_assets.begin(); asset_itr != calculated_assets.end(); ++asset_itr)
    //         {
    //             assets[asset_itr->first] = asset_itr->second;
    //         }
    //     }
    //     else
    //     {
    //         // Advanced mode - using the pre-calculated values
    //         // translate seeds_amount (from prior version to escrow seeds)
    //         if (assets.find("seeds_amount") != assets.end())
    //         {
    //             assets["seeds_escrow_amount"] = assets.at("seeds_amount");
    //             assets.erase(assets.find("seeds_amount"));
    //         }
    //     }
    // }
    // else if (proposal_type == name("edit") && names.at("original_scope") == name("assignment"))
    // {
    //     // if editing an assignment proposal, we need to recalculate the token quantities
    //     uint64_t assignment_id = ints.at("original_object_id");
    //     object_table o_t(get_self(), name("assignment").value);
    //     auto o_itr = o_t.find(assignment_id);
    //     check(o_itr != o_t.end(), "Assignment ID: " + std::to_string(assignment_id) + " does not exist.");

    //     uint64_t role_id = o_itr->ints.at("role_id");
    //     object_table o_t_role(get_self(), name("role").value);
    //     auto o_itr_role = o_t_role.find(role_id);
    //     check(o_itr_role != o_t_role.end(), "Role ID: " + std::to_string(role_id) + " does not exist.");

    //     // assignment proposal time_share is greater that or equal role minimum
    //     check(ints.at("time_share_x100") >= o_itr_role->ints.at("min_time_share_x100"), "Role ID: " +
    //                                                                                         std::to_string(role_id) + " has a minimum commitment % (x100) of " + std::to_string(o_itr_role->ints.at("min_time_share_x100")) +
    //                                                                                         "; proposal requests commitment % (x100) of: " + std::to_string(ints.at("time_share_x100")));

    //     // assignment proposal deferred pay % is greater that or equal role minimum
    //     check(ints.at("deferred_perc_x100") >= o_itr_role->ints.at("min_deferred_x100"), "Role ID: " +
    //                                                                                          std::to_string(role_id) + " has a minimum deferred pay % (x100) of " + std::to_string(o_itr_role->ints.at("min_deferred_x100")));

    //     // merge calculated assets into map
    //     map<string, asset> calculated_assets = get_assets(role_id, get_float(ints, "deferred_perc_x100"), get_float(ints, "time_share_x100"));
    //     std::map<string, asset>::const_iterator asset_itr;
    //     for (asset_itr = calculated_assets.begin(); asset_itr != calculated_assets.end(); ++asset_itr)
    //     {
    //         assets[asset_itr->first] = asset_itr->second;
    //     }
    // }
    // new_object(owner, name("proposal"), names, strings, assets, time_points, ints, trxs);
    // event(name("high"), variant_helper(names, strings, assets, time_points, ints));
}
