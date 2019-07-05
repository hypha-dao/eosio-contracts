#include <hyphadao.hpp>

void hyphadao::reset () {
	board.reset ();
	holocracy.reset ();
	bank.reset ();
//    voting_configs.remove();
}

void hyphadao::init () {
	board.init ();
	inithvoice ( string { "https://joinseeds.com" });
}

void hyphadao::setconfig ( const name&     hypha_token_contract) {

   	require_auth (get_self());
   	bank.set_config (hypha_token_contract, hypha_token_contract);
}

void hyphadao::proposerole (const name& proposer,
                           const name& role_name,
                           const string& info_url,
                           const string& description,
                           const asset& hypha_salary,
                           const asset& preseeds_salary,
                           const asset& voice_salary) {

    require_auth (proposer);
	qualify_proposer (proposer);
	roleprop_table rp_t (get_self(), get_self().value);
	uint64_t proposal_pk = rp_t.available_primary_key();
	rp_t.emplace (get_self(), [&](auto &p) {
		p.proposal_id		= proposal_pk;
		p.proposer			= proposer;
		p.role_name			= role_name;
		p.info_url 			= info_url;
		p.description		= description;
		p.hypha_salary		= hypha_salary;
		p.preseeds_salary	= preseeds_salary;
		p.voice_salary		= voice_salary;
		p.created_date		= current_block_time().to_time_point();
	});

	transaction trx (time_point_sec(current_time_point())+ (60 * 60 * 24 * 35));
	trx.actions.emplace_back(permission_level{get_self(), "owner"_n}, 
		get_self(), "newrole"_n, 
		std::make_tuple(proposal_pk));
	trx.delay_sec = 0;

   	makeprop (proposer, info_url, role_name, description, common::ROLE_PROPOSAL, proposal_pk, trx);
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

void hyphadao::propassign (const name&       proposer,
                        const name&        assigned_account,
                        const name&        role_name,
                        const string&      info_url,
                        const string&      notes,
                        const uint64_t&  start_period,
                        const float&       time_share) {
    require_auth (proposer);
    qualify_proposer (proposer);
	assprop_table assprop_t (get_self(), get_self().value);
	uint64_t proposal_pk = assprop_t.available_primary_key();
	assprop_t.emplace (get_self(), [&](auto &a) {
		a.proposal_id		= proposal_pk;
		a.proposer			= proposer;
		a.assigned_account	= assigned_account;
		a.role_name			= role_name;
		a.info_url			= info_url;
		a.notes				= notes;
		a.start_period 		= start_period;
		a.time_share 		= time_share;
		a.status 			= common::OPEN;
		a.created_date		= current_block_time().to_time_point();
	});
	
	transaction trx (time_point_sec(current_time_point())+ (60 * 60 * 24 * 35));
	trx.actions.emplace_back(permission_level{get_self(), "owner"_n}, 
		get_self(), "assign"_n, 
		std::make_tuple(proposal_pk));
	trx.delay_sec = 0;

  	makeprop (proposer, info_url, role_name, notes, common::ASS_PROPOSAL, proposal_pk, trx);
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
						ass_itr->role_name,
						ass_itr->info_url,
						ass_itr->notes,
						ass_itr->start_period,
						ass_itr->time_share);
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
	uint64_t proposal_pk = payout_t.available_primary_key();
	payout_t.emplace (get_self(), [&](auto &p) {
		p.proposal_id 		= proposal_pk;
		p.recipient			= recipient;
		p.notes 			= notes;
		p.hypha_value		= hypha_value;
		p.preseeds_value 	= preseeds_value;
		p.voice_value 		= voice_value;
		p.status 			= common::OPEN;
		p.contribution_date	= contribution_date;
		p.created_date		= current_block_time().to_time_point();
	});

	transaction trx (time_point_sec(current_time_point())+ (60 * 60 * 24 * 35));
	trx.actions.emplace_back(permission_level{get_self(), "owner"_n}, 
		get_self(), "makepayout"_n, 
		std::make_tuple(proposal_pk));
	trx.delay_sec = 0;

   	makeprop (proposer, info_url, recipient, notes, common::PAYOUT_PROPOSAL, proposal_pk, trx);
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

	// bank.
}

void hyphadao::makeprop(const name& proposer, 
                        const string& info_url,
                        const name& proposal_name,
                        const string& notes,
						const uint8_t& proposal_type,
						const uint64_t& proposal_fk,
                        const transaction& trx) {

   	require_auth(proposer);

	// check(is_hvoice_holder(proposer) || is_steward_holder(proposer), "caller must be a STEWARD or HVOICE holder");

	// ballots_table ballots("eosio.trail"_n, "eosio.trail"_n.value);
	//uint64_t next_ballot_id = ballots.available_primary_key();
	// uint32_t begin_time = current_block_time().to_time_point().sec_since_epoch() + _config.start_delay;
	// uint32_t end_time = begin_time + _config.prop_duration;

   	// action(permission_level{get_self(), "active"_n}, "eosio.trail"_n, "regballot"_n, make_tuple(
	// 	get_self(),
	// 	uint8_t(0), 			// NOTE: makes a proposal on Trail
	// 	common::S_HVOICE,
	// 	begin_time,
    //   end_time,
    //   info_url
	// )).send();
	
	proposal_table proposals(get_self(), get_self().value);
	proposals.emplace(get_self(), [&](auto& p) {
		p.proposal_id		= proposals.available_primary_key();
		p.proposer        	= proposer;
		p.proposal_name   	= proposal_name;
      	p.notes           	= notes;
	//	p.ballot_id       	= next_ballot_id;
		p.proposal_type		= proposal_type;
		p.proposal_fk		= proposal_fk;
		p.transaction     	= trx;
	});
}

