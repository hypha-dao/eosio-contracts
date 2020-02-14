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
	// bank.reset_config ();
	require_auth (get_self());
	// config_table      config_s (get_self(), get_self().value);
   	// Config c = config_s.get_or_create (get_self(), Config()); 
	// config_s.remove ();
}

void hyphadao::togglepause () {
	require_auth (get_self());
	config_table      config_s (get_self(), get_self().value);
   	Config c = config_s.get_or_create (get_self(), Config());   
	if (c.ints.find ("paused") == c.ints.end() || c.ints.at("paused") == 0) {
		c.ints["paused"]	= 1;
	} else {
		c.ints["paused"] 	= 0;
	} 	
	config_s.set (c, get_self());
}

void hyphadao::remperiods (const uint64_t& begin_period_id, 
                           const uint64_t& end_period_id) {
	require_auth (get_self());
    bank.remove_periods (begin_period_id, end_period_id);
}

void hyphadao::resetperiods () {
	require_auth (get_self());
	bank.reset_periods();
}

void hyphadao::addowner (const name& scope) {
	require_auth (get_self());
	object_table o_t (get_self(), scope.value);
	auto o_itr = o_t.begin();
	while (o_itr != o_t.end()) {
		o_t.modify (o_itr, get_self(), [&](auto &o) {
			o.names["owner"] = o_itr->names.at("proposer");
		});
		o_itr++;
	}
}

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

	check ( !is_paused(), "Contract is paused for maintenance. Please try again later.");	

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

	check ( !is_paused(), "Contract is paused for maintenance. Please try again later.");	
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
	check ( !is_paused(), "Contract is paused for maintenance. Please try again later.");	
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

			if (names.at("proposal_type") == "role"_n) { 
				// role logic/business rules 
				check (ints.at("fulltime_capacity_x100") > 0, "fulltime_capacity_x100 must be greater than zero. You submitted: " + std::to_string(ints.at("fulltime_capacity_x100")));
				check (assets.at("annual_usd_salary").amount > 0, "annual_usd_salary must be greater than zero. You submitted: " + assets.at("annual_usd_salary").to_string());
			} else if (names.at("proposal_type") == "assignment"_n) {
				check (false, "Assignment proposals are not yet supported. Soon.");
				check (ints.find("role_id") != ints.end(), "Role ID is required when proposal_type is assignment.");
				check (ints.find("time_share_x100") != ints.end(), "time_share_x100 is a required field for assignment proposals.");
				check (ints.at("time_share_x100") > 0 && ints.at("time_share_x100") < 100, "time_share_x100 must be greater than zero and less than or equal to 100.");
				check (ints.find("start_period") != ints.end(), "start_period is a required field for assignment proposals.");
				check (ints.find("end_period") != ints.end(), "end_period is a required field for assignment proposals.");

				o.ints["fk"]	= ints.at("role_id");

				// object_table o_t_role (get_self(), "role"_n.value);
				// auto o_itr_role = o_t_role.find (ints.at("role_id"));
				// check (o_itr_role != o_t_role.end(), "Role ID: " + std::to_string(ints.at("role_id")) + " does not exist.");
				// check (o_itr_role->ints.at("consumed_capacity") + ints.at("time_share_x100") <= o_itr_role->ints.at("fulltime_capacity"), "Role ID: " + 
				// 	std::to_string (ints.at("role_id")) + " cannot support assignment. Full time capacity (x100) is " + std::to_string(o_itr_role->ints.at("fulltime_capacity")) + 
				// 	" and consumed capacity (x100) is " + std::to_string(o_itr_role->ints.at("consumed_capacity")) + "; proposal requests time share (x100) of: " + std::to_string(ints.at("time_share")));
			}
		}
	});      
}

void hyphadao::clrdebugs (const uint64_t& starting_id, const uint64_t& batch_size) {
	check (has_auth ("hyphanewyork"_n) || has_auth(get_self()), "Requires higher permission.");
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

	// proposal_table p_t (get_self(), get_self().value);
	// auto p_itr = p_t.find (proposal_id);
	// check (p_itr != p_t.end(), "Proposal ID: " + std::to_string(proposal_id) + " does not exist.");
	// check (p_itr->trxs.size() > 1, "There are not transactions to execute. Trx map size: " + std::to_string(p_itr->trxs.size()) + "; Proposal ID: " + std::to_string(proposal_id));

	// for (auto trx_itr = p_itr->trxs.begin(); trx_itr != p_itr->trxs.end(); ++trx_itr) {
	// 	// skip the transaction named "exec_on_approval" because that is the one currently executing 
	// 	// debug ( "trx_itr->first	: " + trx_itr->first);
	// 	// debug ( "true/false		: " + std::to_string(trx_itr->first.compare("exec_on_approval") == 0));
	// 	if (trx_itr->first.compare("exec_on_approval") == 0) {
	// 		debug ("Executing transaction	: " + trx_itr->first);
	// 		trx_itr->second.send(current_block_time().to_time_point().sec_since_epoch(), get_self());
	// 	}
	// }
}

