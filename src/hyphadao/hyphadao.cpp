#include <hyphadao.hpp>

void hyphadao::reset () {
	board.reset ();
	holocracy.reset ();
	bank.reset ();

	proposal_table p_t (get_self(), get_self().value);
	auto  p_itr = p_t.begin();
	while (p_itr != p_t.end()) {
		p_itr = p_t.erase (p_itr);
	}

	roleprop_table rp_t (get_self(), get_self().value);
	auto rp_itr = rp_t.begin();
	while (rp_itr != rp_t.end()) {
		rp_itr = rp_t.erase (rp_itr);
	}

	payoutprop_table pp_t (get_self(), get_self().value);
	auto pp_itr = pp_t.begin();
	while (pp_itr != pp_t.end()) {
		pp_itr = pp_t.erase (pp_itr);
	}

	assprop_table ap_t (get_self(), get_self().value);
	auto ap_itr = ap_t.begin();
	while (ap_itr != ap_t.end()) {
		ap_itr = ap_t.erase (ap_itr);
	}
}

void hyphadao::resetperiods () {
	bank.reset_periods();
}

// void hyphadao::copyassprop () {
// 	require_auth (get_self());
// 	assprop_table_bu assprop_bu_t (get_self(), get_self().value);
// 	assprop_table assprop_t (get_self(), get_self().value);

// 	auto a_itr = assprop_t.begin();
// 	while (a_itr != assprop_t.end()) {
// 		assprop_bu_t.emplace (get_self(), [&](auto& a) {
// 			a.proposal_id 		= a_itr->proposal_id;
// 			a.ballot_id			= a_itr->ballot_id;
// 			a.proposer			= a_itr->proposer;
// 			a.assigned_account 	= a_itr->assigned_account;
// 			a.role_id			= a_itr->role_id;
// 			a.info_url			= a_itr->info_url;
// 			a.notes 			= a_itr->notes;
// 			a.start_period 		= a_itr->start_period;
// 			a.time_share 		= a_itr->time_share;
// 			a.status	 		= a_itr->status;
// 			a.trx 				= a_itr->trx;
// 			a.created_date 		= a_itr->created_date;
// 			a.executed_date		= a_itr->executed_date;
// 		});
// 		a_itr = assprop_t.erase (a_itr);
// 	}
// }

// void hyphadao::copyroleprop () {
// 	require_auth (get_self());
// 	roleprop_table_bu roleprop_bu_t (get_self(), get_self().value);
// 	roleprop_table roleprop_t (get_self(), get_self().value);

// 	auto r_itr = roleprop_t.begin();
// 	while (r_itr != roleprop_t.end()) {
// 		roleprop_bu_t.emplace (get_self(), [&](auto& r) {
// 			r.proposal_id 	= r_itr->proposal_id;
// 			r.ballot_id 	= r_itr->ballot_id;
// 			r.proposer 	= r_itr->proposer;
// 			r.role_name 	= r_itr->role_name;
// 			r.info_url 	= r_itr->info_url;
// 			r.description 	= r_itr->description;
// 			r.hypha_salary 	= r_itr->hypha_salary;
// 			r.preseeds_salary 	= r_itr->preseeds_salary;
// 			r.voice_salary 	= r_itr->voice_salary;
// 			r.status 	= r_itr->status;
// 			r.trx 	= r_itr->trx;
// 			r.created_date  = r_itr->created_date;
// 			r.executed_date	= r_itr->executed_date; 
// 		});
// 		r_itr = roleprop_t.erase (r_itr);
// 	}
// }

