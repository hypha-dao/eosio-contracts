#include <hyphadao.hpp>

using namespace hyphaspace;

void hyphadao::createroot (const string &notes)
{
	require_auth (get_self());

	document_graph::document root = _document_graph.get_or_create (get_self(), _document_graph.new_content("root_node", get_self()));
	setconfigatt("root_node", _document_graph.readable_hash(root.hash));
}

checksum256 hyphadao::get_root (const name &contract)
{
	auto ctnt = document_graph::new_content("root_node", contract);
	document_graph::content_group cg; 
	cg.push_back(ctnt);
	vector<document_graph::content_group> cgs;
	cgs.push_back(cg);
	return document_graph::hash_document(cgs);
}

checksum256 hyphadao::get_root ()
{
	return get_root(get_self());
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
	config_table config_s(get_self(), get_self().value);
	Config c = config_s.get_or_create(get_self(), Config());
	c.strings[component] = version;
	config_s.set(c, get_self());
}

void hyphadao::setlastballt(const name &last_ballot_id)
{
	require_auth(get_self());
	config_table config_s(get_self(), get_self().value);
	Config c = config_s.get_or_create(get_self(), Config());
	c.names["last_ballot_id"] = last_ballot_id;
	config_s.set(c, get_self());
}

void hyphadao::togglepause()
{
	require_auth(get_self());
	config_table config_s(get_self(), get_self().value);
	Config c = config_s.get_or_create(get_self(), Config());
	if (c.ints.find("paused") == c.ints.end() || c.ints.at("paused") == 0)
	{
		c.ints["paused"] = 1;
	}
	else
	{
		c.ints["paused"] = 0;
	}
	config_s.set(c, get_self());
}
