#include <hyphadao.hpp>

void hyphadao::reset () {
	board.reset ();
	holocracy.reset ();
	bank.reset ();
//    voting_configs.remove();

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

void hyphadao::init () {
	board.init ();
	inithvoice ( string { "https://joinseeds.com" });
}

void hyphadao::setconfig ( const name&     hypha_token_contract) {

   	require_auth (get_self());
   	bank.set_config (hypha_token_contract, hypha_token_contract);
}

uint64_t hyphadao::register_ballot (const name& proposer,
									const string& info_url) {
	require_auth(proposer);
	qualify_proposer(proposer);

	ballots_table ballots("eosio.trail"_n, "eosio.trail"_n.value);
	uint64_t next_ballot_id = ballots.available_primary_key();
	uint32_t begin_time = current_block_time().to_time_point().sec_since_epoch() + board.get_config().start_delay;
	uint32_t end_time = begin_time + board.get_config().issue_duration;

   	action(permission_level{get_self(), "active"_n}, "eosio.trail"_n, "regballot"_n, make_tuple(
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
                           const asset& voice_salary) {

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
		});

		p.proposer			= proposer;
		p.role_name			= role_name;
		p.info_url 			= info_url;
		p.description		= description;
		p.trx				= trx;
		p.hypha_salary		= hypha_salary;
		p.preseeds_salary	= preseeds_salary;
		p.voice_salary		= voice_salary;
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
						rp_itr->voice_salary);
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
                        const float&       	time_share) {

    require_auth (proposer);
    qualify_proposer (proposer);
	assprop_table assprop_t (get_self(), get_self().value);
	assprop_t.emplace (get_self(), [&](auto &a) {
		a.proposal_id		= assprop_t.available_primary_key();

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
		bank.makepayment (-1, p_itr->recipient, p_itr->hypha_value, memo);
	}

	if (p_itr->preseeds_value.amount > 0) {
		bank.makepayment (-1, p_itr->recipient, p_itr->preseeds_value, memo);
	}
	
	if (p_itr->voice_value.amount > 0) {
		action(permission_level{get_self(), "active"_n}, "eosio.trail"_n, "issuetoken"_n, make_tuple(
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

void hyphadao::closeprop(const name& holder,
						const uint64_t& proposal_id) {

   	require_auth(holder);
	qualify_proposer(holder);

	proposal_table props(get_self(), get_self().value);
	auto i_iter = props.find(proposal_id);
	check(i_iter != props.end(), "prop not found");
	auto prop = *i_iter;

	ballots_table ballots("eosio.trail"_n, "eosio.trail"_n.value);
	auto ballot = ballots.get(prop.ballot_id, "ballot does not exist");

	proposals_table proposals("eosio.trail"_n, "eosio.trail"_n.value);
	auto trail_prop = proposals.get(ballot.reference_id, "proposal not found");

	registries_table registries("eosio.trail"_n, "eosio.trail"_n.value);
	auto registry = registries.get(trail_prop.no_count.symbol.code().raw(), "registry not found, this shouldn't happen");
	uint32_t total_voters = registry.total_voters;
	
	uint32_t unique_voters = trail_prop.unique_voters;
     
	uint32_t quorum_threshold = total_voters / (board.get_config().board_quorum_divisor - 1);
	common::PROP_STATE state = common::FAIL;
	if(unique_voters > quorum_threshold)
		state = common::COUNT;
	
	if(state == common::COUNT) {
		if(trail_prop.yes_count > trail_prop.no_count)
			state = common::PASS;
		else if(trail_prop.yes_count == trail_prop.no_count)
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
		action(permission_level{get_self(), "active"_n}, "eosio.trail"_n, "regballot"_n, make_tuple(
			get_self(),		      //proposer name
			uint8_t(0), 			//ballot_type uint8_t
			common::S_HVOICE,	   //voting_symbol symbol
			begin_time,				//begin_time uint32_t
			end_time,				//end_time uint32_t
			prop.info_url			//info_url string
		)).send();

		props.modify(prop, same_payer, [&](auto& i) {
			i.ballot_id = next_ballot_id;
		});
	}

   action(permission_level{get_self(), "active"_n}, "eosio.trail"_n, "closeballot"_n, make_tuple(
		get_self(),
		prop.ballot_id,
		uint8_t(state)
	)).send();
}

void hyphadao::inithvoice(string initial_info_link) {
    require_auth(get_self());
    
    action(permission_level{get_self(), name("active")}, name("eosio.trail"), name("regtoken"), make_tuple(
		common::INITIAL_HVOICE_MAX_SUPPLY, //max_supply
		get_self(), //publisher
		initial_info_link //info_url
	)).send();

    action(permission_level{get_self(), name("active")}, name("eosio.trail"), name("initsettings"), make_tuple(
		get_self(), //publisher
		common::INITIAL_HVOICE_MAX_SUPPLY.symbol, //token_symbol
		common::INITIAL_HVOICE_SETTINGS //new_settings
	)).send();

    print("\nHVOICE Registration and Initialization Actions Sent...");
}

void hyphadao::initsteward(const string initial_info_link) {}

void hyphadao::qualify_proposer (const name& proposer) {
	check(board.is_hvoice_holder(proposer) || 
		board.is_steward_holder(proposer), "caller must be a STEWARD or HVOICE holder");
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