#include <hyphadao.hpp>

using namespace hyphaspace;

void hyphadao::updtrxs(const string &note)
{
	require_auth(get_self());

	object_table o_t(get_self(), name("proposal").value);
	auto o_itr = o_t.begin();
	while (o_itr != o_t.end())
	{
		transaction trx(time_point_sec(current_time_point()) + (60 * 60 * 24 * 35));
		trx.actions.emplace_back(
			permission_level{get_self(), "active"_n},
			o_itr->names.at("trx_action_contract"), o_itr->names.at("trx_action_name"),
			std::make_tuple(o_itr->id));
		trx.delay_sec = 0;

		o_t.modify(o_itr, get_self(), [&](auto &o) {
			o.trxs["exec_on_approval"] = trx;
		});
		o_itr++;
	}
}

void hyphadao::updtype(const string &note)
{
	require_auth(get_self());

	object_table o_t(get_self(), name("proposal").value);
	auto o_itr = o_t.begin();
	while (o_itr != o_t.end())
	{
		o_t.modify(o_itr, get_self(), [&](auto &o) {
			o.names["type"] = name("payout"); // o_itr->names.at("proposal_type");
		});
		o_itr++;
	}

	o_t = object_table(get_self(), name("proparchive").value);
	o_itr = o_t.begin();
	while (o_itr != o_t.end())
	{
		o_t.modify(o_itr, get_self(), [&](auto &o) {
			o.names["type"] = name("payout"); // o_itr->names.at("proposal_type");
		});
		o_itr++;
	}
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
							 std::make_tuple(d_itr->debug_id, batch_size));
	out.delay_sec = 1;
	out.send(get_next_sender_id(), get_self());
}

void hyphadao::eraseobj(const name &scope,
						const uint64_t &id)
{
	require_auth(get_self());
	object_table o_t(get_self(), scope.value);
	auto o_itr = o_t.find(id);
	check(o_itr != o_t.end(), "Scope: " + scope.to_string() + "; Object ID: " + std::to_string(id) + " does not exist.");
	o_t.erase(o_itr);
}

void hyphadao::recreate(const name &scope, const uint64_t &id)
{
	require_auth(get_self());
	object_table o_t(get_self(), scope.value);
	auto o_itr = o_t.find(id);
	check(o_itr != o_t.end(), "Proposal not found. Scope: " + scope.to_string() + "; ID: " + std::to_string(id));

	action(
		permission_level{get_self(), name("active")},
		get_self(), name("create"),
		std::make_tuple(scope, o_itr->names,
						o_itr->strings,
						o_itr->assets,
						o_itr->time_points,
						o_itr->ints,
						o_itr->floats,
						o_itr->trxs))
		.send();

	// erase original object
	eraseobj(scope, id);
}

void hyphadao::eraseobjs(const name &scope)
{
	require_auth(get_self());
	object_table o_t(get_self(), scope.value);
	auto o_itr = o_t.begin();
	while (o_itr != o_t.end())
	{
		o_itr = o_t.erase(o_itr);
	}
}

void hyphadao::remperiods(const uint64_t &begin_period_id,
						  const uint64_t &end_period_id)
{
	require_auth(get_self());
	bank.remove_periods(begin_period_id, end_period_id);
}

void hyphadao::changescope(const name &current_scope, const uint64_t &id, const name &new_scope)
{
	require_auth(get_self());
	vector<name> new_scopes = {new_scope};
	change_scope(current_scope, id, new_scopes, true);
}

void hyphadao::resetperiods()
{
	require_auth(get_self());
	bank.reset_periods();
}

void hyphadao::addowner(const name &scope)
{
	require_auth(get_self());
	object_table o_t(get_self(), scope.value);
	auto o_itr = o_t.begin();
	while (o_itr != o_t.end())
	{
		o_t.modify(o_itr, get_self(), [&](auto &o) {
			o.names["owner"] = o_itr->names.at("proposer");
		});
		o_itr++;
	}
}

void hyphadao::updroleint(const uint64_t &role_id, const string &key, const int64_t &intvalue)
{
	object_table o_t(get_self(), name("role").value);
	auto o_itr = o_t.find(role_id);
	check(o_itr != o_t.end(), "Role ID not found: " + std::to_string(role_id));

	o_t.modify(o_itr, get_self(), [&](auto &o) {
		o.ints[key] = intvalue;
	});
}

void hyphadao::updballot(const uint64_t &proposal_id, const name &ballot_id)
{
	object_table o_t(get_self(), name("proposal").value);
	auto o_itr = o_t.find(proposal_id);
	check(o_itr != o_t.end(), "Proposal ID not found: " + std::to_string(proposal_id));

	o_t.modify(o_itr, get_self(), [&](auto &o) {
		o.names["ballot_id"] = ballot_id;
	});
}

void hyphadao::updcreated(const uint64_t &assignment_id, const time_point &tp)
{
	object_table o_t(get_self(), name("assignment").value);
	auto o_itr = o_t.find(assignment_id);
	check(o_itr != o_t.end(), "Assignment ID not found: " + std::to_string(assignment_id));

	o_t.modify(o_itr, get_self(), [&](auto &o) {
		o.created_date = tp;
	});
}

