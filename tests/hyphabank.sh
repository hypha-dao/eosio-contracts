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


cleos -u https://eos.greymass.com push action -sjd -x 86400 eosio.token transfer '["thehyphabank", "amazonjungle", "0.0001 EOS", "test trx"]' -p thehyphabank > test_transfer.json
cleos -u https://eos.greymass.com multisig proposetrx testtrx '[{"actor": "amazonjungle", "permission": "active"},{"actor": "illum1nation", "permission": "active"},{"actor": "onealchemist", "permission": "active"},{"actor": "sevenflash42", "permission": "active"},{"actor": "tchopeosacc1", "permission": "active"}]' ./test_transfer.json amazonjungle
cleos -u https://eos.greymass.com multisig approve amazonjungle testtrx '{"actor": "amazonjungle", "permission": "active"}' -p amazonjungle
cleos -u https://eos.greymass.com multisig exec amazonjungle testtrx

tchopeosacc1