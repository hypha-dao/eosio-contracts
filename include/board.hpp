#ifndef BOARD_H
#define BOARD_H

#include <eosio/eosio.hpp>
#include <eosio/asset.hpp>
#include <eosio/singleton.hpp>
#include <eosio/multi_index.hpp>

#include "common.hpp"

using namespace eosio;
using std::string;
using std::vector;

class Board {

    public:

        #pragma region native

        enum class key_type : uint8_t {
            k1 = 0,
            r1 = 1,
        };

        struct public_key {
            key_type type{};
            std::array<uint8_t, 33> data{};
        };

        struct permission_level_weight {
            permission_level  permission;
            uint16_t          weight;
        };

        struct key_weight {
            public_key  key;
            uint16_t    weight;
        };

        struct wait_weight {
            uint32_t           wait_sec;
            uint16_t           weight;
        };

        struct authority {
            uint32_t                              threshold = 0;
            std::vector<key_weight>               keys;
            std::vector<permission_level_weight>  accounts;
            std::vector<wait_weight>              waits;
        };

        #pragma endregion native

        struct [[ eosio::table, eosio::contract("hyphadac") ]] BoardConfig
        {
            // TODO: should this be a time type instead of uint32_t
            //name        publisher                    = contract;
            uint8_t     max_board_seats               = 9; // NOTE: adjustable by board members
            uint8_t     open_seats                    = 9;
            uint64_t    open_election_id              = 0;
            uint32_t    holder_quorum_divisor         = 5;
            uint32_t    board_quorum_divisor          = 2;
            uint32_t    issue_duration                = 360;
            uint32_t    start_delay                   = 30;
            uint32_t    leaderboard_duration          = 360;
            uint32_t    election_frequency            = 360*3;
            uint32_t    last_board_election_time      ;
            bool        is_active_election            = false;

            // uint64_t primary_key() const { return publisher.value; }
        };

        struct [[eosio::table, eosio::contract("hyphadao") ]] Nominee {
            name      nominee;
            uint64_t  primary_key() const { return nominee.value; }
        };

        struct [[eosio::table, eosio::contract("hyphadao") ]] Steward {
            name     member;
            uint64_t primary_key() const { return member.value; }
        };

        typedef multi_index<"nominees"_n, Nominee> nominee_table;
        typedef multi_index<"stewards"_n, Steward> steward_table;

        typedef singleton<"bconfig"_n, BoardConfig> boardconfig_table;
        typedef multi_index<"bconfig"_n, BoardConfig> boardconfig_table_placeholder;

        Board (const name& contract) :
            contract (contract),
            nominee_t (contract, contract.value), 
            steward_t (contract, contract.value),
            boardconfig_s (contract, contract.value) {}

        nominee_table       nominee_t;
        steward_table       steward_t;
        boardconfig_table   boardconfig_s;
        name                contract;

    //     void init ();
    //     void setconfig (const name& member, BoardConfig new_config);
    //     void init_token ();
    //     void reset();
    //     void nominate (const name& nominee, const name& nominator);
    //     void makeelection(const name& holder, const string& info_url);
    //     void addcand(name nominee, string info_link);
    //     void removecand(name candidate);
    //     void endelection(name holder);
    //     void removemember(name member_to_remove);

    // private:

    //     name        contract;

