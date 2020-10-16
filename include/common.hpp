#pragma once

#include <eosio/symbol.hpp>

using namespace eosio;
using std::string;

namespace common {

    static const symbol         S_HYPHA                         ("HYPHA", 2);
    static const symbol         S_HVOICE                        ("HVOICE", 2);
    static const symbol         S_SEEDS                         ("SEEDS", 4);
    static const symbol         S_HUSD                          ("HUSD", 2);

    static const asset          RAM_ALLOWANCE                   = asset (20000, symbol("TLOS", 4));

    // 365.25 / 7.4
    static const float          PHASES_PER_YEAR                 = 49.3581081081;

    // 49.36 phases per annum, so each phase is 2.026% of the total
    static const float          PHASE_TO_YEAR_RATIO             = 0.02026009582;

    // content keys
    static const string         TITLE                     = string ("title");
    static const string         DESCRIPTION                        = string ("description");
    static const string         CONTENT                    = string ("content");
    static const string         ICON                    = string ("icon");


    static const string         BADGE_STRING                = string ("badge");
    static const string         DETAILS                     = string ("details");
    static const string         TYPE                        = string ("type");
    static const string         ASSIGNEE                    = string ("assignee");
    static const string         MEMBER_STRING               = string ("member");
    static const string         SYSTEM                      = string ("system");
    static const string         BALLOT_ID                   = string ("ballot_id");
    
    // graph edge names
    static constexpr name       HOLDS_BADGE                 = name ("holdsbadge");
    static constexpr name       HELD_BY                     = name ("heldby");
    static constexpr name       OWNED_BY                    = name ("ownedby");
    static constexpr name       OWNS                        = name ("owns");
    static constexpr name       MEMBER_OF                   = name ("memberof");

    static constexpr name       ASSIGN_BADGE                = name ("assignbadge");

    // graph edges hanging off of primary DHO node
    static constexpr name       BADGE_NAME                  = name ("badge");
    static constexpr name       PROPOSAL                    = name ("proposal");
    static constexpr name       FAILED_PROPS                = name ("failedprops");
    static constexpr name       PASSED_PROPS                = name ("passedprops");
    static constexpr name       MEMBER                      = name ("member");

    static const uint64_t       NO_ASSIGNMENT                   = -1;         
};