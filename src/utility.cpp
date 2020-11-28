#include <hyphadao.hpp>

using namespace hypha;

void hyphadao::event(const name &level,
					 const map<string, hyphadao::flexvalue1> &values)
{

	config_table config_s(get_self(), get_self().value);
	Config c = config_s.get_or_create(get_self(), Config());
	name publisher_contract = c.names.at("publisher_contract");

	action(
		permission_level{get_self(), name("active")},
		publisher_contract, name("event"),
		make_tuple(get_self(), level, values))
		.send();
}

// translates the typed maps to a flexvalue map used by more recent
// code within the smart contract
map<string, hyphadao::flexvalue1> hyphadao::variant_helper(const map<string, name> &names,
														  const map<string, string> &strings,
														  const map<string, asset> &assets,
														  const map<string, time_point> &time_points,
														  const map<string, uint64_t> &ints)
{

	map<string, hyphadao::flexvalue1> flexvalues;

	map<string, name>::const_iterator name_itr;
	for (name_itr = names.begin(); name_itr != names.end(); ++name_itr)
	{
		flexvalues[name_itr->first] = name_itr->second;
	}

	map<string, string>::const_iterator string_itr;
	for (string_itr = strings.begin(); string_itr != strings.end(); ++string_itr)
	{
		flexvalues[string_itr->first] = string_itr->second.substr(0, std::min(string_itr->second.length(), size_t(150)));
	}

	map<string, asset>::const_iterator asset_itr;
	for (asset_itr = assets.begin(); asset_itr != assets.end(); ++asset_itr)
	{
		flexvalues[asset_itr->first] = asset_itr->second;
	}

	map<string, time_point>::const_iterator time_point_itr;
	for (time_point_itr = time_points.begin(); time_point_itr != time_points.end(); ++time_point_itr)
	{
		flexvalues[time_point_itr->first] = time_point_itr->second;
	}

	map<string, uint64_t>::const_iterator int_itr;
	for (int_itr = ints.begin(); int_itr != ints.end(); ++int_itr)
	{
		flexvalues[int_itr->first] = int_itr->second;
	}
	return flexvalues;
}

void hyphadao::clrdebugs(const uint64_t &starting_id, const uint64_t &batch_size)
{
	check(has_auth(name("hyphanewyork")) || has_auth(get_self()), "Requires higher permission."); // TODO: remove hyphanewyork
	debug_table d_t(get_self(), get_self().value);
	auto d_itr = d_t.find(starting_id);

	while (d_itr->debug_id <= starting_id + batch_size)
	{
		d_itr = d_t.erase(d_itr);
	}

	eosio::transaction out{};
	out.actions.emplace_back(permission_level{get_self(), name("active")},
							 get_self(), name("clrdebugs"),
							 make_tuple(d_itr->debug_id, batch_size));
	out.delay_sec = 1;
	out.send(get_next_sender_id(), get_self());
}

void hyphadao::resetperiods()
{
	require_auth(get_self());
	reset_periods();
}

void hyphadao::debugmsg(const string &message)
{
	require_auth(get_self());
	debug(message);
}

void hyphadao::debug(const string &notes)
{
	debug_table d_t(get_self(), get_self().value);
	d_t.emplace(get_self(), [&](auto &d) {
		d.debug_id = d_t.available_primary_key();
		d.notes = notes;
	});
}

uint64_t hyphadao::get_next_sender_id()
{
	config_table config_s(get_self(), get_self().value);
	Config c = config_s.get_or_create(get_self(), Config());
	uint64_t return_senderid = c.ints.at("last_sender_id");
	return_senderid++;
	c.ints["last_sender_id"] = return_senderid;
	config_s.set(c, get_self());
	return return_senderid;
}

void hyphadao::remperiods(const uint64_t &begin_period_id,
						  const uint64_t &end_period_id)
{
	require_auth(get_self());
	remove_periods(begin_period_id, end_period_id);
}

asset hyphadao::adjust_asset(const asset &original_asset, const float &adjustment)
{
	return asset{static_cast<int64_t>(original_asset.amount * adjustment), original_asset.symbol};
}

float hyphadao::get_float(const map<string, uint64_t> ints, string key)
{
	return (float)ints.at(key) / (float)100;
}

bool hyphadao::is_paused()
{
	config_table config_s(get_self(), get_self().value);
	Config c = config_s.get_or_create(get_self(), Config());
	check(c.ints.find("paused") != c.ints.end(), "Contract does not have a pause configuration. Assuming it is paused. Please contact administrator.");

	uint64_t paused = c.ints.at("paused");
	return paused == 1;
}

string hyphadao::get_string(const map<string, string> strings, string key)
{
	if (strings.find(key) != strings.end())
	{
		return strings.at(key);
	}
	else
	{
		return string{""};
	}
}