    //     #pragma region Helper_Functions
    //     // BoardConfig get_default_config();
    //     void        add_steward             (name nominee);
    //     void        rmv_steward             (name member);
    //     void        addseats                (name member, uint8_t num_seats);
    //     bool        is_steward              (name user);
    //     bool        is_nominee              (name user);
    //     bool        is_hvoice_holder        (name user);
    //     bool        is_steward_holder       (name user);
    //     bool        is_term_expired         ();
    //     void        remove_and_seize_all    ();
    //     void        remove_and_seize        (name member);
    //     void        set_permissions         (vector<permission_level_weight> perms);
    //     uint8_t     get_occupied_seats      ();
    //     vector<permission_level_weight> perms_from_members();
    //     #pragma endregion Helper_Functions

BoardConfig get_config() {
    return boardconfig_s.get_or_create (contract, BoardConfig());
}

void reset() {
    boardconfig_s.remove();
}

void init_token () {
    require_auth(contract);
      
    action(permission_level{contract, name("active")}, name("eosio.trail"), name("regtoken"), make_tuple(
        common::INITIAL_STEWARD_MAX_SUPPLY,   //max_supply
        contract, 						//publisher
        "https://joinseeds.com" 			   //info_url
    )).send();

    action(permission_level{contract, name("active")}, name("eosio.trail"), name("initsettings"), make_tuple(
        contract,                                  //publisher
        common::INITIAL_STEWARD_MAX_SUPPLY.symbol,  //token_symbol
        common::INITIAL_STEWARD_SETTINGS            //new_settings
    )).send();

    asset board_token = asset { 1, common::S_STEWARD };
    action(permission_level{contract, "active"_n }, "eosio.trail"_n, "issuetoken"_n,
        std::make_tuple(
            contract,		//account to update
            contract,
            board_token,
            false
    )).send();

    print("\nSTEWARD Registration and Initialization Actions Sent...");
}

void setconfig(const name& member, BoardConfig new_config) { 

    require_auth(contract);

    check(new_config.max_board_seats >= new_config.open_seats, "can't have more open seats than max seats");
	check(new_config.holder_quorum_divisor > 0, "holder_quorum_divisor must be a non-zero number");
	check(new_config.board_quorum_divisor > 0, "board_quorum_divisor must be a non-zero number");
	check(new_config.issue_duration > 0, "issue_duration must be a non-zero number");
	check(new_config.start_delay > 0, "start_delay must be a non-zero number");
	check(new_config.leaderboard_duration > 0, "leaderboard_duration must be a non-zero number");
	check(new_config.election_frequency > 0, "election_frequency must be a non-zero number");

    BoardConfig existing_config;
    if (boardconfig_s.exists()) {
        existing_config = boardconfig_s.get();
        if(new_config.max_board_seats >= existing_config.max_board_seats){
            new_config.open_seats = new_config.max_board_seats - existing_config.max_board_seats + existing_config.open_seats;

            auto extra_seats = new_config.max_board_seats - existing_config.max_board_seats;
            if(extra_seats > 0){
                asset board_token = asset(extra_seats, common::S_STEWARD);
                action(permission_level{"tf"_n, "active"_n }, "eosio.trail"_n, "raisemax"_n,
                    std::make_tuple(
                        contract,
                        board_token
                    )
                ).send();
            }
        }else if(new_config.max_board_seats > existing_config.max_board_seats - existing_config.open_seats){
            new_config.open_seats = new_config.max_board_seats - (existing_config.max_board_seats - existing_config.open_seats);
        }else{
            new_config.open_seats = 0;
        }

        //  new_config.publisher = existing_config.publisher;
        new_config.open_election_id = existing_config.open_election_id;
        new_config.last_board_election_time = existing_config.last_board_election_time;
        new_config.is_active_election = existing_config.is_active_election;
    }
    boardconfig_s.set (new_config, contract);
}

void init () {

	boardconfig_s.set(BoardConfig(), contract);
    init_token ();
}


// BoardConfig get_default_config() {
// 	auto c = BoardConfig {
// 		contract,			// publisher
// 		uint8_t(9),		    // max seats
// 		uint8_t(9),         // open seats
// 		uint64_t(0),		// open_election_id
// 		uint32_t(5), 		// holder_quorum_divisor
// 		uint32_t(2), 		// board_quorum_divisor
// 		uint32_t(360),	    // issue_duration
// 		uint32_t(60),  	    // start_delay
// 		uint32_t(360),      // leaderboard_duration
// 		uint32_t(360),	    // election_frequency
// 		uint32_t(0),		// last_board_election_time
// 		false	            // is_active_election
// 	};
// 	boardconfig_s.set(c, contract);
// 	return c;
// }

void nominate(const name& nominee, const name& nominator) {
   require_auth(nominator);
   check(is_account(nominee), "nominee account must exist");
   check(is_hvoice_holder(nominator), "caller must be a HVOICE holder");
   check(!is_steward(nominee) || is_term_expired(), "nominee is a steward, nominee's term must be expired");

   nominee_table noms(contract, contract.value);
   auto n = noms.find(nominee.value);
   check(n == noms.end(), "nominee has already been nominated");

   noms.emplace(contract, [&](auto& m) {
      m.nominee = nominee;
   });
}

void makeelection(const name& holder, const string& info_url) {
   require_auth(holder);

    BoardConfig board_config = boardconfig_s.get_or_create (contract, BoardConfig());
	check(!board_config.is_active_election, "there is already an election is progress");
    check(is_hvoice_holder(holder) || is_steward(holder), "caller must be a HVOICE or STEWARD holder");
	check(board_config.open_seats > 0 || is_term_expired(), "it isn't time for the next election");

	ballots_table ballots("eosio.trail"_n, "eosio.trail"_n.value);
	board_config.open_election_id = ballots.available_primary_key(); 

	uint32_t begin_time = current_block_time().to_time_point().sec_since_epoch() + board_config.start_delay;
	uint32_t end_time = begin_time + board_config.leaderboard_duration;

    action(permission_level{contract, name("active")}, name("eosio.trail"), name("regballot"), make_tuple(
		contract,
		uint8_t(2), 			//NOTE: makes a leaderboard on Trail
		common::S_HVOICE,
		begin_time,
      end_time,
      info_url
	)).send();

	uint8_t available_seats = board_config.open_seats;
	if(is_term_expired()){
		available_seats = board_config.max_board_seats;
	}

   action(permission_level{contract, name("active")}, name("eosio.trail"), name("setseats"), make_tuple(
		contract,
		board_config.open_election_id, 			//NOTE: adds available seats to a leaderboard on Trail
		available_seats
	)).send();

	//NOTE: this prevents makeelection from being called multiple times.
	//NOTE2 : this gets overwritten by setconfig
	board_config.open_seats = 0;
	board_config.is_active_election = true;
    boardconfig_s.set(board_config, contract);
}

void addcand(name nominee, string info_link) {

	require_auth(nominee);
    BoardConfig board_config = boardconfig_s.get_or_create (contract, BoardConfig());
	check(is_nominee(nominee), "only nominees can be added to the election");
	check(board_config.is_active_election, "no active election for board members at this time");
	check(!is_steward(nominee) || is_term_expired(), "nominee can't already be a board member, or their term must be expired.");

    action(permission_level{contract, name("active")}, name("eosio.trail"), name("addcandidate"), make_tuple(
		contract, 				    // publisher
		board_config.open_election_id, 	// ballot_id
		nominee, 					    // new_candidate
		info_link					    // info_link
	)).send();
}

void removecand(name candidate) {
	require_auth(candidate);
	check(is_nominee(candidate), "candidate is not a nominee");

    BoardConfig board_config = boardconfig_s.get_or_create (contract, BoardConfig());

    action(permission_level{contract, name("active")}, name("eosio.trail"), name("rmvcandidate"), make_tuple(
            contract, 				   //publisher
            board_config.open_election_id, 	//ballot_id
            candidate 					   //new_candidate
    )).send();
}

void endelection(name holder) {
    require_auth(holder);
    BoardConfig board_config = boardconfig_s.get_or_create (contract, BoardConfig());

    check(is_hvoice_holder(holder) || is_steward(holder), "caller must be a HVOICE or STEWARD holder");
	check(board_config.is_active_election, "there is no active election to end");
	uint8_t status = 1;

    ballots_table ballots(name("eosio.trail"), name("eosio.trail").value);
    auto bal = ballots.get(board_config.open_election_id);
    
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
		board_config.last_board_election_time = current_block_time().to_time_point().sec_since_epoch();
	}

