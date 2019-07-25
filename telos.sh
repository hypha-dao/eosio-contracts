Private key: 5J1gYLAc4GUo7EXNAXyaZTgo3m3SxtxDygdVUsNL4Par5Swfy1q
Public key: EOS6vvAofsMC5RJyY6fRHcyiQLNjDGukX6tRUoF1WEc63idQ3BqJn

hyphadaotest
hyphatoken11
hyphaboard11
hyphaboard11
hyphamember1
hyphamember2
hyphamember3
hyphamember4
hyphamember5
hyphadaobali
hyphatoken12


cleos -u https://testnet.telos.caleos.io set contract hyphadaobal1 hyphadao/hyphadao
cleos -u https://testnet.telos.caleos.io set contract hyphatoken12 hyphadao/eosiotoken

cleos -u https://testnet.telos.caleos.io push action hyphadaotst1 newrole '["blockdev", "blockchain developer", "10 HYPHA", "10 PRESEED", "10 HVOICE"]' -p hyphdactest

cleos -u https://testnet.telos.caleos.io push action hypha



Payout Testing
hyphadaotst1

cleos -u https://testnet.telos.caleos.io set contract hyphadaotst1 hyphadao/hyphadao
cleos -u https://testnet.telos.caleos.io push action hyphadaotst1 setconfig '["hyphatoken12", "eosio.trail"]' -p hyphadaotst1

cleos -u https://testnet.telos.caleos.io push action hyphadaotst1 init '[]' -p hyphadaotst1

cleos -u https://testnet.telos.caleos.io push action hyphatoken12 create '["hyphadaotst1", "1000000000000 HYA"]' -p hyphatoken12
cleos -u https://testnet.telos.caleos.io push action hyphatoken12 create '["hyphadaotst1", "1000000.00000000 PRX"]' -p hyphatoken12
cleos -u https://testnet.telos.caleos.io push action hyphatoken12 issue '["hyphamember1", "1.00000000 PRX", "memo"]' -p hyphadaotst1

cleos -u https://testnet.telos.caleos.io push action hyphatoken12 issue '["hyphamember1", "1 HYA", "memo"]' -p hyphadaotst1
cleos -u https://testnet.telos.caleos.io push action hyphatoken12 issue '["hyphamember2", "1 HYA", "memo"]' -p hyphadaotst1
cleos -u https://testnet.telos.caleos.io push action hyphatoken12 issue '["hyphamember3", "1 HYA", "memo"]' -p hyphadaotst1
cleos -u https://testnet.telos.caleos.io push action hyphatoken12 issue '["hyphamember4", "1 HYA", "memo"]' -p hyphadaotst1
cleos -u https://testnet.telos.caleos.io push action hyphatoken12 issue '["hyphamember5", "1 HYA", "memo"]' -p hyphadaotst1


cleos -u https://testnet.telos.caleos.io push action eosio.trail issuetoken '["hyphadaotst1", "hyphamember1", "1 HVQ", false]' -p hyphadaotst1
cleos -u https://testnet.telos.caleos.io push action eosio.trail issuetoken '["hyphadaotst1", "hyphamember2", "1 HVQ", false]' -p hyphadaotst1
cleos -u https://testnet.telos.caleos.io push action eosio.trail issuetoken '["hyphadaotst1", "hyphamember3", "1 HVQ", false]' -p hyphadaotst1
cleos -u https://testnet.telos.caleos.io push action eosio.trail issuetoken '["hyphadaotst1", "hyphamember4", "1 HVQ", false]' -p hyphadaotst1
cleos -u https://testnet.telos.caleos.io push action eosio.trail issuetoken '["hyphadaotst1", "hyphamember5", "1 HVQ", false]' -p hyphadaotst1

cleos -u https://testnet.telos.caleos.io push action hyphadaotst1 proposerole '["hyphamember1", "blockdev", "https://joinseeds.com", "blockchain developer", "10 HYA", "10.00000000 PRX", "10 HVQ"]' -p hyphamember1