void hyphadao::debugx(const string &message)
{
	transaction trx(time_point_sec(current_time_point()) + (60));
	trx.actions.emplace_back(
		permission_level{get_self(), name("active")},
		get_self(), name("debugmsg"),
		make_tuple(message));
	trx.delay_sec = 0;
	trx.send(get_next_sender_id(), get_self());
}

void hyphadao::checkx(const bool &condition, const string &message)
{

	if (condition)
	{
		return;
	}

	transaction trx(time_point_sec(current_time_point()) + (60));
	trx.actions.emplace_back(
		permission_level{get_self(), name("active")},
		get_self(), name("debugmsg"),
		make_tuple(message));
	trx.delay_sec = 0;
	trx.send(get_next_sender_id(), get_self());

	check(false, message);
}

void hyphadao::check_capacity(const uint64_t &role_id, const uint64_t &req_time_share_x100)
{
	// Ensure that this proposal would not push the role over it's approved full time capacity
	object_table o_t_role(get_self(), name("role").value);
	auto o_itr_role = o_t_role.find(role_id);
	checkx(o_itr_role != o_t_role.end(), "Role ID: " + to_string(role_id) + " does not exist.");
	int role_capacity = o_itr_role->ints.at("fulltime_capacity_x100");

	object_table o_t_assignment(get_self(), name("assignment").value);
	auto sorted_by_role = o_t_assignment.get_index<name("byfk")>();
	auto a_itr_by_role = sorted_by_role.find(role_id);
	int consumed_capacity = 0;
	debug("Role capacity: " + to_string(role_capacity) + ", fk: " +
		  to_string(a_itr_by_role->ints.at("fk")) + "; Role ID: " + to_string(role_id));
	while (a_itr_by_role != sorted_by_role.end() && a_itr_by_role->ints.at("fk") == role_id)
	{
		consumed_capacity += a_itr_by_role->ints.at("time_share_x100");
		a_itr_by_role++;
	}

	checkx(consumed_capacity + req_time_share_x100 <= role_capacity, "Role ID: " +
																		 to_string(role_id) + " cannot support assignment. Full time capacity (x100) is " + to_string(role_capacity) +
																		 " and consumed capacity (x100) is " + to_string(consumed_capacity) + "; proposal requests time share (x100) of: " + to_string(req_time_share_x100));
}

void hyphadao::erasedochash (const checksum256 &doc) 
{
	require_auth (get_self());
	m_documentGraph.eraseDocument(doc);
}

void hyphadao::erasedocbyid (const uint64_t &id) 
{
	require_auth (get_self());
	Document::document_table d_t (get_self(), get_self().value);
	auto d_itr = d_t.find (id);
	check (d_itr != d_t.end(), "Document with ID is not found: " + std::to_string(id));
	d_t.erase (d_itr);
}

void hyphadao::eraseedges (const string &notes) 
{
	require_auth (get_self());
	Edge::edge_table e_t (get_self(), get_self().value);
	auto e_itr = e_t.begin();
	while (e_itr != e_t.end()) {
		e_itr = e_t.erase (e_itr);
	}
}

void hyphadao::erasedocs(const uint64_t &begin_id, const uint64_t &batch_size) 
{
	require_auth(get_self());
	Document::document_table d_t (get_self(), get_self().value);
	auto d_itr = d_t.find (begin_id);
	check (d_itr != d_t.end(), "begin_id not found: " + std::to_string(begin_id));
	
	int i = 0;
	while (d_itr != d_t.end() && i < batch_size) {
		d_itr = d_t.erase(d_itr);		
		i++;
	}

	if (d_itr != d_t.end()) {
		// d_itr++;
		eosio::transaction out{};
		out.actions.emplace_back(permission_level{get_self(), name("active")},
								get_self(), name("erasedocs"),
			std::make_tuple(d_itr->id, batch_size));
		out.delay_sec = 1;
		out.send(get_next_sender_id(), get_self());
	}
}

void hyphadao::erasealldocs (const string &notes)
{
	require_auth(get_self());
	Document::document_table d_t (get_self(), get_self().value);
	auto d_itr = d_t.begin ();
	while (d_itr != d_t.end()) {

		eosio::transaction out{};
		out.actions.emplace_back(permission_level{get_self(), name("active")},
								get_self(), name("erasedochash"),
			std::make_tuple(d_itr->hash));
		out.delay_sec = 1;
		out.send(get_next_sender_id(), get_self());

		d_itr++;
	}
}

// void hyphadao::reset()
//     {
//         require_auth(contract);

//         auto pay_itr = payment_t.begin();
//         while (pay_itr != payment_t.end())
//         {
//             pay_itr = payment_t.erase(pay_itr);
//         }
//     }

