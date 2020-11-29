#include <hyphadao.hpp>

#include <algorithm>

using namespace hyphaspace;

void hyphadao::createroot (const string &notes)
{
	require_auth (get_self());

	document_graph::document root = _document_graph.get_or_create (get_self(), 
																   _document_graph.new_content(common::ROOT_NODE, get_self()));

	//Create the settings document as well and add an edge to it
	document_graph::document settings = _document_graph.get_or_create(get_self(), 
																	  _document_graph.new_content(common::ROOT_NODE, _document_graph.readable_hash(root.hash)));

	_document_graph.create_edge(root.hash, settings.hash, common::SETTINGS_EDGE);
	/* Legacy Config Table
	* setconfigatt("root_node", _document_graph.readable_hash(root.hash));
	*/
}

checksum256 hyphadao::get_root ()
{
	auto ctnt = _document_graph.new_content("root_node", get_self());
	document_graph::content_group cg; 
	cg.push_back(ctnt);
	vector<document_graph::content_group> cgs;
	cgs.push_back(cg);
	return document_graph::hash_document(cgs);
}

document_graph::document hyphadao::getSettingsDocument()
{
	auto root = get_root();
	
	auto settings_edge = _document_graph.get_edge(root, common::SETTINGS_EDGE, true);

	return _document_graph.get_document(settings_edge.to_node);
}

void hyphadao::setconfig(const map<string, name> names,
						 const map<string, string> strings,
						 const map<string, asset> assets,
						 const map<string, time_point> time_points,
						 const map<string, uint64_t> ints,
						 const map<string, float> floats,
						 const map<string, transaction> trxs)
{
	require_auth(get_self());

	config_table config_s(get_self(), get_self().value);
	Config c = config_s.get_or_create(get_self(), Config());

	// retain last_ballot_id from the current configuration if it is not provided in the new one
	name last_ballot_id;
	if (names.find("last_ballot_id") != names.end())
	{
		last_ballot_id = names.at("last_ballot_id");
	}
	else if (c.names.find("last_ballot_id") != c.names.end())
	{
		last_ballot_id = c.names.at("last_ballot_id");
	}

	c.names = names;
	c.names["last_ballot_id"] = last_ballot_id;

	c.strings = strings;
	c.assets = assets;
	c.time_points = time_points;
	c.ints = ints;
	c.floats = floats;
	c.trxs = trxs;

	config_s.set(c, get_self());

	// validate for required configurations
	string required_names[]{"hypha_token_contract", "husd_token_contract", "seeds_token_contract", "telos_decide_contract", "last_ballot_id"};
	for (int i{0}; i < std::size(required_names); i++)
	{
		check(c.names.find(required_names[i]) != c.names.end(), "name configuration: " + required_names[i] + " is required but not provided.");
	}
}

void hyphadao::setconfigatt(const string& key, const hyphadao::flexvalue1& value)
{
	require_auth(get_self());

	config_table config_s(get_self(), get_self().value);
	Config c = config_s.get_or_create(get_self(), Config());

	if (std::holds_alternative<name>(value))
	{
		c.names[key] = std::get<name>(value);
	}
	else if (std::holds_alternative<string>(value))
	{
		c.strings[key] = std::get<string>(value);
	}
	else if (std::holds_alternative<asset>(value))
	{
		c.assets[key] = std::get<asset>(value);
	}
	else if (std::holds_alternative<time_point>(value))
	{
		c.time_points[key] = std::get<time_point>(value);
	}
	else if (std::holds_alternative<uint64_t>(value))
	{
		c.ints[key] = std::get<uint64_t>(value);
	}
	c.time_points["updated_date"] = current_time_point();
 	config_s.set(c, get_self());
}

void hyphadao::remconfigatt(const string& key)
{
	require_auth(get_self());

	config_table config_s(get_self(), get_self().value);
	Config c = config_s.get_or_create(get_self(), Config());

	if (c.names.find(key) != c.names.end())
	{
		c.names.erase (key);
	}
	else if (c.strings.find(key) != c.strings.end())
	{
		c.strings.erase (key);
	}
	else if (c.assets.find(key) != c.assets.end())
	{
		c.assets.erase (key);
	}
	else if (c.time_points.find(key) != c.time_points.end())
	{
		c.time_points.erase (key);
	}
	else if (c.ints.find(key) != c.ints.end())
	{
		c.ints.erase (key);
	}
	c.time_points["updated_date"] = current_time_point();
 	config_s.set(c, get_self());
}

void hyphadao::setsetting(const string &key, const flexvalue& value)
{
	require_auth(get_self());

	auto document = getSettingsDocument();
	
	auto& content_group = document.content_groups.at(0);

	auto setting_content = document_graph::content{key, value};

	document_graph::insert_or_replace(content_group, setting_content);
	
	auto updated_setting_content = document_graph::content{common::UPDATED_DATE, current_time_point()};

	document_graph::insert_or_replace(content_group, updated_setting_content);

	document = _document_graph.update_document(get_self(), document.hash, std::move(document.content_groups));

	//Update edges
	_document_graph.update_edge_to(get_root(), common::SETTINGS_EDGE, document.hash);
}

void hyphadao::remsetting(const string &key)
{
	require_auth(get_self());

	auto document = getSettingsDocument();

	auto& content_group = document.content_groups.at(0);

	auto is_key = [&key](auto &c) 
	{
		return c.label == key;
	};

	//First let's check if key exists
	auto content_itr = std::find_if(content_group.begin(), content_group.end(), is_key);

	if (content_itr != content_group.end())
	{	
		content_group.erase(content_itr);
		
		auto updated_setting_content = document_graph::content{common::UPDATED_DATE, current_time_point()};

		document_graph::insert_or_replace(content_group, updated_setting_content);

		document = _document_graph.update_document(get_self(), document.hash, std::move(document.content_groups));

		//Update edges
		_document_graph.update_edge_to(get_root(), common::SETTINGS_EDGE, document.hash);
	}
	//Should we assert if setting doesn't exits ?
}

void hyphadao::setalert (const name &level, const string &content)
{
	// inline actions seem happiest when affixing types
	string alert_level = string {"alert_level"};
	string alert_content = string {"alert_content"};
	hyphadao::flexvalue1 fv_level = level;
	hyphadao::flexvalue1 fv_content = content;

	action(
		permission_level{get_self(), name("active")},
		get_self(), name("setconfigatt"),
		std::make_tuple(alert_level, fv_level))
	.send();
	
	action(
		permission_level{get_self(), name("active")},
		get_self(), name("setconfigatt"),
		std::make_tuple(alert_content, fv_content))
	.send();
}

void hyphadao::remalert (const string &notes)
{
	// inline actions seem happiest when affixing types
	string alert_level = string {"alert_level"};
	string alert_content = string {"alert_content"};
	
	action(
		permission_level{get_self(), name("active")},
		get_self(), name("remconfigatt"),
		std::make_tuple(alert_level))
	.send();

	action(
		permission_level{get_self(), name("active")},
		get_self(), name("remconfigatt"),
		std::make_tuple(alert_content))
	.send();
}


void hyphadao::updversion(const string &component, const string &version)
{
	setsetting(component, version);
}

void hyphadao::setlastballt(const name &last_ballot_id)
{
	setsetting(common::LAST_BALLOT_ID, last_ballot_id);
}

void hyphadao::togglepause()
{
	int64_t new_state = !getSettingOrDefault<int64_t>(common::PAUSED);

	setsetting(common::PAUSED, new_state);
}