cleos -u https://testnet.telos.caleos.io push action eosio.trail castvote '["hyphamember1", 53, 1]' -p hyphamember1
cleos -u https://testnet.telos.caleos.io push action eosio.trail castvote '["hyphamember2", 53, 1]' -p hyphamember2
cleos -u https://testnet.telos.caleos.io push action eosio.trail castvote '["hyphamember3", 53, 1]' -p hyphamember3
cleos -u https://testnet.telos.caleos.io push action eosio.trail castvote '["hyphamember4", 53, 0]' -p hyphamember4

cleos -u https://testnet.telos.caleos.io push action hyphadaotst1 closeprop '["hyphamember1", 1]' -p hyphamember1

cleos -u https://testnet.telos.caleos.io push action hyphadaotst1 propassign '["hyphamember1", "hyphamember1", 0, "https://joinseeds.com", "I would like this job", 0, 1.000000000]' -p hyphamember1

cleos -u https://testnet.telos.caleos.io push action hyphadaotst1 payassign '[0, 0]' -p hyphamember1























# Trail experimentation

cleos -u https://testnet.telos.caleos.io get table hyphadaotst1 hyphadaotst1 config
cleos -u https://testnet.telos.caleos.io get table hyphadaotst1 hyphadaotst1 nominees
cleos -u https://testnet.telos.caleos.io get table hyphadaotst1 hyphadaotst1 boardmembers

cleos -u https://testnet.telos.caleos.io get table hyphadaotst1 hyphadaotst1 proposals
cleos -u https://testnet.telos.caleos.io get table hyphadaotst1 hyphadaotst1 roles
cleos -u https://testnet.telos.caleos.io get table hyphadaotst1 hyphadaotst1 assignments
cleos -u https://testnet.telos.caleos.io get table hyphadaotst1 hyphadaotst1 roleprops

cleos -u https://testnet.telos.caleos.io get table eosio.trail eosio.trail registries
cleos -u https://testnet.telos.caleos.io get table -lower 50  eosio.trail eosio.trail ballots
cleos -u https://testnet.telos.caleos.io get table eosio.trail eosio.trail elections
cleos -u https://testnet.telos.caleos.io get table eosio.trail eosio.trail leaderboards --lower 4
cleos -u https://testnet.telos.caleos.io get table eosio.trail eosio.trail proposals

cleos -u https://testnet.telos.caleos.io push action hyphadaobali reset '[]' -p hyphadaobali
cleos -u https://testnet.telos.caleos.io push action hyphadaobal1 inithvoice '["https://joinseeds.com"]' -p hyphadaobal1
cleos -u https://testnet.telos.caleos.io push action hyphadaobali initsteward '["https://joinseeds.com"]' -p hyphaboard11

cleos -u https://testnet.telos.caleos.io push action eosio.trail issuetoken '["hyphadaobal1", "hyphamember1", "1 HYVO", false]' -p hyphadaobal1
cleos -u https://testnet.telos.caleos.io push action eosio.trail issuetoken '["hyphadaobal1", "hyphamember2", "1 HYVO", false]' -p hyphadaobal1
cleos -u https://testnet.telos.caleos.io push action eosio.trail issuetoken '["hyphadaobal1", "hyphamember3", "1 HYVO", false]' -p hyphadaobal1
cleos -u https://testnet.telos.caleos.io push action eosio.trail issuetoken '["hyphadaobal1", "hyphamember4", "1 HYVO", false]' -p hyphadaobal1
cleos -u https://testnet.telos.caleos.io push action eosio.trail issuetoken '["hyphadaobal1", "hyphamember5", "1 HYVO", false]' -p hyphadaobal1

cleos -u https://testnet.telos.caleos.io push action hyphaboard11 nominate '["hyphamember1", "hyphamember1"]' -p hyphamember1
cleos -u https://testnet.telos.caleos.io push action hyphaboard11 makeelection '["hyphamember1", "https://joinseeds.com"]' -p hyphamember1

cleos -u https://testnet.telos.caleos.io push action hyphaboard11 addcand '["hyphamember1", "https://joinseeds.com"]' -p hyphamember1


cleos -u https://testnet.telos.caleos.io push action eosio.trail castvote '["hyphamember1", 19, 0]' -p hyphamember1
cleos -u https://testnet.telos.caleos.io push action eosio.trail castvote '["hyphamember2", 19, 1]' -p hyphamember2
cleos -u https://testnet.telos.caleos.io push action eosio.trail castvote '["hyphamember3", 19, 1]' -p hyphamember3
cleos -u https://testnet.telos.caleos.io push action eosio.trail castvote '["hyphamember4", 19, 1]' -p hyphamember4

