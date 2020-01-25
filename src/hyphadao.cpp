#include <hyphadao.hpp>

void hyphadao::addmember (const name& member) {
	require_auth (get_self());
	member_table m_t (get_self(), get_self().value);
	auto m_itr = m_t.find (member.value);
	check (m_itr == m_t.end(), "Account is already a member: " + member.to_string());
	m_t.emplace (get_self(), [&](auto &m) {
		m.member = member;
	});
}

void hyphadao::removemember (const name& member) {
	require_auth (get_self());
	member_table m_t (get_self(), get_self().value);
	auto m_itr = m_t.find (member.value);
	check (m_itr != m_t.end(), "Account is not a member: " + member.to_string());
	m_t.erase (m_itr);
}

void hyphadao::reset () {
	holocracy.reset ();
	// bank.reset_config ();

	proposal_table p_t (get_self(),get_self().value);
	auto  p_itr = p_t.begin();
	while (p_itr != p_t.end()) {
		p_itr = p_t.erase (p_itr);
	}

	p_t = proposal_table (get_self(), "archive"_n.value);
	p_itr = p_t.begin();
	while (p_itr != p_t.end()) {
		p_itr = p_t.erase (p_itr);
	}

	// config_table      config_s (get_self(), get_self().value);
   	// Config c = config_s.get_or_create (get_self(), Config()); 
	// config_s.remove ();
}

void hyphadao::remperiods (const uint64_t& begin_period_id, 
                           const uint64_t& end_period_id) {
    bank.remove_periods (begin_period_id, end_period_id);
}

void hyphadao::resetperiods () {
	bank.reset_periods();
}

void hyphadao::setconfig (	const map<string, name> 		names,
							const map<string, string>       strings,
							const map<string, asset>        assets,
							const map<string, time_point>   time_points,
							const map<string, uint64_t>     ints,
							const map<string, float>        floats,
							const map<string, transaction>  trxs)
{
	require_auth (get_self());

	config_table      config_s (get_self(), get_self().value);
   	Config c = config_s.get_or_create (get_self(), Config());   

	// retain last_ballot_id from the current configuration if it is not provided in the new one
	name last_ballot_id	;
	if (names.find("last_ballot_id") != names.end()) { 
		last_ballot_id	= names.at("last_ballot_id"); 
	} else if (c.names.find("last_ballot_id") != c.names.end()) {
		last_ballot_id	= c.names.at("last_ballot_id");
	}

	c.names						= names;
	c.names["last_ballot_id"] 	= last_ballot_id;

	c.strings		= strings;
	c.assets			= assets;
	c.time_points	= time_points;
	c.ints			= ints;
	c.floats			= floats;
	c.trxs			= trxs;

	config_s.set (c, get_self());

	// validate for required configurations
    string required_names[]{ "hypha_token_contract", "seeds_token_contract", "telos_decide_contract", "last_ballot_id"};
    for (int i{ 0 }; i < std::size(required_names); i++) {
		check (c.names.find(required_names[i]) != c.names.end(), "name configuration: " + required_names[i] + " is required but not provided.");
	}

	// bank.set_config (names.at("hypha_token_contract"), names.at("seeds_token_contract"), names.at("telos_decide_contract"));
}

void hyphadao::setlastballt ( const name& last_ballot_id) {
	require_auth (get_self());
	config_table      config_s (get_self(), get_self().value);
   	Config c = config_s.get_or_create (get_self(), Config());   
	c.names["last_ballot_id"]			=	last_ballot_id;
	config_s.set (c, get_self());
}

void hyphadao::enroll (	const name& enroller,
						const name& applicant, 
						const string& content) {

	// this action is linked to the hyphadaomain@enrollers permission
	applicant_table a_t (get_self(), get_self().value);
	auto a_itr = a_t.find (applicant.value);
	check (a_itr != a_t.end(), "Applicant not found: " + applicant.to_string());

	config_table      config_s (get_self(), get_self().value);
   	Config c = config_s.get_or_create (get_self(), Config());  

	asset one_hvoice = asset { 100, common::S_HVOICE };
	string memo { "Welcome to Hypha DAO!"};
	action(	
		permission_level{get_self(), "active"_n}, 
		c.names.at("telos_decide_contract"), "mint"_n, 
		make_tuple(applicant, one_hvoice, memo))
	.send();

	// Should we also send 1 HYPHA?  I think so, so I'll put it for now, but comment it out
	// asset one_hypha = asset { 1, common::S_HYPHA };
	// bank.makepayment (-1, applicant, one_hypha, memo, common::NO_ASSIGNMENT);

	member_table m_t (get_self(), get_self().value);
	auto m_itr = m_t.find (applicant.value);
	check (m_itr == m_t.end(), "Account is already a member: " + applicant.to_string());
	m_t.emplace (get_self(), [&](auto &m) {
		m.member = applicant;
	});

	a_t.erase (a_itr);
}	

