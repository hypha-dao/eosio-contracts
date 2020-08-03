#include <hyphadao.hpp>

using namespace hyphaspace;

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