cleos -u https://testnet.telos.caleos.io push action hyphaboard11 endelection '["hyphamember1"]' -p hyphamember1


##### Propose a role
cleos -u https://testnet.telos.caleos.io push action hyphadaobal1 proposerole '["hyphamember1", "Strawberry Gatherer", "https://joinseeds.com", "Farmer growing food", "12 HYPHA", "9 PRESEED", "15 HYVOICE"]' -p hyphamember1
cleos -u https://testnet.telos.caleos.io push action eosio.trail castvote '["hyphamember4", 41, 1]' -p hyphamember4
cleos -u https://testnet.telos.caleos.io push action eosio.trail castvote '["hyphamember2", 41, 1]' -p hyphamember2
cleos -u https://testnet.telos.caleos.io push action eosio.trail castvote '["hyphamember3", 41, 1]' -p hyphamember3
cleos -u https://testnet.telos.caleos.io push action hyphadaobal1 closeprop '["hyphamember1", 0]' -p hyphamember1

#####  Propose an assignment
cleos -u https://testnet.telos.caleos.io push action hyphadaobal1 propassign '["hyphamember1", "hyphamember1", 0, "https://joinseeds.com", "I would like this job", 0, 1.000000000]' -p hyphamember1
cleos -u https://testnet.telos.caleos.io push action eosio.trail castvote '["hyphamember1", 34, 1]' -p hyphamember1
cleos -u https://testnet.telos.caleos.io push action eosio.trail castvote '["hyphamember2", 34, 1]' -p hyphamember2
cleos -u https://testnet.telos.caleos.io push action eosio.trail castvote '["hyphamember3", 34, 1]' -p hyphamember3
cleos -u https://testnet.telos.caleos.io push action hyphadaobal1 closeprop '["hyphamember3", 1]' -p hyphamember1

##### Propose a contribution
cleos -u https://testnet.telos.caleos.io set contract hyphadaobal1 hyphadao/hyphadao
cleos -u https://testnet.telos.caleos.io push action hyphadaobal1 proppayout '["hyphamember2", "hyphamember2", "Investment", "https://joinseeds.com", "2000 HHH", "10000.00000000 PPP", "5 HYVO", "2019-07-05T05:49:01.500"]' -p hyphamember2
cleos -u https://testnet.telos.caleos.io push action eosio.trail castvote '["hyphamember1", 50, 1]' -p hyphamember1
cleos -u https://testnet.telos.caleos.io push action eosio.trail castvote '["hyphamember2", 50, 1]' -p hyphamember2
cleos -u https://testnet.telos.caleos.io push action eosio.trail castvote '["hyphamember3", 50, 1]' -p hyphamember3
cleos -u https://testnet.telos.caleos.io push action hyphadaobal1 closeprop '["hyphamember2", 9]' -p hyphamember2

cleos -u https://testnet.telos.caleos.io get table hyphatoken12 hyphamember2 accounts

cleos -u https://testnet.telos.caleos.io push action hyphadaobal1 makepayout '[2]' -p hyphadaobal1
cleos -u https://testnet.telos.caleos.io push action hyphadaobal1 reset '[]' -p hyphadaobal1





cleos -u https://testnet.telos.caleos.io push action hyphadaobali setconfig '["hyphatoken12", "hyphatoken12"]' -p hyphadaobali


cleos -u https://testnet.telos.caleos.io push action eosio updateauth '{
    "account": "hyphadaotst1",
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
                    "actor": "hyphadaotst1",
                    "permission": "eosio.code"
                },
                "weight": 1
            }
        ],
        "waits": []
    }
}' -p hyphadaotst1@owner

cleos -u https://testnet.telos.caleos.io push action eosio updateauth '{
    "account": "hyphadaotst1",
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
                    "actor": "hyphadaotst1",
                    "permission": "eosio.code"
                },
                "weight": 1
            }
        ],
        "waits": []
    }
}' -p hyphadaotst1@owner