void hyphadao::remapply (const name& applicant) {
	require_auth (get_self());
	applicant_table a_t (get_self(), get_self().value);
	auto a_itr = a_t.find (applicant.value);
	a_t.erase (a_itr);
}

void hyphadao::apply (const name& applicant, 
						const string& content) {

	require_auth (applicant);

	member_table m_t (get_self(), get_self().value);
	auto m_itr = m_t.find (applicant.value);
	check (m_itr == m_t.end(), "Applicant is already a member: " + applicant.to_string());

	applicant_table a_t (get_self(), get_self().value);
	auto a_itr = a_t.find (applicant.value);

	if (a_itr != a_t.end()) {
		a_t.modify (a_itr, get_self(), [&](auto &a) {
			a.content = content;
			a.updated_date = current_time_point();
		});
	} else {
		a_t.emplace (get_self(), [&](auto &a) {
			a.applicant = applicant;
			a.content = content;
		});
	}
}				

name hyphadao::register_ballot (const name& proposer, 
								const map<string, string>& strings) 
{
	require_auth(proposer);
	qualify_proposer(proposer);

	config_table      config_s (get_self(), get_self().value);
   	Config c = config_s.get_or_create (get_self(), Config());  
	
	// increment the ballot_id
	name new_ballot_id = name (c.names.at("last_ballot_id").value + 1);
	c.names["last_ballot_id"] = new_ballot_id;
	config_s.set(c, get_self());
	
	trailservice::trail::ballots_table b_t (c.names.at("telos_decide_contract"), c.names.at("telos_decide_contract").value);
	auto b_itr = b_t.find (new_ballot_id.value);
	check (b_itr == b_t.end(), "ballot_id: " + new_ballot_id.to_string() + " has already been used.");

	vector<name> options;
   	options.push_back ("pass"_n);
   	options.push_back ("fail"_n);

	action (
      permission_level{get_self(), "active"_n},
      c.names.at("telos_decide_contract"), "newballot"_n,
      std::make_tuple(
			new_ballot_id, 
			"poll"_n, 
			get_self(), 
			common::S_HVOICE, 
			"1token1vote"_n, 
			options))
   .send();

	//	  // default is to vote all tokens, not just staked tokens
	//    action (
	//       permission_level{get_self(), "active"_n},
	//       c.telos_decide_contract, "togglebal"_n,
	//       std::make_tuple(new_ballot_id, "votestake"_n))
	//    .send();

   action (
	   	permission_level{get_self(), "active"_n},
		c.names.at("telos_decide_contract"), "editdetails"_n,
		std::make_tuple(
			new_ballot_id, 
			strings.at("title"), 
			strings.at("description"),
			strings.at("content")))
   .send();

   auto expiration = time_point_sec(current_time_point()) + c.ints.at("voting_duration_sec");
   
   action (
      permission_level{get_self(), "active"_n},
      c.names.at("telos_decide_contract"), "openvoting"_n,
      std::make_tuple(new_ballot_id, expiration))
   .send();

	return new_ballot_id;
}

