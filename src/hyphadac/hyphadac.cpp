#include <hyphadac.hpp>

hyphadac::hyphadac(name self, name code, datastream<const char*> ds)
: contract(self, code, ds), voting_configs(get_self(), get_self().value) {
	print("\n exists?: ", voting_configs.exists());
	_config = voting_configs.exists() ? voting_configs.get() : get_default_config();
}

hyphadac::~hyphadac() {
	if(voting_configs.exists()) voting_configs.set(_config, get_self());
}

ACTION hyphadac::reset () {
   voting_configs.remove();
}

hyphadac::VotingConfig hyphadac::get_default_config() {
	auto c = VotingConfig {
		get_self(),			//publisher
		uint8_t(9),		   //max seats
		uint8_t(9),       //open seats
		uint64_t(0),		//open_election_id
		uint32_t(5), 		//holder_quorum_divisor
		uint32_t(2), 		//board_quorum_divisor
		uint32_t(360),	   //issue_duration
		uint32_t(60),  	//start_delay
		uint32_t(360),    //leaderboard_duration
		uint32_t(360),	   //election_frequency
		uint32_t(0),		//last_board_election_time
		false				   //is_active_election
	};
	voting_configs.set(c, get_self());
	return c;
}

void hyphadac::setconfig (const uint32_t period_length, 
                           const name     hypha_token_contract) {

   require_auth (get_self());
   check (is_account(hypha_token_contract), "HYPHA token contract is not an account: " + hypha_token_contract.to_string());

   config_table config_s (get_self(), get_self().value);
   Config c = config_s.get_or_create (get_self(), Config());
   c.period_length            = period_length;
   c.hypha_token_contract     = hypha_token_contract;
   c.preseeds_token_contract  = hypha_token_contract;
   config_s.set(c, get_self());

   voting_configs.remove();
	VotingConfig new_config = get_default_config();
   check(new_config.max_board_seats >= new_config.open_seats, "can't have more open seats than max seats");
	check(new_config.holder_quorum_divisor > 0, "holder_quorum_divisor must be a non-zero number");
	check(new_config.board_quorum_divisor > 0, "board_quorum_divisor must be a non-zero number");
	check(new_config.issue_duration > 0, "issue_duration must be a non-zero number");
	check(new_config.start_delay > 0, "start_delay must be a non-zero number");
	check(new_config.leaderboard_duration > 0, "leaderboard_duration must be a non-zero number");
	check(new_config.election_frequency > 0, "election_frequency must be a non-zero number");

	// NOTE : this will break an ongoing election check for makeelection 
	if(new_config.max_board_seats >= _config.max_board_seats){
		new_config.open_seats = new_config.max_board_seats - _config.max_board_seats + _config.open_seats;

		auto extra_seats = new_config.max_board_seats - _config.max_board_seats;
		if(extra_seats > 0){
			asset board_token = asset(extra_seats, common::S_STEWARD);
			action(permission_level{get_self(), "active"_n }, "eosio.trail"_n, "raisemax"_n,
				std::make_tuple(
					get_self(),
					board_token
				)
			).send();
		}
	} else if(new_config.max_board_seats > _config.max_board_seats - _config.open_seats){
		new_config.open_seats = new_config.max_board_seats - (_config.max_board_seats - _config.open_seats);
	} else {
		new_config.open_seats = 0;
	}

	new_config.publisher = _config.publisher;
	new_config.open_election_id = _config.open_election_id;
	new_config.last_board_election_time = _config.last_board_election_time;
	new_config.is_active_election = _config.is_active_election;

	_config = new_config;
	voting_configs.set(_config, get_self());
}