// void hyphadao::approved (const name& scope, const uint64_t& id) {}

void hyphadao::newrole (const uint64_t& proposal_id) {

   	require_auth (get_self());
	change_scope ("proposal"_n, proposal_id, "proparchive"_n, false);
	change_scope ("proposal"_n, proposal_id, "role"_n, true);
}

void hyphadao::addperiod (const time_point& start_date, const time_point& end_date, const string& phase) {
	require_auth (get_self());
	bank.addperiod (start_date, end_date, phase);
}

void hyphadao::assign ( const uint64_t& 		proposal_id) {

   	require_auth (get_self());

	object_table o_t_proposal (get_self(), "proposal"_n.value);
	auto o_itr = o_t_proposal.find(proposal_id);
	check (o_itr != o_t_proposal.end(), "Scope: " + "proposal"_n.to_string() + "; Object ID: " + std::to_string(proposal_id) + " does not exist.");

	// Ensure that the owner of this proposer doesn't already have this assignment
	object_table o_t_assignment (get_self(), "assignment"_n.value);
	auto sorted_by_owner = o_t_assignment.get_index<"byowner"_n>();
	auto a_itr = sorted_by_owner.find(o_itr->names.at("owner").value);

	while (a_itr != sorted_by_owner.end() && a_itr->names.at("owner") == o_itr->names.at("owner")) {
		check (! (a_itr->ints.at("role_id") == o_itr->ints.at("role_id") && a_itr->names.at("owner") == o_itr->names.at("owner")), 
			"Proposal owner already has this role. Owner: " + o_itr->names.at("owner").to_string() + "; Role ID: " + std::to_string(o_itr->ints.at("role_id")));    
		a_itr++;
	}

	// Ensure that this proposal would not push the role over it's approved full time capacity
	object_table o_t_role (get_self(), "role"_n.value);
	auto o_itr_role = o_t_role.find (o_itr->ints.at("role_id"));
	check (o_itr_role != o_t_role.end(), "Role ID: " + std::to_string(o_itr->ints.at("role_id")) + " does not exist.");

	// auto sorted_by_role = o_t_assignment.get_index<"byfk"_n>();
	// auto a_itr_by_role = sorted_by_role.find(o_itr->ints.at("fk"));
	// int consumed_capacity = 0;
	// while (a_itr_by_role != sorted_by_role.end() && a_itr_by_role->ints.at("fk") == o_itr->ints.at("fk")) {
	// 	consumed_capacity += a_itr_by_role->ints.at("time_share_x100");
	// }

	// check (consumed_capacity + o_itr->ints.at("time_share_x100") <= o_itr_role->ints.at("fulltime_capacity_x100"), "Role ID: " + 
	// 	std::to_string (o_itr->ints.at("fk")) + " cannot support assignment. Full time capacity (x100) is " + std::to_string(o_itr_role->ints.at("fulltime_capacity_x100")) + 
	// 	" and consumed capacity (x100) is " + std::to_string(consumed_capacity) + "; proposal requests time share (x100) of: " + std::to_string(o_itr->ints.at("time_share_x100")));

	change_scope ("proposal"_n, proposal_id, "proparchive"_n, false);
	change_scope ("proposal"_n, proposal_id, "assignment"_n, true);
}