void hyphadao::propose (const map<string, name> 		names,
						const map<string, string>       strings,
						const map<string, asset>        assets,
						const map<string, time_point>   time_points,
						const map<string, uint64_t>     ints,
						const map<string, float>        floats,
						const map<string, transaction>  trxs)
{
	const name proposer = names.at("proposer");

	require_auth (proposer);
	qualify_proposer (proposer);

	proposal_table p_t (get_self(), get_self().value);
	p_t.emplace (get_self(), [&](auto &p) {
		p.id                       	= p_t.available_primary_key();
		p.proposer					= proposer;

		p.names                    	= names;

		/* default trx_action_account to hyphadaomain */
		if (names.find("trx_action_contract") == names.end()) {
			p.names["trx_action_contract"] = get_self();
		}

		p.names["ballot_id"]		= register_ballot (proposer, strings);

		p.strings                  	= strings;
		p.assets                  	= assets;
		p.time_points              	= time_points;
		p.ints                     	= ints;
		p.floats                   	= floats;
		p.trxs                     	= trxs;

		if (names.find("trx_action_name") != names.end()) {
			// this transaction executes if the proposal passes
			transaction trx (time_point_sec(current_time_point())+ (60 * 60 * 24 * 35));
			trx.actions.emplace_back(
				permission_level{get_self(), "active"_n}, 
				p.names.at("trx_action_contract"), p.names.at("trx_action_name"), 
				std::make_tuple(p.id));
			trx.delay_sec = 0;
			p.trxs["exec_on_approval"]      = trx;      
		}		
	});      
}

void hyphadao::clrdebugs (const uint64_t& starting_id, const uint64_t& batch_size) {
	debug_table d_t (get_self(), get_self().value);
	auto d_itr = d_t.find (starting_id);

	while (d_itr->debug_id <= starting_id + batch_size) {
		d_itr = d_t.erase (d_itr);
	}

	eosio::transaction out{};
	out.actions.emplace_back(permission_level{get_self(), "active"_n}, 
							get_self(), "clrdebugs"_n, 
							std::make_tuple(d_itr->debug_id, batch_size));
	out.delay_sec = 1;
	out.send(get_next_sender_id(), get_self());    
}

void hyphadao::exectrx (const uint64_t& proposal_id) {
	require_auth (get_self());

	proposal_table p_t (get_self(), get_self().value);
	auto p_itr = p_t.find (proposal_id);
	check (p_itr != p_t.end(), "Proposal ID: " + std::to_string(proposal_id) + " does not exist.");
	check (p_itr->trxs.size() > 1, "There are not transactions to execute. Trx map size: " + std::to_string(p_itr->trxs.size()) + "; Proposal ID: " + std::to_string(proposal_id));

	for (auto trx_itr = p_itr->trxs.begin(); trx_itr != p_itr->trxs.end(); ++trx_itr) {
		// skip the transaction named "exec_on_approval" because that is the one currently executing 
		// debug ( "trx_itr->first	: " + trx_itr->first);
		// debug ( "true/false		: " + std::to_string(trx_itr->first.compare("exec_on_approval") == 0));
		if (trx_itr->first.compare("exec_on_approval") == 0) {
			debug ("Executing transaction	: " + trx_itr->first);
			trx_itr->second.send(current_block_time().to_time_point().sec_since_epoch(), get_self());
		}
	}
}

void hyphadao::newrole (const uint64_t& proposal_id) {

   	require_auth (get_self());

	proposal_table p_t (get_self(), get_self().value);
	auto p_itr = p_t.find (proposal_id);
	check (p_itr != p_t.end(), "Proposal ID: " + std::to_string(proposal_id) + " does not exist.");

	debug ("Creating Role::Title: " + p_itr->strings.at("title"));

	holocracy.newrole (p_itr->strings.at("title"),
						p_itr->strings.at("description"),
						p_itr->strings.at("content"),
						p_itr->assets.at("hypha_amount"),
						p_itr->assets.at("seeds_amount"),
						p_itr->assets.at("hvoice_amount"),
						p_itr->ints.at("start_period"),
						p_itr->ints.at("end_period"));
			
	archive (proposal_id);
}

void hyphadao::addperiod (const time_point& start_date, const time_point& end_date, const string& phase) {
	bank.addperiod (start_date, end_date, phase);
}

void hyphadao::assign ( const uint64_t& 		proposal_id) {

   	require_auth (get_self());

	proposal_table p_t (get_self(), get_self().value);
	auto p_itr = p_t.find (proposal_id);
	check (p_itr != p_t.end(), "Proposal ID: " + std::to_string(proposal_id) + " does not exist.");

	holocracy.newassign (	p_itr->names.at("assigned_account"),
							p_itr->ints.at("role_id"),
							p_itr->strings.at("title"),
							p_itr->strings.at("description"),
							p_itr->strings.at("content"),
							p_itr->ints.at("start_period"),
							p_itr->ints.at("end_period"),
							p_itr->floats.at("time_share"));

	archive (proposal_id);
}

