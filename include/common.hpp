#pragma once

#include <eosio/symbol.hpp>

using namespace eosio;
using std::string;

namespace common {

    static const symbol         S_HYPHA                         ("HYPHA", 2);
    static const symbol         S_HVOICE                        ("HVOICE", 2);
    static const symbol         S_SEEDS                         ("SEEDS", 4);
    static const symbol         S_HUSD                          ("HUSD", 2);

    // 365.25 / 7.4
    static const float          PHASES_PER_YEAR                 = 49.3581081081;

    // 49.36 phases per annum, so each phase is 2.026% of the total
    static const float          PHASE_TO_YEAR_RATIO             = 0.02026009582;
   
    static const uint64_t       NO_ASSIGNMENT                   = -1;         
};