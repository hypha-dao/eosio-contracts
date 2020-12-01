#include <hyphadao.hpp>

namespace hypha
{

	// void hyphadao::event(const name &level,
	// 					 const map<string, hyphadao::flexvalue1> &values)
	// {

	// 	name publisher_contract = getSettingOrFail<name>(common::PUBLISHER_CONTRACT);

	// 	action(
	// 		permission_level{get_self(), name("active")},
	// 		publisher_contract, name("event"),
	// 		make_tuple(get_self(), level, values))
	// 		.send();
	// }

	// void hyphadao::clrdebugs(const uint64_t &starting_id, const uint64_t &batch_size)
	// {
	// 	check(has_auth(name("hyphanewyork")) || has_auth(get_self()), "Requires higher permission."); // TODO: remove hyphanewyork
	// 	debug_table d_t(get_self(), get_self().value);
	// 	auto d_itr = d_t.find(starting_id);

	// 	while (d_itr->debug_id <= starting_id + batch_size)
	// 	{
	// 		d_itr = d_t.erase(d_itr);
	// 	}

	// 	eosio::transaction out{};
	// 	out.actions.emplace_back(permission_level{get_self(), name("active")},
	// 							 get_self(), name("clrdebugs"),
	// 							 make_tuple(d_itr->debug_id, batch_size));
	// 	out.delay_sec = 1;
	// 	out.send(get_next_sender_id(), get_self());
	// }


// void hyphadao::new_document(const name &creator,
// 							const name &scope,
// 							const map<string, name> names,
// 							const map<string, string> strings,
// 							const map<string, asset> assets,
// 							const map<string, time_point> time_points,
// 							const map<string, uint64_t> ints)
// {
	// vector<ContentGroup> content_groups;
	// ContentGroup cg{};

	// document_graph::content c{};
	// c.label = "scope";
	// c.value = scope;
	// cg.push_back(c);

	// std::map<string, name>::const_iterator name_itr;
	// for (name_itr = names.begin(); name_itr != names.end(); ++name_itr)
	// {
	// 	document_graph::content c{};
	// 	c.label = name_itr->first;
	// 	c.value = name_itr->second;
	// 	cg.push_back(c);
	// }

	// std::map<string, asset>::const_iterator asset_itr;
	// for (asset_itr = assets.begin(); asset_itr != assets.end(); ++asset_itr)
	// {
	// 	document_graph::content c{};
	// 	c.label = asset_itr->first;
	// 	c.value = asset_itr->second;
	// 	cg.push_back(c);
	// }

	// std::map<string, string>::const_iterator string_itr;
	// for (string_itr = strings.begin(); string_itr != strings.end(); ++string_itr)
	// {
	// 	document_graph::content c{};
	// 	c.label = string_itr->first;
	// 	c.value = string_itr->second;
	// 	cg.push_back(c);
	// }

	// std::map<string, uint64_t>::const_iterator int_itr;
	// for (int_itr = ints.begin(); int_itr != ints.end(); ++int_itr)
	// {
	// 	document_graph::content c{};
	// 	c.label = int_itr->first;
	// 	c.value = int_itr->second;
	// 	cg.push_back(c);
	// }

	// std::map<string, time_point>::const_iterator time_point_itr;
	// for (time_point_itr = time_points.begin(); time_point_itr != time_points.end(); ++time_point_itr)
	// {
	// 	document_graph::content c{};
	// 	c.label = time_point_itr->first;
	// 	c.value = time_point_itr->second;
	// 	cg.push_back(c);
	// }

	// content_groups.push_back(cg);

	// // needs to be inline action to use the get_self() permission
	// // action(
	// // 	permission_level{get_self(), name("active")},
	// // 	contract, name("createdoc"),
	// // 	std::make_tuple(get_self(), content_groups))
	// // .send();
	// _document_graph.create_document(creator, content_groups);
// }

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

	float hyphadao::get_float(const string& key)
	{
		return 0.0; //getSettingOrFail<int64_t>(key) / 100.f;
	}

	bool hyphadao::is_paused()
	{
		// if (auto paused = getSettingOpt<int64_t>(common::PAUSED))
		// {
		// 	return *paused == 1;
		// }

		// check(false, "Contract does not have a pause configuration. Assuming it is paused. Please contact administrator.");

		return false;
	}
	asset hyphadao::adjust_asset(const asset &original_asset, const float &adjustment)
	{
		return asset{static_cast<int64_t>(original_asset.amount * adjustment), original_asset.symbol};
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

} // namespace hypha