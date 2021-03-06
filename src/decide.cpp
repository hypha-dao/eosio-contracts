#include <hyphadao.hpp>

using namespace hyphaspace;

name hyphadao::register_ballot(const name &proposer,
								const string &title, const string &description, const string &content)
{
	check(has_auth(proposer) || has_auth(get_self()), "Authentication failed. Must have authority from proposer: " +
														  proposer.to_string() + "@active or " + get_self().to_string() + "@active.");
	qualify_owner(proposer);

	config_table config_s(get_self(), get_self().value);
	Config c = config_s.get_or_create(get_self(), Config());

	// increment the ballot_id
	name new_ballot_id = name(c.names.at("last_ballot_id").value + 1);
	c.names["last_ballot_id"] = new_ballot_id;
	config_s.set(c, get_self());

	trailservice::trail::ballots_table b_t(c.names.at("telos_decide_contract"), c.names.at("telos_decide_contract").value);
	auto b_itr = b_t.find(new_ballot_id.value);
	check(b_itr == b_t.end(), "ballot_id: " + new_ballot_id.to_string() + " has already been used.");

	vector<name> options;
	options.push_back(name("pass"));
	options.push_back(name("fail"));
	options.push_back(name("abstain"));

	action(
		permission_level{get_self(), name("active")},
		c.names.at("telos_decide_contract"), name("newballot"),
		std::make_tuple(
			new_ballot_id,
			name("poll"),
			get_self(),
			common::S_HVOICE,
			name("1token1vote"),
			options))
		.send();

	//	  // default is to vote all tokens, not just staked tokens
	//    action (
	//       permission_level{get_self(), "active"_n},
	//       c.telos_decide_contract, "togglebal"_n,
	//       std::make_tuple(new_ballot_id, "votestake"_n))
	//    .send();

	action(
		permission_level{get_self(), name("active")},
		c.names.at("telos_decide_contract"), name("editdetails"),
		std::make_tuple(
			new_ballot_id,
			title,
			description.substr(0, std::min(description.length(), size_t(25))),
			content))
		.send();

	auto expiration = time_point_sec(current_time_point()) + c.ints.at("voting_duration_sec");

	action(
		permission_level{get_self(), name("active")},
		c.names.at("telos_decide_contract"), name("openvoting"),
		std::make_tuple(new_ballot_id, expiration))
		.send();

	// TODO: add a record to the seeds scheduler to close the proposal
	return new_ballot_id;
}

name hyphadao::register_ballot(const name &proposer,
							   const map<string, string> &strings)
{
	return register_ballot (proposer, strings.at("title"), strings.at("description"), strings.at("content"));
}

// checksum256 hyphadao::castvote (const checksum256 &ballot )