void hyphadao::copyroleback () {
	require_auth (get_self());
	roleprop_table_bu roleprop_bu_t (get_self(), get_self().value);
	roleprop_table roleprop_t (get_self(), get_self().value);

	auto r_itr = roleprop_bu_t.begin();
	while (r_itr != roleprop_bu_t.end()) {
		roleprop_t.emplace (get_self(), [&](auto& r) {
			r.proposal_id 	= r_itr->proposal_id;
			r.ballot_id 	= r_itr->ballot_id;
			r.proposer 	= r_itr->proposer;
			r.role_name 	= r_itr->role_name;
			r.info_url 	= r_itr->info_url;
			r.description 	= r_itr->description;
			r.hypha_salary 	= r_itr->hypha_salary;
			r.preseeds_salary 	= r_itr->preseeds_salary;
			r.voice_salary 	= r_itr->voice_salary;
			r.status 	= r_itr->status;
			r.trx 	= r_itr->trx;
			r.created_date  = r_itr->created_date;
			r.executed_date	= r_itr->executed_date; 
			r.start_period = 3;
			r.end_period = 16;
		});
		r_itr = roleprop_bu_t.erase (r_itr);
	}
}

void hyphadao::init () {
	board.init ();
	inithvoice ( string { "https://joinseeds.com" });
}

void hyphadao::setconfig ( const name&     	hypha_token_contract,
							const name& 	trail_contract) {

   	require_auth (get_self());
	config_table      config_s (get_self(), get_self().value);
   	DAOConfig c = config_s.get_or_create (get_self(), DAOConfig());   
	c.trail_contract 	= trail_contract;
	config_s.set (c, get_self());
   	bank.set_config (hypha_token_contract, hypha_token_contract);
}

void hyphadao::setvconfig (const uint8_t& max_board_seats,
                            const uint8_t&  open_seats,
                            const uint32_t& holder_quorum_divisor,
                            const uint32_t& board_quorum_divisor,
                            const uint32_t& issue_duration,
                            const uint32_t& start_delay,
                            const uint32_t& election_frequency) {

    board.setvconfig (max_board_seats, open_seats, holder_quorum_divisor, board_quorum_divisor,
						issue_duration, start_delay, election_frequency);
}
						

uint64_t hyphadao::register_ballot (const name& proposer,
									const string& info_url) {
	require_auth(proposer);
	qualify_proposer(proposer);

	config_table      config_s (get_self(), get_self().value);
   	DAOConfig c = config_s.get_or_create (get_self(), DAOConfig());  

	ballots_table ballots(c.trail_contract, c.trail_contract.value);
	uint64_t next_ballot_id = ballots.available_primary_key();
	uint32_t begin_time = current_block_time().to_time_point().sec_since_epoch() + board.get_config().start_delay;
	// uint32_t end_time = begin_time + 259'200; //board.get_config().issue_duration;
	uint32_t end_time = begin_time + board.get_config().issue_duration;

   	action(permission_level{get_self(), "active"_n}, c.trail_contract, "regballot"_n, make_tuple(
		get_self(),
		uint8_t(0), 			// NOTE: makes a proposal on Trail
		common::S_HVOICE,
		begin_time,
      	end_time,
      	info_url
	)).send();

	return next_ballot_id;
}

void hyphadao::proposerole (const name& proposer,
                           const string& role_name,
                           const string& info_url,
                           const string& description,
                           const asset& hypha_salary,
                           const asset& preseeds_salary,
                           const asset& voice_salary,
                           const uint64_t& start_period,
                           const uint64_t& end_period) {

    require_auth (proposer);
	qualify_proposer (proposer);

	roleprop_table rp_t (get_self(), get_self().value);
	rp_t.emplace (get_self(), [&](auto &p) {
		p.proposal_id		= rp_t.available_primary_key();

		transaction trx (time_point_sec(current_time_point())+ (60 * 60 * 24 * 35));
		trx.actions.emplace_back(permission_level{get_self(), "owner"_n}, 
			get_self(), "newrole"_n, 
			std::make_tuple(p.proposal_id));
		trx.delay_sec = 0;

		p.trx				= trx;
		p.ballot_id			= register_ballot (proposer, info_url);

		proposal_table proposal_t (get_self(), get_self().value);
		proposal_t.emplace (get_self(), [&](auto &gen_p) {
			gen_p.proposal_id 	= proposal_t.available_primary_key();
			gen_p.ballot_id		= p.ballot_id;
			gen_p.trx			= trx;

			// issue deferred trx to close the proposal
			defcloseprop (gen_p.proposal_id);
		});

		p.proposer			= proposer;
		p.role_name			= role_name;
		p.info_url 			= info_url;
		p.description		= description;
		p.trx				= trx;
		p.hypha_salary		= hypha_salary;
		p.preseeds_salary	= preseeds_salary;
		p.voice_salary		= voice_salary;
		p.start_period		= start_period;
		p.end_period		= end_period;
		p.created_date		= current_block_time().to_time_point();
	});
}

