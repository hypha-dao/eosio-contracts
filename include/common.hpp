#pragma once

#include <eosio/symbol.hpp>
#include <trail.voting.hpp>
#include <trail.tokens.hpp>

using namespace eosio;
using std::string;

namespace common {

    static const symbol         S_HYPHA                         ("HYPHA", 0);
    static const symbol         S_HVOICE                        ("HVOIC", 0);
    static const symbol         S_PRESEEDS                      ("PRESEED", 8);
    static const symbol         S_STEWARD                       ("STEWARD", 0);
    
    static const uint64_t       SCALER                          = 100000000;

    static const uint8_t        PAUSED                          = 1;
    static const uint8_t        UNPAUSED                        = 0;

    static const uint8_t         OPEN                            = 1;
    static const uint8_t         EXECUTED                        = 2;
    static const uint8_t         REJECTED                        = 3;

    static const uint8_t        ROLE_PROPOSAL                   = 1;
    static const uint8_t        ASS_PROPOSAL                    = 2;
    static const uint8_t        PAYOUT_PROPOSAL                 = 3;
    static const uint8_t        OTHER_PROPOSAL                  = 4;

    static const uint8_t        NO_ASSIGNMENT                   = -1;

    static const string         SEEDS_URL                       = "https://joinseeds.com";
    
    enum PROP_STATE {
		FAIL = 2,
		COUNT = 0,
		TIE = 3,
		PASS = 1
	};

    asset const INITIAL_HVOICE_MAX_SUPPLY = asset(500, common::S_HVOICE); 
    
    asset const INITIAL_STEWARD_MAX_SUPPLY = asset(9, common::S_STEWARD); 
    
    token_settings const INITIAL_HVOICE_SETTINGS = token_settings { 
        false, //is_destructible
        true, //is_proxyable
        true, //is_burnable
        true, //is_seizable
        false, //is_max_mutable
        false, //is_transferable
        true, //is_recastable
        false, //is_initialized
        uint32_t(500), //counterbal_decay_rate (not applicable since non-transferable)
        true, //lock_after_initialize
    };

    token_settings const INITIAL_STEWARD_SETTINGS = token_settings { 
        false, //is_destructible
        false, //is_proxyable
        true, //is_burnable
        true, //is_seizable
        true, //is_max_mutable
        false, //is_transferable
        false, //is_recastable
        false, //is_initialized
        uint32_t(500), //counterbal_decay_rate (not applicable since non-transferable)
        true, //lock_after_initialize
    };

};