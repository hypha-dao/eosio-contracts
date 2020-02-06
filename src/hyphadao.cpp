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
	c.assets		= assets;
	c.time_points	= time_points;
	c.ints			= ints;
	c.floats		= floats;
	c.trxs			= trxs;

	config_s.set (c, get_self());

	// validate for required configurations
    string required_names[]{ "hypha_token_contract", "seeds_token_contract", "telos_decide_contract", "last_ballot_id"};
    for (int i{ 0 }; i < std::size(required_names); i++) {
		check (c.names.find(required_names[i]) != c.names.end(), "name configuration: " + required_names[i] + " is required but not provided.");
	}
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

void hyphadao::create (const name&						scope,
						const map<string, name> 		names,
						const map<string, string>       strings,
						const map<string, asset>        assets,
						const map<string, time_point>   time_points,
						const map<string, uint64_t>     ints,
						const map<string, float>        floats,
						const map<string, transaction>  trxs)
{
	const name owner = names.at("owner");

	require_auth (owner);
	qualify_proposer (owner);

	object_table o_t (get_self(), scope.value);
	o_t.emplace (get_self(), [&](auto &o) {
		o.id                       	= o_t.available_primary_key();
		o.names                    	= names;
		o.strings                  	= strings;
		o.assets                  	= assets;
		o.time_points              	= time_points;
		o.ints                     	= ints;
		o.floats                   	= floats;
		o.trxs                     	= trxs;

		if (scope == "proposal"_n) {
			o.names["ballot_id"]		= register_ballot (owner, strings);

			/* default trx_action_account to hyphadaomain */
			if (names.find("trx_action_contract") == names.end()) {
				o.names["trx_action_contract"] = get_self();
			}

			if (names.find("trx_action_name") != names.end()) {
				// this transaction executes if the proposal passes
				transaction trx (time_point_sec(current_time_point())+ (60 * 60 * 24 * 35));
				trx.actions.emplace_back(
					permission_level{get_self(), "active"_n}, 
					o.names.at("trx_action_contract"), o.names.at("trx_action_name"), 
					std::make_tuple(o.id));
				trx.delay_sec = 0;
				o.trxs["exec_on_approval"]      = trx;      
			}	
		} // else if (scope == "role"_n) { role logic/business rules }		
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

// void hyphadao::approved (const name& scope, const uint64_t& id) {}

void hyphadao::newrole (const uint64_t& proposal_id) {

   	require_auth (get_self());
	change_scope ("proposal"_n, id, "proparchive"_n, false);
	change_scope ("proposal"_n, id, "role"_n, true);
}

void hyphadao::addperiod (const time_point& start_date, const time_point& end_date, const string& phase) {
	bank.addperiod (start_date, end_date, phase);
}

void hyphadao::assign ( const uint64_t& 		proposal_id) {

   	require_auth (get_self());

	object_table o_t (get_self(), "proposal"_n.value);
	auto o_itr = o_t.find(proposal_id);
	check (o_itr != o_t.end(), "Scope: " + "proposal"_n.to_string() + "; Object ID: " + std::to_string(proposal_id) + " does not exist.");

	object_table o_t (get_self(), "assignment"_n.value);
	auto sorted_by_assigned = assignment_t.get_index<"byowner"_n>();
	auto a_itr = sorted_by_assigned.begin();
	while (a_itr != sorted_by_assigned.end()) {
		check (! (a_itr->ints.at("role_id") == o_itr->ints.at("role_id") && a_itr->names.at("assigned_account") == o_itr->names.at("assigned_account"), 
			"Assigned account already has this role. Assigned account: " + o_itr->names.at("assigned_account").to_string() + "; Role ID: " + std::to_string(o_itr->ints.at("role_id")));    
		a_itr++;
	}

	change_scope ("proposal"_n, id, "proparchive"_n, false);
	change_scope ("proposal"_n, id, "assignment"_n, true);
}

void hyphadao::makepayout (const uint64_t&        proposal_id) {

	require_auth (get_self());

	object_table o_t (get_self(), "proposal"_n.value);
	auto o_itr = o_t.find(proposal_id);
	check (o_itr != o_t.end(), "Scope: " + "proposal"_n.to_string() + "; Object ID: " + std::to_string(proposal_id) + " does not exist.");

	string memo { "One time payout for Hypha Contribution. Proposal ID: " + std::to_string(proposal_id) };
	debug (" HYPHA payout: " + o_itr->assets.at("hypha_amount").to_string());

	if (o_itr->assets.at("hypha_amount").amount > 0) {
		debug ("Making a payment to : " + o_itr->names.at("recipient").to_string());
		bank.makepayment (-1, o_itr->names.at("recipient"), o_itr->assets.at("hypha_amount"), memo, common::NO_ASSIGNMENT);
	}

	if (o_itr->assets.at("seeds_amount").amount > 0) {
		bank.makepayment (-1, o_itr->names.at("recipient"), o_itr->assets.at("seeds_amount"), memo, common::NO_ASSIGNMENT);
	}
	
	if (o_itr->assets.at("hvoice_amount").amount > 0) {
		config_table      config_s (get_self(), get_self().value);
   		Config c = config_s.get_or_create (get_self(), Config());  

		action(	
			permission_level{get_self(), "active"_n}, 
			c.names.at("telos_decide_contract"), "mint"_n, 
			make_tuple(o_itr->names.at("recipient"), o_itr->assets.at("hvoice_amount"), memo
		)).send();
	}

	change_scope ("proposal"_n, id, "proparchive"_n, true);
}

void hyphadao::eraseobj (	const name& scope, 
						  	const uint64_t& id) {
	require_auth (get_self());
	object_table o_t (get_self(), scope.value);
	auto o_itr = o_t.find(id);
	check (o_itr != o_t.end(), "Scope: " + scope.to_string() + "; Object ID: " + std::to_string(id) + " does not exist.");
	o_t.erase (i_iter);
}

void hyphadao::closeprop(const uint64_t& proposal_id) {

	object_table o_t (get_self(), "proposal"_n.value);
	auto o_itr = o_t.find(id);
	check (o_itr != o_t.end(), "Scope: " + scope.to_string() + "; Object ID: " + std::to_string(id) + " does not exist.");
	auto prop = *o_itr;

	config_table      config_s (get_self(), get_self().value);
   	Config c = config_s.get_or_create (get_self(), Config());  

	trailservice::trail::ballots_table b_t (c.names.at("telos_decide_contract"), c.names.at("telos_decide_contract").value);
	auto b_itr = b_t.find (prop.names.at("ballot_id").value);
	check (b_itr != b_t.end(), "ballot_id: " + prop.names.at("ballot_id").to_string() + " not found.");

	trailservice::trail::treasuries_table t_t (c.names.at("telos_decide_contract"), c.names.at("telos_decide_contract").value);
	auto t_itr = t_t.find (common::S_HVOICE.code().raw());
	check (t_itr != t_t.end(), "Treasury: " + common::S_HVOICE.code().to_string() + " not found.");

	asset quorum_threshold = adjust_asset(t_itr->supply, 0.20000000);
	map<name, asset> votes = b_itr->options;
	asset votes_pass = votes.at("pass"_n);
	asset votes_fail = votes.at("fail"_n);

	string debug_str = " Total Vote Weight: " + b_itr->total_raw_weight.to_string() + "\n";
	debug_str = debug_str + " Total Supply: " + t_itr->supply.to_string() + "\n"; 
	debug_str = debug_str + " Quorum Threshold: " + quorum_threshold.to_string() + "\n";	
	debug_str = debug_str + " Votes Passing: " + votes_pass.to_string() + "\n";
	debug_str = debug_str + " Votes Failing: " + votes_fail.to_string() + "\n";

	if (b_itr->total_raw_weight >= quorum_threshold && 			// must meet quorum
		adjust_asset(votes_pass, 0.2500000000) > votes_fail) {  // must have 80% of the vote power
		debug_str = debug_str + "Executing transaction\n";
		prop.trxs.at("exec_on_approval").send(current_block_time().to_time_point().sec_since_epoch(), get_self());		
	}

	debug_str = debug_str + string ("Ballot ID read from prop for closing ballot: " + prop.names.at("ballot_id").to_string() + "\n");
	action (
		permission_level{get_self(), "active"_n},
		c.names.at("telos_decide_contract"), "closevoting"_n,
		std::make_tuple(prop.names.at("ballot_id"), true))
	.send();

	debug (debug_str);
}

void hyphadao::qualify_proposer (const name& proposer) {
	// Should we require that users hold Hypha before they are allowed to propose?  Disabled for now.

	// check (bank.holds_hypha (proposer), "Proposer: " + proposer.to_string() + " does not hold HYPHA.");
}

void hyphadao::payassign (const uint64_t& assignment_id, const uint64_t& period_id) {

	object_table o_t_assignment (get_self(), "assignment"_n);
	auto a_itr = o_t_assignment.find (assignment_id);
	check (a_itr != o_t_assignment.end(), "Cannot pay assignment. Assignment ID does not exist: " + std::to_string(assignment_id));

	require_auth (a_itr->names.at("assigned_account"));

	object_table o_t_role (get_self(), "role"_n);
	auto r_itr = o_t_role.find (a_itr->ints.at("role_id"));
	check (r_itr != o_t_role.end(), "Cannot pay assignment. Role ID does not exist: " + std::to_string(a_itr->role_id));

	// Check that the assignment has not been paid for this period yet
	asspay_table asspay_t (get_self(), get_self().value);
	auto period_index = asspay_t.get_index<"byperiod"_n>();
	auto per_itr = period_index.find (period_id);
	while (per_itr->period_id == period_id && per_itr != period_index.end()) {
		check (per_itr->assignment_id != assignment_id, "Assignment ID has already been paid for this period. Period ID: " +
			std::to_string(period_id) + "; Assignment ID: " + std::to_string(assignment_id));
		per_itr++;
	}

	// Check that the period has elapsed
	auto p_itr = bank.period_t.find (period_id);
	check (p_itr != bank.period_t.end(), "Cannot make payment. Period ID not found: " + std::to_string(period_id));
	check (p_itr->end_date.sec_since_epoch() < current_block_time().to_time_point().sec_since_epoch(), 
		"Cannot make payment. Period ID " + std::to_string(period_id) + " has not closed yet.");

	// debug ( "Assignment created date : " + a_itr->created_date.to_string() + "; Seconds    : " + std::to_string(a_itr->created_date.sec_since_epoch()));
	// debug ( "Period end              : " + p_itr->end_date.to_string() + ";  Seconds: " + std::to_string(p_itr->end_date.sec_since_epoch()));

	// debug ( "Assignment created date Seconds    : " + std::to_string(a_itr->created_date.sec_since_epoch()));
	// debug ( "Period end Seconds : " + std::to_string(p_itr->end_date.sec_since_epoch()));

	// check that the creation date of the assignment is before the end of the period
	check (a_itr->time_points.at("created_date").sec_since_epoch() < p_itr->end_date.sec_since_epoch(), 
		"Cannot make payment to assignment. Assignment was not approved before this period.");

	// check that pay period is between (inclusive) the start and end period of the role and the assignment
	check (a_itr->ints.at("start_period") <= period_id && a_itr->ints.at("end_period") >= period_id, "For assignment, period ID must be between " +
		std::to_string(a_itr->ints.at("start_period")) + " and " + std::to_string(a_itr->ints.at("end_period")) + " (inclusive). You tried: " + std::to_string(period_id));

	check (r_itr->ints.at("start_period") <= period_id && r_itr->ints.at("end_period") >= period_id, "For role, period ID must be between " +
		std::to_string(r_itr->ints.at("start_period")) + " and " + std::to_string(r_itr->ints.at("end_period")) + " (inclusive). You tried: " + std::to_string(period_id));

	float time_share_calc = a_itr->floats.at("time_share");

	// pro-rate the payout if the assignment was created 
	if (a_itr->time_points.at("created_date").sec_since_epoch() > p_itr->time_points.at("start_date").sec_since_epoch()) {
		time_share_calc = time_share_calc * (float) ( (float) p_itr->time_points.at("end_date").sec_since_epoch() - a_itr->time_points.at("created_date").sec_since_epoch()) / 
							( (float) p_itr->time_points.at("end_date").sec_since_epoch() - p_itr->time_points.at("start_date").sec_since_epoch());
	}

	asset hypha_payment = adjust_asset(r_itr->assets.at("hypha_salary"), time_share_calc);
	asset seeds_payment = adjust_asset(r_itr->assets.at("seeds_salary"), time_share_calc);
	asset voice_payment = adjust_asset(r_itr->assets.at("voice_salary"), time_share_calc);

	asspay_t.emplace (contract, [&](auto &a) {
		a.ass_payment_id        = asspay_t.available_primary_key();
		a.assignment_id         = assignment_id;
		a.recipient             = a_itr->names.at("assigned_account"),
		a.period_id             = period_id;
		a.payment_date          = current_block_time().to_time_point();
		a.payments.push_back (hypha_payment);
		a.payments.push_back (seeds_payment);
		a.payments.push_back (voice_payment);
	});

	bank.makepayment (period_id, a_itr->names.at("assigned_account"), hypha_payment, 
		"Payment for role " + std::to_string(a_itr->ints.at("role_id")) + "; Period ID: " + std::to_string(period_id),
		assignment_id);

	bank.makepayment (period_id, a_itr->assigned_account, seeds_payment, 
		"Payment for role " + std::to_string(a_itr->ints.at("role_id")) + "; Period ID: " + std::to_string(period_id),
		assignment_id);

	bank.makepayment (period_id, a_itr->assigned_account, voice_payment, 
		"Payment for role " + std::to_string(a_itr->ints.at("role_id")) + "; Period ID: " + std::to_string(period_id),
		assignment_id);
}