void hyphadao::newrole (const uint64_t& proposal_id) {

   	require_auth (get_self());

	roleprop_table rp_t (get_self(), get_self().value);
	auto rp_itr = rp_t.find (proposal_id);
	check (rp_itr != rp_t.end(), "New role proposal is not found.");

	rp_t.modify (rp_itr, get_self(), [&](auto &r) {
		r.status 		= common::EXECUTED;
		r.executed_date	= current_block_time().to_time_point();
	});

	holocracy.newrole (rp_itr->role_name, 
						rp_itr->description, 
						rp_itr->hypha_salary, 
						rp_itr->preseeds_salary, 
						rp_itr->voice_salary,
						rp_itr->start_period,
						rp_itr->end_period);
}

void hyphadao::addperiod (const time_point& start_date, const time_point& end_date, const string& phase) {
	bank.addperiod (start_date, end_date, phase);
}

void hyphadao::propassign (const name&      proposer,
                        const name&        	assigned_account,
                        const uint64_t&     role_id,
                        const string&      	info_url,
                        const string&      	notes,
                        const uint64_t&  	start_period,
						const uint64_t& 	end_period,
                        const float&       	time_share) {

    require_auth (proposer);
    qualify_proposer (proposer);
	assprop_table assprop_t (get_self(), get_self().value);
	assprop_t.emplace (get_self(), [&](auto &a) {
		a.proposal_id		= assprop_t.available_primary_key();

		// transaction must be approved within 35 days (may need to make this a config item)
		transaction trx (time_point_sec(current_time_point())+ (60 * 60 * 24 * 35));
		trx.actions.emplace_back(permission_level{get_self(), "owner"_n}, 
			get_self(), "assign"_n, 
			std::make_tuple(a.proposal_id));
		trx.delay_sec = 0;

		a.trx 				= trx;
		a.ballot_id			= register_ballot (proposer, info_url);

		proposal_table proposal_t (get_self(), get_self().value);
		proposal_t.emplace (get_self(), [&](auto &gen_p) {
			gen_p.proposal_id 	= proposal_t.available_primary_key();
			gen_p.ballot_id		= a.ballot_id;
			gen_p.trx			= trx;
		});

		a.proposer			= proposer;
		a.assigned_account	= assigned_account;
		a.role_id			= role_id;
		a.info_url			= info_url;
		a.notes				= notes;
		a.start_period 		= start_period;
		a.end_period		= end_period;
		a.time_share 		= time_share;
		a.status 			= common::OPEN;
		a.created_date		= current_block_time().to_time_point();
	});
}

void hyphadao::assign ( const uint64_t& 		proposal_id) {

   	require_auth (get_self());

	assprop_table assprop_t (get_self(), get_self().value);
	auto ass_itr = assprop_t.find (proposal_id);
	check (ass_itr != assprop_t.end(), "Assignment proposal is not found.");

	assprop_t.modify (ass_itr, get_self(), [&](auto& p) {
		p.status 		= common::EXECUTED;
		p.executed_date	= current_block_time().to_time_point();
	});

	holocracy.newassign (ass_itr->assigned_account,
						ass_itr->role_id,
						ass_itr->info_url,
						ass_itr->notes,
						ass_itr->start_period,
						ass_itr->end_period,
						ass_itr->time_share);
}

void hyphadao::payassign(const uint64_t& assignment_id, const uint64_t& period_id) {
	holocracy.payassign (assignment_id, period_id);
}