// ONLY DEV
void hyphadao::remove_periods(const uint64_t &begin_period_id,
                              const uint64_t &end_period_id)
{
    require_auth(get_self());
    period_table period_t(get_self(), get_self().value);
    auto p_itr = period_t.find(begin_period_id);
    check(p_itr != period_t.end(), "Begin period ID not found: " + std::to_string(begin_period_id));

    while (p_itr->period_id <= end_period_id)
    {
        p_itr = period_t.erase(p_itr);
    }
}

// ONLY DEV
void hyphadao::reset_periods()
{
    require_auth(get_self());
    period_table period_t(get_self(), get_self().value);
    auto per_itr = period_t.begin();
    while (per_itr != period_t.end())
    {
        per_itr = period_t.erase(per_itr);
    }
}

void hyphadao::addperiod(const time_point &start_date, const time_point &end_date, const string &phase)
{
    require_auth(get_self());
    period_table period_t(get_self(), get_self().value);
    period_t.emplace(get_self(), [&](auto &p) {
        p.period_id = period_t.available_primary_key();
        p.start_date = start_date;
        p.end_date = end_date;
        p.phase = phase;
    });
}


uint64_t hyphadao::get_last_period_id()
{
    period_table p_t(get_self(), get_self().value);
    auto p_itr = p_t.begin();
    while (p_itr != p_t.end())
    {
        if (p_itr->start_date <= current_time_point() && p_itr->end_date >= current_time_point())
        {
            return p_itr->period_id - 1;
        }
        p_itr++;
    }
    check(false, "FATAL ERROR: Last period ID is not found, perhaps periods table is empty.");
    return 0;
}

bool hyphadao::holds_hypha(const name &account)
{
    config_table config_s(get_self(), get_self().value);
    Config c = config_s.get_or_create(get_self(), Config());

    eosiotoken::accounts a_t(c.names.at("hypha_token_contract"), account.value);
    auto a_itr = a_t.find(common::S_HYPHA.code().raw());
    if (a_itr == a_t.end())
    {
        return false;
    }
    else if (a_itr->balance.amount > 0)
    {
        return true;
    }
    else
    {
        return false;
    }
}


// void hyphadao::reset()
// {
// 	// bank.reset_config ();
// 	require_auth(get_self());
// 	// config_table      config_s (get_self(), get_self().value);
// 	// Config c = config_s.get_or_create (get_self(), Config());
// 	// config_s.remove ();
// }
// void hyphadao::erasebackups (const name& scope) {
// 	check ( is_paused(), "Contract must be paused to call this action.");

// 	backup_object_table o_t_backup (get_self(), scope.value);
// 	auto o_itr = o_t_backup.begin();
// 	while (o_itr != o_t_backup.end()) {
// 		o_itr = o_t_backup.erase (o_itr);
// 	}
// }

// void hyphadao::backupobjs (const name& scope) {
// 	check ( is_paused(), "Contract must be paused to call this action.");

// 	object_table o_t (get_self(), scope.value);
// 	backup_object_table o_t_backup (get_self(), scope.value);

// 	auto o_itr = o_t.begin();
// 	while (o_itr != o_t.end()) {
// 		o_t_backup.emplace (get_self(), [&](auto &o) {
// 			o.id                          = o_itr->id;
// 			o.names                       = o_itr->names;
// 			o.assets                      = o_itr->assets;
// 			o.strings                     = o_itr->strings;
// 			o.floats                      = o_itr->floats;
// 			o.time_points                 = o_itr->time_points;
// 			o.ints                        = o_itr->ints;
// 			o.trxs                        = o_itr->trxs;
// 		});
// 		o_itr = o_t.erase (o_itr);
// 	}
// }

// void hyphadao::restoreobjs (const name& scope) {
// 	check ( is_paused(), "Contract must be paused to call this action.");

// 	object_table o_t (get_self(), scope.value);
// 	backup_object_table o_t_backup (get_self(), scope.value);

// 	auto o_itr_backup = o_t_backup.begin();
// 	while (o_itr_backup != o_t_backup.end()) {
// 		o_t.emplace (get_self(), [&](auto &o) {
// 			o.id                          = o_itr_backup->id;
// 			o.names                       = o_itr_backup->names;
// 			o.assets                      = o_itr_backup->assets;
// 			o.strings                     = o_itr_backup->strings;
// 			o.floats                      = o_itr_backup->floats;
// 			o.time_points                 = o_itr_backup->time_points;
// 			o.ints                        = o_itr_backup->ints;
// 			o.trxs                        = o_itr_backup->trxs;
// 		});
// 		o_itr_backup++;
// 	}
// }