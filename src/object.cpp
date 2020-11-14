
#include <hyphadao.hpp>

using namespace hyphaspace;

void hyphadao::set(const name &scope, const uint64_t &id, const string &key, const hyphadao::flexvalue1 &value)
{
	require_auth(get_self());
	hyphadao::object_table o_t(get_self(), scope.value);
	auto o_itr = o_t.find(id);
	check(o_itr != o_t.end(), "Scope: " + scope.to_string() + "; ID: " + std::to_string(id) + " does not exist.");

	o_t.modify(o_itr, get_self(), [&](auto &o) {
		if (std::holds_alternative<name>(value))
		{
			o.names[key] = std::get<name>(value);
		}
		else if (std::holds_alternative<string>(value))
		{
			o.strings[key] = std::get<string>(value);
		}
		else if (std::holds_alternative<asset>(value))
		{
			o.assets[key] = std::get<asset>(value);
		}
		else if (std::holds_alternative<time_point>(value))
		{
			o.time_points[key] = std::get<time_point>(value);
		}
		else if (std::holds_alternative<uint64_t>(value))
		{
			o.ints[key] = std::get<uint64_t>(value);
		}
	});
}

void hyphadao::new_object(const name &creator,
						  const name &scope,
						  const map<string, name> names,
						  const map<string, string> strings,
						  const map<string, asset> assets,
						  const map<string, time_point> time_points,
						  const map<string, uint64_t> ints,
						  const map<string, transaction> trxs)
{

	hyphadao::object_table o_t(get_self(), scope.value);
	o_t.emplace(get_self(), [&](auto &o) {
		o.id = o_t.available_primary_key();
		o.names = names;
		o.strings = strings;
		o.assets = assets;
		o.time_points = time_points;
		o.ints = ints;
		o.trxs = trxs;

		o.ints["object_id"] = o.id;
		new_document(creator, scope, o.names, o.strings, o.assets, o.time_points, o.ints);
	});
}

void hyphadao::merge(const name &scope,
					 const uint64_t &id,
					 const map<string, name> names,
					 const map<string, string> strings,
					 const map<string, asset> assets,
					 const map<string, time_point> time_points,
					 const map<string, uint64_t> ints,
					 const map<string, transaction> trxs)
{

	object_table original_t(get_self(), scope.value);
	auto original_itr = original_t.find(id);
	check(original_itr != original_t.end(), "Scope: " + scope.to_string() + "; ID: " + std::to_string(id) + " does not exist.");

	original_t.modify(original_itr, get_self(), [&](auto &orig) {
		// edit all maps, over-writing any values having the same key
		std::map<string, name>::const_iterator name_itr;
		for (name_itr = names.begin(); name_itr != names.end(); ++name_itr)
		{
			// skip over system attributes
			if (name_itr->first == "owner" || name_itr->first == "type" || name_itr->first == "ballot_id" || name_itr->first == "prior_scope" ||
				name_itr->first == "trx_action_name" || name_itr->first == "trx_action_contract" || name_itr->first == "original_scope")
			{
				continue;
			}
			orig.names[name_itr->first] = name_itr->second;
		}

		std::map<string, string>::const_iterator string_itr;
		for (string_itr = strings.begin(); string_itr != strings.end(); ++string_itr)
		{
			// skip over system attributes
			if (string_itr->first == "client_version" || string_itr->first == "contract_version")
			{
				continue;
			}
			orig.strings[string_itr->first] = string_itr->second;
		}

		std::map<string, asset>::const_iterator asset_itr;
		for (asset_itr = assets.begin(); asset_itr != assets.end(); ++asset_itr)
		{
			orig.assets[asset_itr->first] = asset_itr->second;
		}

		std::map<string, time_point>::const_iterator time_point_itr;
		for (time_point_itr = time_points.begin(); time_point_itr != time_points.end(); ++time_point_itr)
		{
			orig.time_points[time_point_itr->first] = time_point_itr->second;
		}

		std::map<string, uint64_t>::const_iterator int_itr;
		for (int_itr = ints.begin(); int_itr != ints.end(); ++int_itr)
		{
			// skip over system attributes
			if (int_itr->first == "revisions" || int_itr->first == "prior_id")
			{
				continue;
			}
			orig.ints[int_itr->first] = int_itr->second;
		}

		if (original_itr->ints.find("revisions") != original_itr->ints.end())
		{
			orig.ints["revisions"] = original_itr->ints.at("revisions") + 1;
		}
		else
		{
			orig.ints["revisions"] = 1;
		}
		orig.updated_date = current_time_point();
	});
}