void hyphadao::proppayout (const name&   		proposer,
                           const name&       	recipient,
                           const string&     	notes,
						   const string& 		info_url,
                           const asset&      	hypha_value,
                           const asset&      	preseeds_value, 
						   const asset& 		voice_value,
                           const time_point& 	contribution_date) {
				
    require_auth (proposer);
   	qualify_proposer (proposer);

	payoutprop_table payout_t (get_self(), get_self().value);
	payout_t.emplace (get_self(), [&](auto &p) {
		p.proposal_id 		= payout_t.available_primary_key();

		transaction trx (time_point_sec(current_time_point()) + (60 * 60 * 24 * 35));
		trx.actions.emplace_back(permission_level{get_self(), "owner"_n}, 
			get_self(), "makepayout"_n, 
			std::make_tuple(p.proposal_id));
		trx.delay_sec = 0;

		p.ballot_id			= register_ballot (proposer, info_url);
		p.trx				= trx;

		proposal_table proposal_t (get_self(), get_self().value);
		proposal_t.emplace (get_self(), [&](auto &gen_p) {
			gen_p.proposal_id 	= proposal_t.available_primary_key();
			gen_p.ballot_id		= p.ballot_id;
			gen_p.trx			= trx;

			// issue deferred trx to close the proposal
			defcloseprop (gen_p.proposal_id);
		});

		p.recipient			= recipient;
		p.notes 			= notes;
		p.hypha_value		= hypha_value;
		p.preseeds_value 	= preseeds_value;
		p.voice_value 		= voice_value;
		p.status 			= common::OPEN;
		p.contribution_date	= contribution_date;
		p.created_date		= current_block_time().to_time_point();
	});
}

void hyphadao::makepayout (const uint64_t&        proposal_id) {

	require_auth (get_self());
	payoutprop_table payout_t (get_self(), get_self().value);
	auto p_itr = payout_t.find (proposal_id);
	check (p_itr != payout_t.end(), "Payout proposal is not found.");

	payout_t.modify (p_itr, get_self(), [&](auto &p) {
		p.status 		= 	common::EXECUTED;
		p.executed_date	= 	current_block_time().to_time_point();
	});

	string memo { "One time payout for Hypha DAO Contribution: " + std::to_string(proposal_id) };
	if (p_itr->hypha_value.amount > 0) {
		bank.makepayment (-1, p_itr->recipient, p_itr->hypha_value, memo, common::NO_ASSIGNMENT);
	}

	if (p_itr->preseeds_value.amount > 0) {
		bank.makepayment (-1, p_itr->recipient, p_itr->preseeds_value, memo, common::NO_ASSIGNMENT);
	}
	
	if (p_itr->voice_value.amount > 0) {
		config_table      config_s (get_self(), get_self().value);
   		DAOConfig c = config_s.get_or_create (get_self(), DAOConfig());  

		action(permission_level{get_self(), "active"_n}, c.trail_contract, "issuetoken"_n, make_tuple(
			get_self(),
			p_itr->recipient, 			
			p_itr->voice_value,
			false
		)).send();
	}
}

void hyphadao::eraseprop (const uint64_t& proposal_id) {
	require_auth (get_self());
	proposal_table props(get_self(), get_self().value);
	auto i_iter = props.find(proposal_id);
	check(i_iter != props.end(), "prop not found");
	props.erase (i_iter);
}

void hyphadao::defcloseprop (const uint64_t& proposal_id) {

	// maximum of 35 days (may need to make this a config item)
	transaction trx (time_point_sec(current_time_point())+ (60 * 60 * 24 * 35));
	trx.actions.emplace_back(permission_level{get_self(), "owner"_n}, 
		get_self(), "closeprop"_n, 
		std::make_tuple(proposal_id));
	trx.delay_sec = board.get_config().issue_duration;
	trx.send(current_block_time().to_time_point().sec_since_epoch(), get_self());
}