    for (int n = 0; n < board_candidates.size(); n++) {
		if(board_candidates[n].votes > asset(0, board_candidates[n].votes.symbol))
        	add_steward(board_candidates[n].member);
    }
    
	vector<permission_level_weight> currently_elected = perms_from_members(); //NOTE: needs testing

	if(currently_elected.size() > 0)
		set_permissions(currently_elected);
	
	steward_table members(contract, contract.value);

	board_config.open_seats = board_config.max_board_seats - uint8_t(std::distance(members.begin(), members.end()));

	action(permission_level{contract, name("active")}, name("eosio.trail"), name("closeballot"), make_tuple(
		contract,
		board_config.open_election_id,
		status
	)).send();
	board_config.is_active_election = false;
    boardconfig_s.set(board_config, contract);

}

void removemember(name member_to_remove) {
	require_auth(permission_level{contract, "major"_n});
	remove_and_seize(member_to_remove);
	
	auto perms = perms_from_members();
	set_permissions(perms);
}

#pragma region Helper_Functions

void add_steward(name nominee) {
   nominee_table noms(contract, contract.value);
   auto n = noms.find(nominee.value);
   check(n != noms.end(), "nominee doesn't exist in table");

   steward_table mems(contract, contract.value);
   auto m = mems.find(nominee.value);
   check(m == mems.end(), "nominee is already a board member"); //NOTE: change if error occurs in live environment

   noms.erase(n); //NOTE remove from nominee table

   mems.emplace(contract, [&](auto& m) { //NOTE: emplace in boardmembers table
      m.member = nominee;
   });

	asset board_token = asset(1, common::S_STEWARD);
	action(permission_level{contract, "active"_n }, "eosio.trail"_n, "issuetoken"_n,
		std::make_tuple(
			contract,		//account to update
			nominee,
			board_token,
			false
		)
	).send();
}

