Private key: 5J1gYLAc4GUo7EXNAXyaZTgo3m3SxtxDygdVUsNL4Par5Swfy1q
Public key: EOS6vvAofsMC5RJyY6fRHcyiQLNjDGukX6tRUoF1WEc63idQ3BqJn

hyphadactest
hyphatoken11
hyphaboard11
hyphaboard11
hyphamember1
hyphamember2
hyphamember3
hyphamember4
hyphamember5

cleos -u http://testnet.telosusa.io set contract hyphaboard11 hyphadac/hyphadac
cleos -u http://testnet.telosusa.io set contract hyphatoken11 hyphadac/eosiotoken

cleos -u http://testnet.telosusa.io push action hyphdactest setconfig '[10, "hyphatoken11"]' -p hyphadactest
cleos -u http://testnet.telosusa.io push action hyphdactest newrole '["blockdev", "blockchain developer", "10 HYPHA", "10 PRESEED", "10 HVOICE"]' -p hyphdactest

cleos -u http://testnet.telosusa.io push action hypha




# Trail experimentation

cleos -u https://testnet.telos.caleos.io set contract hyphaboard11 hyphadac/hyphadac

cleos -u https://testnet.telos.caleos.io get table hyphaboardtk hyphaboard11 config
cleos -u https://testnet.telos.caleos.io get table hyphaboard11 hyphaboard11 nominees
cleos -u https://testnet.telos.caleos.io get table hyphaboard11 hyphaboard11 boardmembers
cleos -u https://testnet.telos.caleos.io get table hyphaboard11 hyphaboard11 issues


cleos -u https://testnet.telos.caleos.io get table eosio.trail eosio.trail registries
cleos -u https://testnet.telos.caleos.io get table -l 100 eosio.trail eosio.trail ballots
cleos -u https://testnet.telos.caleos.io get table eosio.trail eosio.trail elections
cleos -u https://testnet.telos.caleos.io get table eosio.trail eosio.trail leaderboards --lower 4
cleos -u https://testnet.telos.caleos.io get table eosio.trail eosio.trail proposals

cleos -u https://testnet.telos.caleos.io push action hyphaboard11 setconfig '[360, "hyphamember1"]' -p hyphaboard11
cleos -u https://testnet.telos.caleos.io push action hyphaboard11 inithvoice '["https://joinseeds.com"]' -p hyphaboard11
cleos -u https://testnet.telos.caleos.io push action hyphaboard11 initsteward '["https://joinseeds.com"]' -p hyphaboard11

cleos -u https://testnet.telos.caleos.io push action eosio.trail issuetoken '["hyphaboard11", "hyphamember1", "1 HVOICE", false]' -p hyphaboard11
cleos -u https://testnet.telos.caleos.io push action eosio.trail issuetoken '["hyphaboard11", "hyphamember2", "1 HVOICE", false]' -p hyphaboard11
cleos -u https://testnet.telos.caleos.io push action eosio.trail issuetoken '["hyphaboard11", "hyphamember3", "1 HVOICE", false]' -p hyphaboard11
cleos -u https://testnet.telos.caleos.io push action eosio.trail issuetoken '["hyphaboard11", "hyphamember4", "1 HVOICE", false]' -p hyphaboard11
cleos -u https://testnet.telos.caleos.io push action eosio.trail issuetoken '["hyphaboard11", "hyphamember5", "1 HVOICE", false]' -p hyphaboard11

cleos -u https://testnet.telos.caleos.io push action hyphaboard11 nominate '["hyphamember1", "hyphamember1"]' -p hyphamember1
cleos -u https://testnet.telos.caleos.io push action hyphaboard11 makeelection '["hyphamember1", "https://joinseeds.com"]' -p hyphamember1

cleos -u https://testnet.telos.caleos.io push action hyphaboard11 addcand '["hyphamember1", "https://joinseeds.com"]' -p hyphamember1


cleos -u https://testnet.telos.caleos.io push action eosio.trail castvote '["hyphamember1", 19, 0]' -p hyphamember1
cleos -u https://testnet.telos.caleos.io push action eosio.trail castvote '["hyphamember2", 19, 1]' -p hyphamember2
cleos -u https://testnet.telos.caleos.io push action eosio.trail castvote '["hyphamember3", 19, 1]' -p hyphamember3
cleos -u https://testnet.telos.caleos.io push action eosio.trail castvote '["hyphamember4", 19, 1]' -p hyphamember4

cleos -u https://testnet.telos.caleos.io push action hyphaboard11 endelection '["hyphamember1"]' -p hyphamember1


##### Propose a role
cleos -u https://testnet.telos.caleos.io push action hyphaboard11 proposerole '["hyphamember3", "blockdev", "https://joinseeds.com", "blockchain developer", "10 HYPHA", "10 PRESEED", "10 HVOICE"]' -p hyphamember3

cleos -u https://testnet.telos.caleos.io push action eosio.trail castvote '["hyphamember1", 20, 1]' -p hyphamember1
cleos -u https://testnet.telos.caleos.io push action eosio.trail castvote '["hyphamember2", 21, 1]' -p hyphamember2
cleos -u https://testnet.telos.caleos.io push action eosio.trail castvote '["hyphamember3", 21, 1]' -p hyphamember3

cleos -u https://testnet.telos.caleos.io push action hyphaboard11 closeissue '["hyphamember3", "hyphamember3"]' -p hyphamember3

#####  Propose an assignment
cleos -u https://testnet.telos.caleos.io push action hyphaboard11 propassign '["hyphamember3", "hyphamember3", "blockdev", "https://joinseeds.com", "I would like this job", 2019-07-04T06:11:45.500", 1.000000000]' -p hyphamember3
cleos -u https://testnet.telos.caleos.io push action eosio.trail castvote '["hyphamember1", 23, 1]' -p hyphamember1
cleos -u https://testnet.telos.caleos.io push action eosio.trail castvote '["hyphamember2", 23, 1]' -p hyphamember2
cleos -u https://testnet.telos.caleos.io push action eosio.trail castvote '["hyphamember3", 23, 1]' -p hyphamember3
cleos -u https://testnet.telos.caleos.io push action hyphaboard11 closeissue '["hyphamember3", "hyphamember3"]' -p hyphamember3

##### Propose a contribution
cleos -u https://testnet.telos.caleos.io push action hyphaboard11 propcontrib '["hyphamember3", "hyphamember3", "blockdev", "https://joinseeds.com", "I would like this job", 2019-07-04T06:11:45.500", 1.000000000]' -p hyphamember3
cleos -u https://testnet.telos.caleos.io push action eosio.trail castvote '["hyphamember1", 23, 1]' -p hyphamember1
cleos -u https://testnet.telos.caleos.io push action eosio.trail castvote '["hyphamember2", 23, 1]' -p hyphamember2
cleos -u https://testnet.telos.caleos.io push action eosio.trail castvote '["hyphamember3", 23, 1]' -p hyphamember3
cleos -u https://testnet.telos.caleos.io push action hyphaboard11 closeissue '["hyphamember3", "hyphamember3"]' -p hyphamember3

1562221738
1562222098

1562221678

cleos -u https://testnet.telos.caleos.io push action eosio updateauth '{
    "account": "hyphaboard11",
    "permission": "active",
    "parent": "owner",
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
                    "actor": "hyphaboard11",
                    "permission": "eosio.code"
                },
                "weight": 1
            }
        ],
        "waits": []
    }
}' -p hyphaboard11@owner