void hyphadao::closeprop(const uint64_t& proposal_id) {

	proposal_table props(get_self(), get_self().value);
	auto i_iter = props.find(proposal_id);
	check(i_iter != props.end(), "prop not found");
	auto prop = *i_iter;

	config_table      config_s (get_self(), get_self().value);
   	DAOConfig c = config_s.get_or_create (get_self(), DAOConfig());  

	ballots_table ballots(c.trail_contract, c.trail_contract.value);
	auto ballot = ballots.get(prop.ballot_id, "ballot does not exist");

	proposals_table proposals(c.trail_contract, c.trail_contract.value);
	auto trail_prop = proposals.get(ballot.reference_id, "proposal not found");

	registries_table registries(c.trail_contract, c.trail_contract.value);
	auto registry = registries.get(trail_prop.no_count.symbol.code().raw(), "registry not found, this shouldn't happen");
	uint32_t total_voters = registry.total_voters;
	
	uint32_t unique_voters = trail_prop.unique_voters;
     
	uint32_t quorum_threshold = total_voters / (board.get_config().holder_quorum_divisor - 1);
	common::PROP_STATE state = common::FAIL;
	if(unique_voters > quorum_threshold)
		state = common::COUNT;
	
	if(state == common::COUNT) {
		if(trail_prop.yes_count > (trail_prop.no_count * 4))
			state = common::PASS;
		else if(trail_prop.yes_count == (trail_prop.no_count * 4))
			state = common::TIE;
		else
			state = common::FAIL;
	}

	if(state == common::PASS) {
		prop.trx.send(current_block_time().to_time_point().sec_since_epoch(), get_self());
	} 

	if(state == common::FAIL || state == common::PASS) {
		props.erase(i_iter);
	}

	if(state == common::TIE) {
		uint64_t next_ballot_id = ballots.available_primary_key();
		uint32_t begin_time = current_block_time().to_time_point().sec_since_epoch() + board.get_config().start_delay;
		uint32_t end_time = begin_time + board.get_config().issue_duration;
		action(permission_level{get_self(), "active"_n}, c.trail_contract, "regballot"_n, make_tuple(
			get_self(),		      	// proposer name
			uint8_t(0), 			// ballot_type uint8_t
			common::S_HVOICE,	   	// voting_symbol symbol
			begin_time,				// begin_time uint32_t
			end_time,				// end_time uint32_t
			prop.info_url			// info_url string
		)).send();

		props.modify(prop, same_payer, [&](auto& i) {
			i.ballot_id = next_ballot_id;
		});
	}

   action(permission_level{get_self(), "active"_n}, c.trail_contract, "closeballot"_n, make_tuple(
		get_self(),
		prop.ballot_id,
		uint8_t(state)
	)).send();
}

void hyphadao::inithvoice(string initial_info_link) {
    require_auth(get_self());

	config_table      config_s (get_self(), get_self().value);
   	DAOConfig c = config_s.get_or_create (get_self(), DAOConfig());
    
    action(permission_level{get_self(), name("active")}, c.trail_contract, name("regtoken"), make_tuple(
		common::INITIAL_HVOICE_MAX_SUPPLY, 	// max_supply
		get_self(), 						// publisher
		initial_info_link 					// info_url
	)).send();

    action(permission_level{get_self(), name("active")}, c.trail_contract, name("initsettings"), make_tuple(
		get_self(), 								// publisher
		common::INITIAL_HVOICE_MAX_SUPPLY.symbol, 	// token_symbol
		common::INITIAL_HVOICE_SETTINGS 			// new_settings
	)).send();

    print("\nHVOICE Registration and Initialization Actions Sent...");
}

void hyphadao::initsteward(const string initial_info_link) {}

void hyphadao::qualify_proposer (const name& proposer) {
	check(board.is_hvoice_holder(proposer) || board.is_steward_holder(proposer), "caller must be a STEWARD or HVOICE holder");
}

void hyphadao::nominate (const name& nominee, const name& nominator) {
	board.nominate (nominee, nominator);
}

void hyphadao::makeelection (const name& holder, const string& info_url) {
	board.makeelection (holder, info_url);
}

void hyphadao::addcand (const name& nominee, const string& info_url) {
	board.addcand (nominee, info_url);
}

void hyphadao::removecand (const name& candidate) {
	board.removecand (candidate);
}

void hyphadao::endelection (const name& holder) {
	board.endelection (holder);
}

void hyphadao::removemember (const name& member_to_remove) {
	board.removemember (member_to_remove);
}