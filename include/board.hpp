#ifndef BOARD_H
#define BOARD_H

#include <eosio/eosio.hpp>
#include <eosio/asset.hpp>
#include <eosio/singleton.hpp>
#include <eosio/multi_index.hpp>


class Board {

    public:

        #pragma region native

        struct permission_level_weight {
            permission_level  permission;
            uint16_t          weight;
        };

        struct key_weight {
            eosio::public_key  key;
            uint16_t           weight;
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
            name        publisher;
            uint8_t     max_board_seats               = 9; // NOTE: adjustable by board members
            uint8_t     open_seats                    = 9;
            uint64_t    open_election_id              = 0;
            uint32_t    holder_quorum_divisor         = 5;
            uint32_t    board_quorum_divisor          = 2;
            uint32_t    issue_duration                = 360;
            uint32_t    start_delay                   = 1;
            uint32_t    leaderboard_duration          = 360;
            uint32_t    election_frequency            = 360*3;
            uint32_t    last_board_election_time      ;
            bool        is_active_election            = false;

            uint64_t primary_key() const { return publisher.value; }
        };

        struct [[eosio::table]] Nominee {
            name      nominee;
            uint64_t  primary_key() const { return nominee.value; }
        };

        struct [[eosio::table]] Steward {
            name     member;
            uint64_t primary_key() const { return member.value; }
        };

        typedef multi_index<"nominees"_n, Nominee> nominee_table;
        typedef multi_index<"stewards"_n, Steward> steward_table;

        typedef singleton<"bconfig"_n, BoardConfig> board_config_table;
        typedef multi_index<"bconfig"_n, BoardConfig> board_config_table_placeholder;

        board_config_table board_configs;
        BoardConfig _config;

        void makeelection(const name holder, const string info_url);
        void addcand(name nominee, string info_link);
        void removecand(name candidate);
        void endelection(name holder);
        void removemember(name member_to_remove);

    private:

        name        contract;

        #pragma region Helper_Functions
        BoardConfig get_default_config();
        void        add_steward             (name nominee);
        void        rmv_steward             (name member);
        void        addseats                (name member, uint8_t num_seats);
        bool        is_steward              (name user);
        bool        is_nominee              (name user);
        bool        is_hvoice_holder        (name user);
        bool        is_steward_holder       (name user);
        bool        is_term_expired         ();
        void        remove_and_seize_all    ();
        void        remove_and_seize        (name member);
        void        set_permissions         (vector<permission_level_weight> perms);
        uint8_t     get_occupied_seats      ();
        vector<permission_level_weight> perms_from_members();
        #pragma endregion Helper_Functions