void hyphadao::eraseprop (const uint64_t& proposal_id) {
	proposal_table props(get_self(), get_self().value);
	auto i_iter = props.find(proposal_id);
	check(i_iter != props.end(), "prop not found");
	props.erase (i_iter);
}

void hyphadao::closeprop(const name& holder, const uint64_t& proposal_id) {
//    require_auth(holder);
// 	check(is_hvoice_holder(holder) || is_steward_holder(holder), "caller must be a HVOICE or STEWARD holder");

	proposal_table props(get_self(), get_self().value);
	auto i_iter = props.find(proposal_id);
	check(i_iter != props.end(), "prop not found");
	auto prop = *i_iter;

// 	ballots_table ballots("eosio.trail"_n, "eosio.trail"_n.value);
// 	auto ballot = ballots.get(prop.ballot_id, "ballot does not exist");

// 	proposals_table proposals("eosio.trail"_n, "eosio.trail"_n.value);
// 	auto prop = proposals.get(ballot.reference_id, "proposal not found");

// 	registries_table registries("eosio.trail"_n, "eosio.trail"_n.value);
// 	auto registry = registries.get(prop.no_count.symbol.code().raw(), "registry not found, this shouldn't happen");
// 	uint32_t total_voters = registry.total_voters;
	
// 	uint32_t unique_voters = prop.unique_voters;
     
// 	uint32_t quorum_threshold = total_voters / (_config.board_quorum_divisor - 1);
// 	common::prop_STATE state = common::FAIL;
// 	if(unique_voters > quorum_threshold)
// 		state = common::COUNT;
	
// 	if(state == common::COUNT) {
// 		if(prop.yes_count > prop.no_count)
// 			state = common::PASS;
// 		else if(prop.yes_count == prop.no_count)
// 			state = common::TIE;
// 		else
// 			state = common::FAIL;
// 	}

// 	if(state == common::PASS) {
	print (" Sending Transaction: ");
	prop.transaction.send(current_block_time().to_time_point().sec_since_epoch(), get_self());
	props.erase (i_iter);
// 	} 

// 	if(state == common::FAIL || state == common::PASS) {
// 		props.erase(i_iter);
// 	}

// 	if(state == common::TIE) {
// 		uint64_t next_ballot_id = ballots.available_primary_key();
// 		uint32_t begin_time = current_block_time().to_time_point().sec_since_epoch() + _config.start_delay;
// 		uint32_t end_time = begin_time + _config.prop_duration;
// 		action(permission_level{get_self(), "active"_n}, "eosio.trail"_n, "regballot"_n, make_tuple(
// 			get_self(),		      //proposer name
// 			uint8_t(0), 			//ballot_type uint8_t
// 			common::S_HVOICE,	   //voting_symbol symbol
// 			begin_time,				//begin_time uint32_t
// 			end_time,				//end_time uint32_t
// 			prop.info_url			//info_url string
// 		)).send();

// 		props.modify(prop, same_payer, [&](auto& i) {
// 			i.ballot_id = next_ballot_id;
// 		});
// 	}

//    action(permission_level{get_self(), "active"_n}, "eosio.trail"_n, "closeballot"_n, make_tuple(
// 		get_self(),
// 		prop.ballot_id,
// 		uint8_t(state)
// 	)).send();
}

void hyphadao::inithvoice(string initial_info_link) {
    // require_auth(get_self());
    
    // action(permission_level{get_self(), name("active")}, name("eosio.trail"), name("regtoken"), make_tuple(
	// 	common::INITIAL_HVOICE_MAX_SUPPLY, //max_supply
	// 	get_self(), //publisher
	// 	initial_info_link //info_url
	// )).send();

    // action(permission_level{get_self(), name("active")}, name("eosio.trail"), name("initsettings"), make_tuple(
	// 	get_self(), //publisher
	// 	common::INITIAL_HVOICE_MAX_SUPPLY.symbol, //token_symbol
	// 	common::INITIAL_HVOICE_SETTINGS //new_settings
	// )).send();

    // print("\nHVOICE Registration and Initialization Actions Sent...");
}

void hyphadao::initsteward(const string initial_info_link) {}

void hyphadao::qualify_proposer (const name& proposer) {
	return;
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