void hyphadao::makepayout (const uint64_t&        proposal_id) {

	require_auth (get_self());

	object_table o_t (get_self(), "proposal"_n.value);
	auto o_itr = o_t.find(proposal_id);
	check (o_itr != o_t.end(), "Scope: " + "proposal"_n.to_string() + "; Object ID: " + std::to_string(proposal_id) + " does not exist.");

	string memo { "One time payout for Hypha Contribution. Proposal ID: " + std::to_string(proposal_id) };
	// debug (" HYPHA payout: " + o_itr->assets.at("hypha_amount").to_string());

	if (o_itr->assets.at("hypha_amount").amount > 0) {
		// debug ("Making a payment to : " + o_itr->names.at("recipient").to_string());
		bank.makepayment (-1, o_itr->names.at("recipient"), o_itr->assets.at("hypha_amount"), memo, common::NO_ASSIGNMENT, 1);
	}

	if (o_itr->assets.at("seeds_amount").amount > 0) {
		bank.makepayment (-1, o_itr->names.at("recipient"), o_itr->assets.at("seeds_amount"), memo, common::NO_ASSIGNMENT, o_itr->ints.at("bypass_escrow"));
	}
	
	if (o_itr->assets.at("hvoice_amount").amount > 0) {
		bank.makepayment (-1, o_itr->names.at("recipient"), o_itr->assets.at("hvoice_amount"), memo, common::NO_ASSIGNMENT, o_itr->ints.at("bypass_escrow"));
	}

	// 	config_table      config_s (get_self(), get_self().value);
   	// 	Config c = config_s.get_or_create (get_self(), Config());  

	// 	action(	
	// 		permission_level{get_self(), "active"_n}, 
	// 		c.names.at("telos_decide_contract"), "mint"_n, 
	// 		make_tuple(o_itr->names.at("recipient"), o_itr->assets.at("hvoice_amount"), memo
	// 	)).send();
	// }

	change_scope ("proposal"_n, proposal_id, "proparchive"_n, true);
}

void hyphadao::eraseobj (	const name& scope, 
						  	const uint64_t& id) {
	require_auth (get_self());
	object_table o_t (get_self(), scope.value);
	auto o_itr = o_t.find(id);
	check (o_itr != o_t.end(), "Scope: " + scope.to_string() + "; Object ID: " + std::to_string(id) + " does not exist.");
	o_t.erase (o_itr);
}

void hyphadao::closeprop(const uint64_t& proposal_id) {

	check ( !is_paused(), "Contract is paused for maintenance. Please try again later.");	

	object_table o_t (get_self(), "proposal"_n.value);
	auto o_itr = o_t.find(proposal_id);
	check (o_itr != o_t.end(), "Scope: " + "proposal"_n.to_string() + "; Object ID: " + std::to_string(proposal_id) + " does not exist.");
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

	check ( !is_paused(), "Contract is paused for maintenance. Please try again later.");	

	object_table o_t_assignment (get_self(), "assignment"_n.value);
	auto a_itr = o_t_assignment.find (assignment_id);
	check (a_itr != o_t_assignment.end(), "Cannot pay assignment. Assignment ID does not exist: " + std::to_string(assignment_id));

	require_auth (a_itr->names.at("assigned_account"));

	object_table o_t_role (get_self(), "role"_n.value);
	auto r_itr = o_t_role.find (a_itr->ints.at("role_id"));
	check (r_itr != o_t_role.end(), "Cannot pay assignment. Role ID does not exist: " + std::to_string(a_itr->ints.at("role_id")));

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
	if (a_itr->time_points.at("created_date").sec_since_epoch() > p_itr->start_date.sec_since_epoch()) {
		time_share_calc = time_share_calc * (float) ( (float) p_itr->end_date.sec_since_epoch() - a_itr->time_points.at("created_date").sec_since_epoch()) / 
							( (float) p_itr->end_date.sec_since_epoch() - p_itr->start_date.sec_since_epoch());
	}

	asset hypha_payment = adjust_asset(r_itr->assets.at("hypha_salary"), time_share_calc);
	asset seeds_payment = adjust_asset(r_itr->assets.at("seeds_salary"), time_share_calc);
	asset voice_payment = adjust_asset(r_itr->assets.at("voice_salary"), time_share_calc);

	asspay_t.emplace (get_self(), [&](auto &a) {
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
		assignment_id, a_itr->ints.at("bypass_escrow"));

	bank.makepayment (period_id, a_itr->names.at("assigned_account"), seeds_payment, 
		"Payment for role " + std::to_string(a_itr->ints.at("role_id")) + "; Period ID: " + std::to_string(period_id),
		assignment_id, a_itr->ints.at("bypass_escrow"));

	bank.makepayment (period_id, a_itr->names.at("assigned_account"), voice_payment, 
		"Payment for role " + std::to_string(a_itr->ints.at("role_id")) + "; Period ID: " + std::to_string(period_id),
		assignment_id, a_itr->ints.at("bypass_escrow"));
}
