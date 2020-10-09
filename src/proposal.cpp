#include <hyphadao.hpp>

using namespace hyphaspace;

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

void hyphadao::new_proposal(const name& owner,
                            map<string, name> &names,
                            map<string, string> &strings,
                            map<string, asset> &assets,
                            map<string, time_point> &time_points,
                            map<string, uint64_t> &ints,
                            map<string, transaction> &trxs)
{
    hyphadao::object_table o_t(get_self(), name("proposal").value);
    config_table config_s(get_self(), get_self().value);
    Config c = config_s.get_or_create(get_self(), Config());
    strings["client_version"] = get_string(c.strings, "client_version");
    strings["contract_version"] = get_string(c.strings, "contract_version");

    check(names.find("type") != names.end(), "Name value with the key of 'type' is required for proposals.");
    name proposal_type = names.at("type");

    names["ballot_id"] = register_ballot(names.at("owner"), strings);

    /* default trx_action_account to hyphadaomain */
    if (names.find("trx_action_contract") == names.end())
    {
        names["trx_action_contract"] = get_self();
    }

    if (names.find("trx_action_name") != names.end())
    { // this transaction executes if the proposal passes
        transaction trx(time_point_sec(current_time_point()) + (60 * 60 * 24 * 35));
        trx.actions.emplace_back(
            permission_level{get_self(), name("active")},
            names.at("trx_action_contract"), names.at("trx_action_name"),
            std::make_tuple(o_t.available_primary_key()));
        trx.delay_sec = 0;
        trxs["exec_on_approval"] = trx;
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
        ints["fk"] = role_id;

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
        for (asset_itr = calculated_assets.begin(); asset_itr != calculated_assets.end(); ++asset_itr)
        {
            assets[asset_itr->first] = asset_itr->second;
        }
    }
    else if (proposal_type == name("payout"))
    {
        if (assets.find("usd_amount") != assets.end())
        {
            // using USD amount + configured parameters
            asset usd_amount = assets.at("usd_amount");

            check(ints.find("end_period") != ints.end(), "End period is required for payout/contribution proposals.");

            uint64_t period_id = ints.at("end_period");
            auto p_itr = bank.period_t.find(period_id);
            check(p_itr != bank.period_t.end(), "Cannot create proposal. End period ID not found: " + std::to_string(period_id));

            // merge calculated assets into map
            map<string, asset> calculated_assets = get_assets(usd_amount, get_float(ints, "deferred_perc_x100"), p_itr->end_date);
            std::map<string, asset>::const_iterator asset_itr;
            for (asset_itr = calculated_assets.begin(); asset_itr != calculated_assets.end(); ++asset_itr)
            {
                assets[asset_itr->first] = asset_itr->second;
            }
        }
        else
        {
            // Advanced mode - using the pre-calculated values
            // translate seeds_amount (from prior version to escrow seeds)
            if (assets.find("seeds_amount") != assets.end())
            {
                assets["seeds_escrow_amount"] = assets.at("seeds_amount");
                assets.erase(assets.find("seeds_amount"));
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
        for (asset_itr = calculated_assets.begin(); asset_itr != calculated_assets.end(); ++asset_itr)
        {
            assets[asset_itr->first] = asset_itr->second;
        }
    }
    new_object(owner, name("proposal"), names, strings, assets, time_points, ints, trxs);
    event(name("high"), variant_helper(names, strings, assets, time_points, ints));
}

void hyphadao::check_coefficient (document_graph::content_group &content_group, const string &coefficient_key)
{
    document_graph::flexvalue coefficient_x10000 = _document_graph.get_content (content_group, coefficient_key, false);
    if (coefficient_x10000 != _document_graph.DOES_NOT_EXIST)
    {
        check(std::holds_alternative<int64_t>(coefficient_x10000), "fatal error: coefficient must be an int64_t type: " + coefficient_key);
        check(std::get<int64_t>(coefficient_x10000) >= 9000 &&  
                std::get<int64_t>(coefficient_x10000) <= 11000, "fatal error: coefficient_x10000 must be between 9000 and 11000, inclusive: " + coefficient_key);
    }   
}

void hyphadao::propose (const name& proposer, const name& proposal_type, std::vector<document_graph::content_group> &content_groups)
{
    // input cleansing
    // switch/case on known types
    // add system content_group

    if (proposal_type == name ("badge")) 
    {
        // enforce required (strict) inputs 
        document_graph::content_group proposal_details = _document_graph.get_content_group (content_groups, "proposal_details", true);
        document_graph::flexvalue title = _document_graph.get_content (proposal_details, "title", true);
        document_graph::flexvalue description = _document_graph.get_content (proposal_details, "description", true);
        document_graph::flexvalue icon = _document_graph.get_content (proposal_details, "icon", true);

        check_coefficient (proposal_details, "husd_coefficient_x10000");
        check_coefficient (proposal_details, "hypha_coefficient_x10000");
        check_coefficient (proposal_details, "hvoice_coefficient_x10000");
        check_coefficient (proposal_details, "seeds_coefficient_x10000");

        // create the system content_group
        config_table config_s(get_self(), get_self().value);
        Config c = config_s.get_or_create(get_self(), Config());
        document_graph::content_group system_cg = document_graph::content_group {};
        system_cg.push_back (_document_graph.new_content("content_group_label", "system"));
        system_cg.push_back (_document_graph.new_content("client_version", get_string(c.strings, "client_version")));
        system_cg.push_back (_document_graph.new_content("contract_version", get_string(c.strings, "contract_version")));
        name ballot_id = register_ballot(proposer, std::get<string>(title), std::get<string>(description), std::get<string>(icon));
        system_cg.push_back (_document_graph.new_content("ballot_id", ballot_id));
        system_cg.push_back (_document_graph.new_content("proposer", proposer));

        auto badge_code = _document_graph.get_content(proposal_details, "badge_code", false);
        if (badge_code == _document_graph.DOES_NOT_EXIST) 
        {
            // add a default badge code
            if (c.names.find("last_badge_code") == c.names.end()) 
            {
                badge_code = name("badge......1");
            } else {
                badge_code = name(c.names.at("last_badge_code").value + 1);
            }
            c.names["last_badge_code"] = std::get<name>(badge_code);
            config_s.set(c, get_self());
        }
           
        system_cg.push_back (_document_graph.new_content ("badge_code", badge_code));
        content_groups.push_back (system_cg);
    }

    document_graph::document proposal_doc = _document_graph.create_document (proposer, content_groups);    
    docindex_table d_t (get_self(), name("proposal").value);
    d_t.emplace (get_self(), [&](auto &d) {
        d.id = d_t.available_primary_key();
        d.document_hash = proposal_doc.hash;
    });
}