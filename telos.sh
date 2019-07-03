Private key: 5J1gYLAc4GUo7EXNAXyaZTgo3m3SxtxDygdVUsNL4Par5Swfy1q
Public key: EOS6vvAofsMC5RJyY6fRHcyiQLNjDGukX6tRUoF1WEc63idQ3BqJn

hyphadactest
hyphatoken11
hyphaboardtk
hyphaboard11
hyphamember1
hyphamember2
hyphamember3
hyphamember4
hyphamember5

cleos -u http://testnet.telosusa.io set contract hyphadactest hyphadac/hyphadac
cleos -u http://testnet.telosusa.io set contract hyphatoken11 hyphadac/eosiotoken

cleos -u http://testnet.telosusa.io push action hyphdactest setconfig '[10, "hyphatoken11"]' -p hyphadactest
cleos -u http://testnet.telosusa.io push action hyphdactest newrole '["blockdev", "blockchain developer", "10 HYPHA", "10 PRESEED", "10 HVOICE"]' -p hyphdactest

cleos -u http://testnet.telosusa.io push action hypha




# Trail experimentation

cleos -u https://testnet.telos.caleos.io set contract hyphaboardtk hyphadac/telos.tfvt

cleos -u https://testnet.telos.caleos.io get table hyphaboardtk hyphaboardtk config
cleos -u https://testnet.telos.caleos.io get table hyphaboardtk hyphaboardtk nominees
cleos -u https://testnet.telos.caleos.io get table hyphaboardtk hyphaboardtk boardmembers
cleos -u https://testnet.telos.caleos.io get table hyphaboardtk hyphaboardtk issues
cleos -u https://testnet.telos.caleos.io get table eosio.trail eosio.trail registries
cleos -u https://testnet.telos.caleos.io get table eosio.trail eosio.trail ballots --lower 15
cleos -u https://testnet.telos.caleos.io get table eosio.trail eosio.trail elections
cleos -u https://testnet.telos.caleos.io get table eosio.trail eosio.trail leaderboards --lower 4
cleos -u https://testnet.telos.caleos.io get table eosio.trail eosio.trail proposals

cleos -u https://testnet.telos.caleos.io push action hyphaboardtk setconfig '["hyphamember1"]' -p hyphaboardtk
cleos -u https://testnet.telos.caleos.io push action hyphaboardtk inittfvt '["https://joinseeds.com"]' -p hyphaboardtk
cleos -u https://testnet.telos.caleos.io push action hyphaboardtk inittfboard '["https://joinseeds.com"]' -p hyphaboardtk

cleos -u https://testnet.telos.caleos.io push action eosio.trail issuetoken '["hyphaboardtk", "hyphamember1", "1 HYPHA", false]' -p hyphaboardtk
cleos -u https://testnet.telos.caleos.io push action eosio.trail issuetoken '["hyphaboardtk", "hyphamember2", "1 HYPHA", false]' -p hyphaboardtk
cleos -u https://testnet.telos.caleos.io push action eosio.trail issuetoken '["hyphaboardtk", "hyphamember3", "1 HYPHA", false]' -p hyphaboardtk
cleos -u https://testnet.telos.caleos.io push action eosio.trail issuetoken '["hyphaboardtk", "hyphamember4", "1 HYPHA", false]' -p hyphaboardtk
cleos -u https://testnet.telos.caleos.io push action eosio.trail issuetoken '["hyphaboardtk", "hyphamember5", "1 HYPHA", false]' -p hyphaboardtk

cleos -u https://testnet.telos.caleos.io push action hyphaboardtk nominate '["hyphamember1", "hyphamember1"]' -p hyphamember1
cleos -u https://testnet.telos.caleos.io push action hyphaboardtk makeelection '["hyphamember1", "https://joinseeds.com"]' -p hyphamember1

cleos -u https://testnet.telos.caleos.io push action hyphaboardtk addcand '["hyphamember1", "https://joinseeds.com"]' -p hyphamember1


cleos -u https://testnet.telos.caleos.io push action eosio.trail castvote '["hyphamember1", 14, 0]' -p hyphamember1
cleos -u https://testnet.telos.caleos.io push action eosio.trail castvote '["hyphamember2", 14, 0]' -p hyphamember2
cleos -u https://testnet.telos.caleos.io push action eosio.trail castvote '["hyphamember3", 14, 0]' -p hyphamember3

cleos -u https://testnet.telos.caleos.io push action hyphaboardtk endelection '["hyphamember1"]' -p hyphamember1


##### Issue voting
cleos -u https://testnet.telos.caleos.io push action hyphaboardtk proposerole '["hyphamember1", "blockdev", "https://joinseeds.com", "blockchain developer", "10 HYPHA", "10 PRESEED", "10 HVOICE"]' -p hyphamember1

cleos -u https://testnet.telos.caleos.io push action eosio.trail castvote '["hyphamember1", 1, 0]' -p hyphamember1
cleos -u https://testnet.telos.caleos.io push action eosio.trail castvote '["hyphamember2", 13, 0]' -p hyphamember2
cleos -u https://testnet.telos.caleos.io push action eosio.trail castvote '["hyphamember3", 13, 0]' -p hyphamember3


cleos -u https://testnet.telos.caleos.io push action eosio.trail castvote '["hyphamember1", 10, 1]' -p hyphamember1
cleos -u https://testnet.telos.caleos.io push action eosio.trail castvote '["hyphamember2", 15, 1]' -p hyphamember2
cleos -u https://testnet.telos.caleos.io push action eosio.trail castvote '["hyphamember3", 15, 1]' -p hyphamember3



cleos -u https://testnet.telos.caleos.io push action hyphaboardtk setconfig '["hyphamember1"]' -p hyphaboardtk







cleos -u https://testnet.telos.caleos.io push action eosio updateauth '{
    "account": "hyphaboardtk",
    "permission": "owner",
    "parent": "",
    "auth": {
        "keys": [
            {
                "key": "EOS6vvAofsMC5RJyY6fRHcyiQLNjDGukX6tRUoF1WEc63idQ3BqJn",
                "weight": 1
            }
        ],
        "threshold": 1,
        "accounts": [
            {
                "permission": {
                    "actor": "hyphaboardtk",
                    "permission": "eosio.code"
                },
                "weight": 1
            }
        ],
        "waits": []
    }
}' -p hyphaboardtk@owner