void hyphadao::copytodraft(const name &copier, const name &scope, const uint64_t &id)
{
	object_table o_t(get_self(), scope.value);
	auto o_itr = o_t.find(id);
	check(o_itr != o_t.end(), "Scope: " + scope.to_string() + "; ID: " + std::to_string(id) + " does not exist.");

	object_table drafts(get_self(), name("draft").value);
	drafts.emplace(get_self(), [&](auto &o) {
		o.id = drafts.available_primary_key();
		o.names = o_itr->names;
		o.assets = o_itr->assets;
		o.strings = o_itr->strings;
		o.floats = o_itr->floats;
		o.time_points = o_itr->time_points;
		o.ints = o_itr->ints;
		o.trxs = o_itr->trxs;

		o.names["prior_scope"] = scope;
		o.ints["prior_id"] = o_itr->id;
		o.names["owner"] = copier;
	});
	debug("Copied document: " + std::to_string(id) + " from scope: " + scope.to_string() + " to users drafts: " + copier.to_string());
}

void hyphadao::erasedraft(const uint64_t &id)
{
	object_table o_t(get_self(), name("draft").value);
	auto o_itr = o_t.find(id);
	check(o_itr != o_t.end(), "Scope: draft; ID: " + std::to_string(id) + " does not exist.");

	check(has_auth(o_itr->names.at("owner")), "Permission denied. Only draft authors can erase drafts.");
	o_t.erase(o_itr);
}

void hyphadao::propdraft(const uint64_t &id)
{
	object_table o_t(get_self(), name("draft").value);
	auto o_itr = o_t.find(id);
	check(o_itr != o_t.end(), "Draft not found; ID: " + std::to_string(id));
	check(has_auth(o_itr->names.at("owner")), "Permission denied. Only draft authors can propose from their drafts.");

	action(
		permission_level{get_self(), name("active")},
		get_self(), name("create"),
		std::make_tuple(name("proposal"), o_itr->names,
						o_itr->strings,
						o_itr->assets,
						o_itr->time_points,
						o_itr->ints,
						o_itr->floats,
						o_itr->trxs))
		.send();

	// erase original object
	erasedraft(id);
}

void hyphadao::resetscope(const name &scope)
{
	require_auth(get_self());
	object_table o_t(get_self(), scope.value);
	auto o_itr = o_t.begin();
	while (o_itr != o_t.end())
	{
		o_itr = o_t.erase(o_itr);
	}
}

void hyphadao::erasedoc(const name &scope,
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
	// eraseobj(scope, id);
}

// void hyphadao::changescope(const name &current_scope, const uint64_t &id, const name &new_scope)
// {
// 	vector<name> new_scopes = {new_scope};
// 	change_scope(current_scope, id, new_scopes, true);
// }

void hyphadao::changescope(const name &current_scope, const uint64_t &id, const vector<name> &new_scopes, const bool &remove_old)
{
	require_auth(get_self());
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

			new_document(get_self(), new_scope, o.names, o.strings, o.assets, o.time_points, o.ints);
		});

		debug("Added object ID: " + std::to_string(id) + " from scope: " + current_scope.to_string() + " to scope: " + new_scope.to_string());
	}

	if (remove_old)
	{
		debug("Erasing object ID: " + std::to_string(id) + " from : " + current_scope.to_string());
		o_t_current.erase(o_itr_current);
	}
}

void hyphadao::transscope (const name &creator, const name &scope, const uint64_t &starting_id, const uint64_t &batch_size) 
{
	require_auth (creator);
	object_table o_t(get_self(), scope.value);
	auto o_itr = o_t.find(starting_id);

	while (o_itr->id <= starting_id + batch_size) 
	{
		transform (creator, scope, o_itr->id);
		o_itr++;
	}

	eosio::transaction out{};
	out.actions.emplace_back(permission_level{get_self(), name("active")},
							 get_self(), name("transscope"),
							 make_tuple(creator, scope, o_itr->id, batch_size));
	out.delay_sec = 1;
	out.send(get_next_sender_id(), get_self());
}

// void hyphadao::erasedocs (const name &scope) 
// {
// 	require_auth (get_self());
// 	document_table d_t(get_self(), scope.value);
// 	auto d_itr = d_t.begin();

// 	while (d_itr != d_t.end()) 
// 	{
// 		d_itr = d_t.erase (d_itr);
// 	}
// }

void hyphadao::transform(const name &creator, const name &scope, const uint64_t &id)
{
	require_auth (creator);
	object_table o_t(get_self(), scope.value);
	auto o_itr = o_t.find(id);
	check(o_itr != o_t.end(), "Scope: " + scope.to_string() + "; Object ID: " + std::to_string(id) + " does not exist.");

	new_document(creator, scope, o_itr->names, o_itr->strings, o_itr->assets, o_itr->time_points, o_itr->ints);
}

void hyphadao::new_document(const name &creator,
							const name &scope,
							const map<string, name> names,
							const map<string, string> strings,
							const map<string, asset> assets,
							const map<string, time_point> time_points,
							const map<string, uint64_t> ints)
{
	// vector<document_graph::content_group> content_groups;
	// document_graph::content_group cg{};

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
}