void rmv_steward(name member) {
    steward_table mems(contract, contract.value);
    auto m = mems.find(member.value);
    check(m != mems.end(), "member is not on the board");

    mems.erase(m);
}

void addseats(name member, uint8_t num_seats) {
   require_auth(contract);

    BoardConfig board_config = boardconfig_s.get_or_create (contract, BoardConfig());
	board_config.max_board_seats += num_seats;
	board_config.open_seats += num_seats;
    boardconfig_s.set(board_config, contract);
}

bool is_steward(name user) {
   steward_table mems(contract, contract.value);
   auto m = mems.find(user.value);
   if (m != mems.end()) {
      return true;
   }
   return false;
}

bool is_nominee(name user) {
   nominee_table noms(contract, contract.value);
   auto n = noms.find(user.value);
   if (n != noms.end()) {
      return true;
   }
   return false;
}

bool is_hvoice_holder(name user) {
   balances_table balances(name("eosio.trail"), common::S_HVOICE.code().raw());
   auto b = balances.find(user.value);
   if (b != balances.end()) {
      return true;
   }
   return false;
}

bool  is_steward_holder(name user) {
    balances_table balances(name("eosio.trail"), common::S_STEWARD.code().raw());
    auto b = balances.find(user.value);
    if (b != balances.end()) {
        return true;
    }
    return false;
}

bool is_term_expired() {
    BoardConfig board_config = boardconfig_s.get_or_create (contract, BoardConfig());

	return current_block_time().to_time_point().sec_since_epoch() - 
        board_config.last_board_election_time > board_config.election_frequency;
}

void remove_and_seize_all() {
	steward_table stewards (contract, contract.value);
	asset amount_to_seize = asset(1, common::S_STEWARD);

	auto itr = stewards.begin();
	vector<name> to_seize;
	while(itr != stewards.end()) {
		to_seize.emplace_back(itr->member);
		itr = stewards.erase(itr);
	}
	
	if(to_seize.size() > 0){
		action(permission_level{contract, "active"_n }, "eosio.trail"_n, "seizebygroup"_n,
			std::make_tuple(
				contract,		// account to update
				to_seize,
				amount_to_seize
			)
		).send();

		asset amount_to_burn = asset(to_seize.size(), common::S_STEWARD);
		action(permission_level{contract, "active"_n }, "eosio.trail"_n, "burntoken"_n,
			std::make_tuple(
				contract,
				amount_to_burn
			)
		).send();
	}
}

void remove_and_seize(name member) {
	steward_table stewards (contract, contract.value);
	asset amount_to_seize = asset(1, common::S_STEWARD);
	auto m = stewards.get(member.value, "board member not found");

	stewards.erase(m);

	action(permission_level{contract, "active"_n }, "eosio.trail"_n, "seizetoken"_n,
		std::make_tuple(
			contract,		//account to update
			member,
			amount_to_seize
		)
	).send();
}

void set_permissions(vector<permission_level_weight> perms) {
	auto self = contract;
	uint16_t active_weight = perms.size() < 3 ? 1 : ((perms.size() / 3) * 2);

	perms.emplace_back(
		permission_level_weight{ permission_level{
				self,
				"eosio.code"_n
			}, active_weight}
	);
	sort(perms.begin(), perms.end(), [](const auto &first, const auto &second) { return first.permission.actor.value < second.permission.actor.value; });
	
	action(permission_level{contract, "owner"_n }, "eosio"_n, "updateauth"_n,
		std::make_tuple(
			contract, 
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
	action(permission_level{contract, "owner"_n }, "eosio"_n, "updateauth"_n,
		std::make_tuple(
			contract, 
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

vector<permission_level_weight> perms_from_members() {
	steward_table members(contract, contract.value);
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

};

#endif