void hyphadac::inithvoice(string initial_info_link) {
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

void hyphadac::initsteward(string initial_info_link) {
   require_auth(get_self());
      
   action(permission_level{get_self(), name("active")}, name("eosio.trail"), name("regtoken"), make_tuple(
      common::INITIAL_STEWARD_MAX_SUPPLY,   //max_supply
      get_self(), 						//publisher
      initial_info_link 			   //info_url
   )).send();

   action(permission_level{get_self(), name("active")}, name("eosio.trail"), name("initsettings"), make_tuple(
      get_self(),                                  //publisher
      common::INITIAL_STEWARD_MAX_SUPPLY.symbol,  //token_symbol
      common::INITIAL_STEWARD_SETTINGS            //new_settings
   )).send();

	asset board_token = asset { 1, common::S_STEWARD };
   action(permission_level{get_self(), "active"_n }, "eosio.trail"_n, "issuetoken"_n,
      std::make_tuple(
         get_self(),		//account to update
         get_self(),
         board_token,
         false
   )).send();

   print("\nSTEWARD Registration and Initialization Actions Sent...");
}

void hyphadac::proposerole (const name proposer,
                           const name role_name,
                           const string info_url,
                           const string description,
                           const asset hypha_salary,
                           const asset preseeds_salary,
                           const asset voice_salary) {

 	transaction trx (time_point_sec(current_time_point())+ (60 * 60 * 24 * 35));
	trx.actions.emplace_back(permission_level{get_self(), "owner"_n}, 
		get_self(), "newrole"_n, 
		std::make_tuple(role_name, description, hypha_salary, preseeds_salary, voice_salary));
	trx.delay_sec = 0;
	
   makeissue (proposer, info_url, role_name, description, trx);
}

void hyphadac::newrole (const name role_name, 
                        const string description,
                        const asset hypha_salary,
                        const asset preseeds_salary,
                        const asset voice_salary) {

   require_auth (get_self());
   role_table role_t (get_self(), get_self().value);
   check (role_t.find (role_name.value) == role_t.end(), "Role already exists: " + role_name.to_string());

   role_t.emplace (get_self(), [&](auto &r) {
      r.role_name    = role_name;
      r.description  = description;
      r.hypha_salary = hypha_salary;
      r.preseeds_salary = preseeds_salary;
      r.voice_salary    = voice_salary;

      // TODO: is current_block_time in milliseconds or seconds, and since when
      r.created_date    = current_block_time().to_time_point();
      r.updated_date    = current_block_time().to_time_point();
   });
}

void hyphadac::updaterole (const name role_name, 
                           const string description,
                           const asset hypha_salary,
                           const asset preseeds_salary,
                           const asset voice_salary) {
   require_auth (get_self());
   role_table role_t (get_self(), get_self().value);
   
   auto r_itr = role_t.find (role_name.value);
   check (r_itr != role_t.end(), "Role does not exist: " + role_name.to_string());

   role_t.modify (r_itr, get_self(), [&](auto &r) {
      r.description  = description;
      r.hypha_salary = hypha_salary;
      r.preseeds_salary = preseeds_salary;
      r.voice_salary    = voice_salary;

      // TODO: is current_block_time in milliseconds or seconds, and since when
      r.updated_date    = current_block_time().to_time_point();
   });
}

void hyphadac::propassign (const name        proposer,
                           const name        assigned_account,
                           const name        role_name,
                           const string      info_url,
                           const string      notes,
                           const time_point  start_date,
                           const float       time_share) {
   transaction trx (time_point_sec(current_time_point())+ (60 * 60 * 24 * 35));
	trx.actions.emplace_back(permission_level{get_self(), "owner"_n}, 
		get_self(), "assign"_n, 
		std::make_tuple(assigned_account, role_name, info_url, notes, start_date, time_share));
	trx.delay_sec = 0;
	
   makeissue (proposer, info_url, role_name, notes, trx);
}

void hyphadac::assign (const name         assigned_account,
                        const name        role_name,
                        const string      info_url,
                        const string      notes,
                        const time_point  start_date,
                        const float       time_share) {

   require_auth (get_self());
   assignment_table assignment_t (get_self(), get_self().value);
   auto sorted_by_assigned = assignment_t.get_index<"byassigned"_n>();

   auto a_itr = sorted_by_assigned.begin();
   while (a_itr != sorted_by_assigned.end()) {
      check (a_itr->role_name != role_name, "Assigned account already has this role. Assigned account: " 
         + assigned_account.to_string() + "; Role name: " + role_name.to_string());    
      a_itr++;
   }
   
   assignment_t.emplace (get_self(), [&](auto &a) {
      a.assignment_id      = assignment_t.available_primary_key();
      a.assigned_account   = assigned_account;
      a.role_name          = role_name;
      a.notes              = notes;
      a.start_date         = start_date;
      a.time_share         = time_share;
   });
}

void hyphadac::propcontrib (const name       proposer,
                           const name        contributor,
                           const string      notes,
                           const asset       hypha_value,
                           const asset       preseeds_value, 
                           const time_point  contribution_date) {

   	transaction trx (time_point_sec(current_time_point())+ (60 * 60 * 24 * 35));
	trx.actions.emplace_back(permission_level{get_self(), "owner"_n}, 
		get_self(), "contribute"_n, 
		std::make_tuple(contributor, notes, hypha_value, preseeds_value, contribution_date));
	trx.delay_sec = 0;
	
   	makeissue (proposer, info_url, contributor, notes, trx);
}

void hyphadac::paycontrib (const name        contributor,
                           const string      notes,
                           const asset       hypha_value,
                           const asset       preseeds_value, 
                           const time_point  contribution_date) {

   require_auth (get_self());
   contribution_table contribution_t (get_self(), get_self().value);

   contribution_t.emplace (get_self(), [&](auto &c) {
      c.contribution_id       = contribution_t.available_primary_key();
      c.contributor           = contributor;
      c.description           = description;
      c.hypha_value           = hypha_value;
      c.preseeds_value        = preseeds_value;
      c.contribution_date     = contribution_date;
      c.status                = common::OPEN;
      c.created_date          = current_block_time().to_time_point();
   });
}

void hyphadac::makeissue(const name proposer, 
                        const string info_url,
                        const name proposal_name,
                        const string notes,
                        const transaction trx) {

	// name 	_holder;
	// string _info_url;
	// name	_issue_name;
	// eosio::transaction _trx;

	// _ds >> _holder >> _info_url >> _issue_name >> _trx;

   	require_auth(proposer);

	check(is_hvoice_holder(proposer) || is_steward_holder(proposer), "caller must be a STEWARD or HVOICE holder");

	ballots_table ballots("eosio.trail"_n, "eosio.trail"_n.value);
	uint64_t next_ballot_id = ballots.available_primary_key();
	uint32_t begin_time = current_block_time().to_time_point().sec_since_epoch() + _config.start_delay;
	uint32_t end_time = begin_time + _config.issue_duration;

   	action(permission_level{get_self(), "active"_n}, "eosio.trail"_n, "regballot"_n, make_tuple(
		get_self(),
		uint8_t(0), 			// NOTE: makes a proposal on Trail
		common::S_HVOICE,
		begin_time,
      end_time,
      info_url
	)).send();
	
	proposal_table proposals(get_self(), get_self().value);
	proposals.emplace(get_self(), [&](auto& p) {
		p.proposer        = proposer;
		p.proposal_name   = proposal_name;
      p.notes           = notes;
		p.ballot_id       = next_ballot_id;
		p.transaction     = trx;
	});
}

void hyphadac::closeissue(name holder, name proposer) {
   require_auth(holder);
	check(is_hvoice_holder(holder) || is_steward_holder(holder), "caller must be a HVOICE or STEWARD holder");

	proposal_table issues(get_self(), get_self().value);
	auto i_iter = issues.find(proposer.value);
	check(i_iter != issues.end(), "issue not found");
	auto issue = *i_iter;

	ballots_table ballots("eosio.trail"_n, "eosio.trail"_n.value);
	auto ballot = ballots.get(issue.ballot_id, "ballot does not exist");

	proposals_table proposals("eosio.trail"_n, "eosio.trail"_n.value);
	auto prop = proposals.get(ballot.reference_id, "proposal not found");

	registries_table registries("eosio.trail"_n, "eosio.trail"_n.value);
	auto registry = registries.get(prop.no_count.symbol.code().raw(), "registry not found, this shouldn't happen");
	uint32_t total_voters = registry.total_voters;
	
	uint32_t unique_voters = prop.unique_voters;
     
	uint32_t quorum_threshold = total_voters / (_config.board_quorum_divisor - 1);
	common::ISSUE_STATE state = common::FAIL;
	if(unique_voters > quorum_threshold)
		state = common::COUNT;
	
	if(state == common::COUNT) {
		if(prop.yes_count > prop.no_count)
			state = common::PASS;
		else if(prop.yes_count == prop.no_count)
			state = common::TIE;
		else
			state = common::FAIL;
	}

	if(state == common::PASS) {

		issue.transaction.send(current_block_time().to_time_point().sec_since_epoch(), get_self());
		// steward_table members(get_self(), get_self().value);
		// std::vector<permission_level> requested;
		// auto itr = members.begin();

		// while (itr != members.end()) {
		// 	// print("adding permission_level: ", name{itr->member});
		// 	requested.emplace_back(permission_level(itr->member, "active"_n));
		// 	itr++;
		// }	
		
		// action(permission_level{get_self(), name("active")}, name("eosio.msig"), name("propose"), make_tuple(
		// 	get_self(),
		// 	issue.issue_name,
		// 	requested,
		// 	issue.transaction
		// )).send();
	} 

	if(state == common::FAIL || state == common::PASS) {
		issues.erase(i_iter);
	}

	if(state == common::TIE) {
		uint64_t next_ballot_id = ballots.available_primary_key();
		uint32_t begin_time = current_block_time().to_time_point().sec_since_epoch() + _config.start_delay;
		uint32_t end_time = begin_time + _config.issue_duration;
		action(permission_level{get_self(), "active"_n}, "eosio.trail"_n, "regballot"_n, make_tuple(
			get_self(),		      //proposer name
			uint8_t(0), 			//ballot_type uint8_t
			common::S_HVOICE,	   //voting_symbol symbol
			begin_time,				//begin_time uint32_t
			end_time,				//end_time uint32_t
			prop.info_url			//info_url string
		)).send();

		issues.modify(issue, same_payer, [&](auto& i) {
			i.ballot_id = next_ballot_id;
		});
	}

   action(permission_level{get_self(), "active"_n}, "eosio.trail"_n, "closeballot"_n, make_tuple(
		get_self(),
		issue.ballot_id,
		uint8_t(state)
	)).send();
}

void hyphadac::nominate(name nominee, name nominator) {
   require_auth(nominator);
   check(is_account(nominee), "nominee account must exist");
   check(is_hvoice_holder(nominator), "caller must be a HVOICE holder");
   check(!is_steward(nominee) || is_term_expired(), "nominee is a steward, nominee's term must be expired");

   nominee_table noms(get_self(), get_self().value);
   auto n = noms.find(nominee.value);
   check(n == noms.end(), "nominee has already been nominated");

   noms.emplace(get_self(), [&](auto& m) {
      m.nominee = nominee;
   });
}

void hyphadac::makeelection(name holder, string info_url) {
   require_auth(holder);
	check(!_config.is_active_election, "there is already an election is progress");
   check(is_hvoice_holder(holder) || is_steward(holder), "caller must be a HVOICE or STEWARD holder");
	check(_config.open_seats > 0 || is_term_expired(), "it isn't time for the next election");

	ballots_table ballots("eosio.trail"_n, "eosio.trail"_n.value);
	_config.open_election_id = ballots.available_primary_key(); 

	uint32_t begin_time = current_block_time().to_time_point().sec_since_epoch() + _config.start_delay;
	uint32_t end_time = begin_time + _config.leaderboard_duration;

    action(permission_level{get_self(), name("active")}, name("eosio.trail"), name("regballot"), make_tuple(
		get_self(),
		uint8_t(2), 			//NOTE: makes a leaderboard on Trail
		common::S_HVOICE,
		begin_time,
      end_time,
      info_url
	)).send();

	uint8_t available_seats = _config.open_seats;
	if(is_term_expired()){
		available_seats = _config.max_board_seats;
	}

   action(permission_level{get_self(), name("active")}, name("eosio.trail"), name("setseats"), make_tuple(
		get_self(),
		_config.open_election_id, 			//NOTE: adds available seats to a leaderboard on Trail
		available_seats
	)).send();

	//NOTE: this prevents makeelection from being called multiple times.
	//NOTE2 : this gets overwritten by setconfig
	_config.open_seats = 0;
	_config.is_active_election = true;
}

void hyphadac::addcand(name nominee, string info_link) {
	require_auth(nominee);
	check(is_nominee(nominee), "only nominees can be added to the election");
	check(_config.is_active_election, "no active election for board members at this time");
	check(!is_steward(nominee) || is_term_expired(), "nominee can't already be a board member, or their term must be expired.");

    action(permission_level{get_self(), name("active")}, name("eosio.trail"), name("addcandidate"), make_tuple(
		get_self(), 				   //publisher
		_config.open_election_id, 	//ballot_id
		nominee, 					   //new_candidate
		info_link					   //info_link
	)).send();
}

void hyphadac::removecand(name candidate) {
	require_auth(candidate);
	check(is_nominee(candidate), "candidate is not a nominee");

   action(permission_level{get_self(), name("active")}, name("eosio.trail"), name("rmvcandidate"), make_tuple(
		get_self(), 				   //publisher
		_config.open_election_id, 	//ballot_id
		candidate 					   //new_candidate
	)).send();
}

void hyphadac::endelection(name holder) {
   require_auth(holder);
   check(is_hvoice_holder(holder) || is_steward(holder), "caller must be a HVOICE or STEWARD holder");
	check(_config.is_active_election, "there is no active election to end");
	uint8_t status = 1;

    ballots_table ballots(name("eosio.trail"), name("eosio.trail").value);
    auto bal = ballots.get(_config.open_election_id);
    
    leaderboards_table leaderboards(name("eosio.trail"), name("eosio.trail").value);
    auto board = leaderboards.get(bal.reference_id);
    auto board_candidates = board.candidates;

    sort(board_candidates.begin(), board_candidates.end(), [](const auto &c1, const auto &c2) { return c1.votes > c2.votes; });
	
	if(board_candidates.size() > board.available_seats) {
		auto first_cand_out = board_candidates[board.available_seats];
		board_candidates.resize(board.available_seats);
		
		// count candidates that are tied with first_cand_out
		uint8_t tied_cands = 0;
		for(int i = board_candidates.size() - 1; i >= 0; i--) {
			if(board_candidates[i].votes == first_cand_out.votes) 
			tied_cands++;
		}

		// remove all tied candidates
		if(tied_cands > 0) board_candidates.resize(board_candidates.size() - tied_cands);
	}

	if(board_candidates.size() > 0 && is_term_expired()) {
		remove_and_seize_all();
		_config.last_board_election_time = current_block_time().to_time_point().sec_since_epoch();
	}

    for (int n = 0; n < board_candidates.size(); n++) {
		if(board_candidates[n].votes > asset(0, board_candidates[n].votes.symbol))
        	add_steward(board_candidates[n].member);
    }
    
	vector<permission_level_weight> currently_elected = perms_from_members(); //NOTE: needs testing

	if(currently_elected.size() > 0)
		set_permissions(currently_elected);
	
	steward_table members(_self, _self.value);

	_config.open_seats = _config.max_board_seats - uint8_t(std::distance(members.begin(), members.end()));

	action(permission_level{get_self(), name("active")}, name("eosio.trail"), name("closeballot"), make_tuple(
		get_self(),
		_config.open_election_id,
		status
	)).send();
	_config.is_active_election = false;
}

void hyphadac::removemember(name member_to_remove) {
	require_auth(permission_level{get_self(), "major"_n});
	remove_and_seize(member_to_remove);
	
	auto perms = perms_from_members();
	set_permissions(perms);
}


#pragma region Helper_Functions

void hyphadac::add_steward(name nominee) {
   nominee_table noms(get_self(), get_self().value);
   auto n = noms.find(nominee.value);
   check(n != noms.end(), "nominee doesn't exist in table");

   steward_table mems(get_self(), get_self().value);
   auto m = mems.find(nominee.value);
   check(m == mems.end(), "nominee is already a board member"); //NOTE: change if error occurs in live environment

   noms.erase(n); //NOTE remove from nominee table

   mems.emplace(get_self(), [&](auto& m) { //NOTE: emplace in boardmembers table
      m.member = nominee;
   });

	asset board_token = asset(1, common::S_STEWARD);
	action(permission_level{get_self(), "active"_n }, "eosio.trail"_n, "issuetoken"_n,
		std::make_tuple(
			get_self(),		//account to update
			nominee,
			board_token,
			false
		)
	).send();
}

void hyphadac::rmv_steward(name member) {
    steward_table mems(get_self(), get_self().value);
    auto m = mems.find(member.value);
    check(m != mems.end(), "member is not on the board");

    mems.erase(m);
}

void hyphadac::addseats(name member, uint8_t num_seats) {
   require_auth(get_self());

   voting_config_table configs(get_self(), get_self().value);
   auto c = voting_configs.get();

	c.max_board_seats += num_seats;
	c.open_seats += num_seats;

   configs.set(c, get_self());
}

bool hyphadac::is_steward(name user) {
   steward_table mems(get_self(), get_self().value);
   auto m = mems.find(user.value);
   
   if (m != mems.end()) {
      return true;
   }

   return false;
}

bool hyphadac::is_nominee(name user) {
   nominee_table noms(get_self(), get_self().value);
   auto n = noms.find(user.value);

   if (n != noms.end()) {
      return true;
   }

   return false;
}

bool hyphadac::is_hvoice_holder(name user) {
   balances_table balances(name("eosio.trail"), common::S_HVOICE.code().raw());
   auto b = balances.find(user.value);

   if (b != balances.end()) {
      return true;
   }

   return false;
}

bool hyphadac:: is_steward_holder(name user) {
    balances_table balances(name("eosio.trail"), common::S_STEWARD.code().raw());
    auto b = balances.find(user.value);

    if (b != balances.end()) {
        return true;
    }

    return false;
}

bool hyphadac::is_term_expired() {
	return current_block_time().to_time_point().sec_since_epoch() - _config.last_board_election_time > _config.election_frequency;
}

void hyphadac::remove_and_seize_all() {
	steward_table stewards (get_self(), get_self().value);
	asset amount_to_seize = asset(1, common::S_STEWARD);

	auto itr = stewards.begin();
	vector<name> to_seize;
	while(itr != stewards.end()) {
		to_seize.emplace_back(itr->member);
		itr = stewards.erase(itr);
	}
	
	if(to_seize.size() > 0){
		action(permission_level{get_self(), "active"_n }, "eosio.trail"_n, "seizebygroup"_n,
			std::make_tuple(
				get_self(),		// account to update
				to_seize,
				amount_to_seize
			)
		).send();

		asset amount_to_burn = asset(to_seize.size(), common::S_STEWARD);
		action(permission_level{get_self(), "active"_n }, "eosio.trail"_n, "burntoken"_n,
			std::make_tuple(
				get_self(),
				amount_to_burn
			)
		).send();
	}
}

void hyphadac::remove_and_seize(name member) {
	steward_table stewards (get_self(), get_self().value);
	asset amount_to_seize = asset(1, common::S_STEWARD);
	auto m = stewards.get(member.value, "board member not found");

	stewards.erase(m);

	action(permission_level{get_self(), "active"_n }, "eosio.trail"_n, "seizetoken"_n,
		std::make_tuple(
			get_self(),		//account to update
			member,
			amount_to_seize
		)
	).send();
}

void hyphadac::set_permissions(vector<permission_level_weight> perms) {
	auto self = get_self();
	uint16_t active_weight = perms.size() < 3 ? 1 : ((perms.size() / 3) * 2);

	perms.emplace_back(
		permission_level_weight{ permission_level{
				self,
				"eosio.code"_n
			}, active_weight}
	);
	sort(perms.begin(), perms.end(), [](const auto &first, const auto &second) { return first.permission.actor.value < second.permission.actor.value; });
	
	action(permission_level{get_self(), "owner"_n }, "eosio"_n, "updateauth"_n,
		std::make_tuple(
			get_self(), 
			name("active"), 
			name("owner"),
			authority {
				active_weight, 
				std::vector<key_weight>{},
				perms,
				std::vector<wait_weight>{}
			}
		)
	).send();

	auto tf_it = std::find_if(perms.begin(), perms.end(), [&self](const permission_level_weight &lvlw) {
        return lvlw.permission.actor == self; 
    });
	perms.erase(tf_it);
	uint16_t minor_weight = perms.size() < 4 ? 1 : (perms.size() / 4);
	action(permission_level{get_self(), "owner"_n }, "eosio"_n, "updateauth"_n,
		std::make_tuple(
			get_self(), 
			name("minor"), 
			name("owner"),
			authority {
				minor_weight, 
				std::vector<key_weight>{},
				perms,
				std::vector<wait_weight>{}
			}
		)
	).send();
}

vector<hyphadac::permission_level_weight> hyphadac::perms_from_members() {
	steward_table members(get_self(), get_self().value);
	auto itr = members.begin();
	
	vector<permission_level_weight> perms;
	while(itr != members.end()) {
			perms.emplace_back(permission_level_weight{ permission_level{
				itr->member,
				"active"_n
			}, 1});
		itr++;
	}

	return perms;
}

#pragma endregion Helper_Functions