void hyphadao::payassign(const uint64_t& assignment_id, const uint64_t& period_id) {
	holocracy.payassign (assignment_id, period_id);
}

void hyphadao::makepayout (const uint64_t&        proposal_id) {

	require_auth (get_self());

	proposal_table p_t (get_self(), get_self().value);
	auto p_itr = p_t.find (proposal_id);
	check (p_itr != p_t.end(), "Proposal ID: " + std::to_string(proposal_id) + " does not exist.");

	string memo { "One time payout for Hypha DAO Contribution: " + std::to_string(proposal_id) };
	debug (" HYPHA payout: " + p_itr->assets.at("hypha_amount").to_string());

	if (p_itr->assets.at("hypha_amount").amount > 0) {
		debug ("Making a payment to : " + p_itr->names.at("recipient").to_string());
		bank.makepayment (-1, p_itr->names.at("recipient"), p_itr->assets.at("hypha_amount"), memo, common::NO_ASSIGNMENT);
	}

	if (p_itr->assets.at("seeds_amount").amount > 0) {
		bank.makepayment (-1, p_itr->names.at("recipient"), p_itr->assets.at("seeds_amount"), memo, common::NO_ASSIGNMENT);
	}
	
	if (p_itr->assets.at("hvoice_amount").amount > 0) {
		config_table      config_s (get_self(), get_self().value);
   		Config c = config_s.get_or_create (get_self(), Config());  

		action(	
			permission_level{get_self(), "active"_n}, 
			c.names.at("telos_decide_contract"), "mint"_n, 
			make_tuple(p_itr->names.at("recipient"), p_itr->assets.at("hvoice_amount"), memo
		)).send();
	}

	archive (proposal_id);
}

void hyphadao::eraseprop (const uint64_t& proposal_id) {
	require_auth (get_self());
	proposal_table props(get_self(), get_self().value);
	auto i_iter = props.find(proposal_id);
	check(i_iter != props.end(), "prop not found");
	props.erase (i_iter);
}

void hyphadao::closeprop(const uint64_t& proposal_id) {

	proposal_table props(get_self(), get_self().value);
	auto i_iter = props.find(proposal_id);
	check(i_iter != props.end(), "prop not found");
	auto prop = *i_iter;

	config_table      config_s (get_self(), get_self().value);
   	Config c = config_s.get_or_create (get_self(), Config());  

	trailservice::trail::ballots_table b_t (c.names.at("telos_decide_contract"), c.names.at("telos_decide_contract").value);
	auto b_itr = b_t.find (prop.names.at("ballot_id").value);
	check (b_itr != b_t.end(), "ballot_id: " + prop.names.at("ballot_id").to_string() + " not found.");

	string debug_str = string ("Ballot ID read from prop for closing ballot: " + prop.names.at("ballot_id").to_string() + "\n");
	action (
		permission_level{get_self(), "active"_n},
		c.names.at("telos_decide_contract"), "closevoting"_n,
		std::make_tuple(prop.names.at("ballot_id"), true))
	.send();

	uint32_t quorum_threshold = b_itr->total_voters / (5 - 1);
	debug_str = debug_str + "Quorum threshold: " + std::to_string(quorum_threshold) + "\n";

	map<name, asset> votes = b_itr->options;
	asset votes_pass = votes.at("pass"_n);
	asset votes_fail = votes.at("fail"_n);
	debug_str = debug_str + "Votes passing: " + votes_pass.to_string() + "\n";
	debug_str = debug_str + "Votes failing: " + votes_fail.to_string() + "\n";

	if (b_itr->total_voters > quorum_threshold && votes_pass > votes_fail) {
		debug_str = debug_str + "Executing transaction\n";
		//prop.trxs.at("exec_on_approval").send(current_block_time().to_time_point().sec_since_epoch(), get_self());		
	}

	debug (debug_str);
}

void hyphadao::qualify_proposer (const name& proposer) {
	// Should we require that users hold Hypha before they are allowed to propose?  Disabled for now.

	// check (bank.holds_hypha (proposer), "Proposer: " + proposer.to_string() + " does not hold HYPHA.");
}