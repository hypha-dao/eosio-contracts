cleos -u https://eos.greymass.com push action eosio updateauth '{
    "account": "thehyphabank",
    "permission": "active",
    "parent": "owner",
    "auth": {
        "keys": [
            {
                "key": "EOS5PEdGhBeDd4hSN79zv9GnwKRVUW6ZXwsTtTzkNJmt3NDrsWXhd",
                "weight": 1
            }
        ],
        "threshold": 3,
        "accounts": [
            {
                "permission": {
                    "actor": "amazonjungle",
                    "permission": "active"
                },
                "weight": 1
            },
            {
                "permission": {
                    "actor": "illum1nation",
                    "permission": "active"
                },
                "weight": 1
            },
            {
                "permission": {
                    "actor": "onealchemist",
                    "permission": "active"
                },
                "weight": 1
            },
            {
                "permission": {
                    "actor": "sevenflash42",
                    "permission": "active"
                },
                "weight": 1
            },
             {
                "permission": {
                    "actor": "tchopeosacc1",
                    "permission": "active"
                },
                "weight": 1
            },
        ],
        "waits": []
    }
}' -p thehyphabank@owner


cleos -u https://eos.greymass.com push action -sjd -x 604800 eosio.token transfer '["thehyphabank", "vishalvishal", "651.0417 EOS", "vishal"]' -p thehyphabank > packed_transfer.json
cleos -u https://eos.greymass.com multisig propose_trx vishal '[{"actor": "amazonjungle", "permission": "active"},{"actor": "illum1nation", "permission": "active"},{"actor": "onealchemist", "permission": "active"},{"actor": "sevenflash42", "permission": "active"},{"actor": "tchopeosacc1", "permission": "active"}]' ./packed_transfer.json amazonjungle
cleos -u https://eos.greymass.com multisig approve amazonjungle vishal '{"actor": "amazonjungle", "permission": "active"}' -p amazonjungle
cleos -u https://api.telos.kitchen multisig exec eosiodetroit bbanophone

cleos -u https://api.telos.kitchen multisig approve eosiodetroit bbanophone  '{"actor": "teloskitchen", "permission": "active"}' -p teloskitchen


tchopeosacc1