void hyphadao::debugmsg(const string &message)
{
	require_auth(get_self());
	debug(message);
}

void hyphadao::updusdtohusd()
{
	require_auth(get_self());

	symbol usd_symbol = symbol("USD", 2);

	object_table o_t(get_self(), name("assignment").value);
	auto o_itr = o_t.begin();

	while (o_itr != o_t.end())
	{
		if (o_itr->assets.at("husd_salary_per_phase").symbol == usd_symbol)
		{
			o_t.modify(o_itr, get_self(), [&](auto &o) {
				o.assets["husd_salary_per_phase"] = asset{o_itr->assets.at("husd_salary_per_phase").amount, common::S_HUSD};
			});
		}
		o_itr++;
	}
}

// uint64_t hyphadao::hash(std::string str)
// {
// 	uint64_t id = 0;
// 	checksum256 h = sha256(const_cast<char *>(str.c_str()), str.size());
// 	auto hbytes = h.extract_as_byte_array();
// 	for (int i = 0; i < 4; i++)
// 	{
// 		id <<= 8;
// 		id |= hbytes[i];
// 	}
// 	return id;
// }

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

void hyphadao::debug(const string &notes)
{
	debug_table d_t(get_self(), get_self().value);
	d_t.emplace(get_self(), [&](auto &d) {
		d.debug_id = d_t.available_primary_key();
		d.notes = notes;
	});
}

void hyphadao::change_scope(const name &current_scope, const uint64_t &id, const vector<name> &new_scopes, const bool &remove_old)
{

	object_table o_t_current(get_self(), current_scope.value);
	auto o_itr_current = o_t_current.find(id);
	check(o_itr_current != o_t_current.end(), "Scope: " + current_scope.to_string() + "; Object ID: " + std::to_string(id) + " does not exist.");

	for (name new_scope : new_scopes)
	{
		object_table o_t_new(get_self(), new_scope.value);
		o_t_new.emplace(get_self(), [&](auto &o) {
			o.id = o_t_new.available_primary_key();
			o.names = o_itr_current->names;
			o.names["prior_scope"] = current_scope;
			o.assets = o_itr_current->assets;
			o.strings = o_itr_current->strings;
			o.floats = o_itr_current->floats;
			o.time_points = o_itr_current->time_points;
			o.ints = o_itr_current->ints;
			o.ints["prior_id"] = o_itr_current->id;
			o.trxs = o_itr_current->trxs;
		});
		debug("Added object ID: " + std::to_string(id) + " from scope: " + current_scope.to_string() + " to scope: " + new_scope.to_string());
	}

	if (remove_old)
	{
		debug("Erasing object ID: " + std::to_string(id) + " from : " + current_scope.to_string());
		o_t_current.erase(o_itr_current);
	}
}

asset hyphadao::adjust_asset(const asset &original_asset, const float &adjustment)
{
	return asset{static_cast<int64_t>(original_asset.amount * adjustment), original_asset.symbol};
}

float hyphadao::get_float(const std::map<string, uint64_t> ints, string key)
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

string hyphadao::get_string(const std::map<string, string> strings, string key)
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
		std::make_tuple(message));
	trx.delay_sec = 0;
	trx.send(get_next_sender_id(), get_self());

	check(false, message);
}

void hyphadao::check_capacity(const uint64_t &role_id, const uint64_t &req_time_share_x100)
{
	// Ensure that this proposal would not push the role over it's approved full time capacity
	object_table o_t_role(get_self(), name("role").value);
	auto o_itr_role = o_t_role.find(role_id);
	checkx(o_itr_role != o_t_role.end(), "Role ID: " + std::to_string(role_id) + " does not exist.");
	int role_capacity = o_itr_role->ints.at("fulltime_capacity_x100");

	object_table o_t_assignment(get_self(), name("assignment").value);
	auto sorted_by_role = o_t_assignment.get_index<name("byfk")>();
	auto a_itr_by_role = sorted_by_role.find(role_id);
	int consumed_capacity = 0;
	debug("Role capacity: " + std::to_string(role_capacity) + ", fk: " +
		  std::to_string(a_itr_by_role->ints.at("fk")) + "; Role ID: " + std::to_string(role_id));
	while (a_itr_by_role != sorted_by_role.end() && a_itr_by_role->ints.at("fk") == role_id)
	{
		consumed_capacity += a_itr_by_role->ints.at("time_share_x100");
		a_itr_by_role++;
	}

	checkx(consumed_capacity + req_time_share_x100 <= role_capacity, "Role ID: " +
																		 std::to_string(role_id) + " cannot support assignment. Full time capacity (x100) is " + std::to_string(role_capacity) +
																		 " and consumed capacity (x100) is " + std::to_string(consumed_capacity) + "; proposal requests time share (x100) of: " + std::to_string(req_time_share_x100));
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