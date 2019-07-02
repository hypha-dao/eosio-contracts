#pragma once

#include <eosio/symbol.hpp>

using namespace eosio;
using std::string;

namespace common {

    static const symbol         S_HYPHA                         ("HYPHA", 0);
    static const symbol         S_VOICE                         ("HVOICE", 0);
    static const symbol         S_PRESEEDS                      ("PRESEED", 8);
    
    static const uint64_t       SCALER                          = 100000000;

    static const uint8_t        PAUSED                          = 1;
    static const uint8_t        UNPAUSED                        = 0;

    static const string         OPEN                            = "OPEN";
    static const string         APPROVED                        = "APPROVED";
    static const string         PAID                            = "PAID";

    // static const string         BITCASH_TRADING_FEE             = "Bitcash Trading Fee";
};