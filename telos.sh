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


cleos -u http://testnet.telosusa.io set contract hyphaboard11 hyphadao/hyphadao
cleos -u http://testnet.telosusa.io set contract hyphatoken12 hyphadao/eosiotoken

cleos -u https://testnet.telos.caleos.io push action hyphadaobal1 setconfig '["hyphatoken12"]' -p hyphadaobal1
cleos -u http://testnet.telosusa.io push action hyphdactest newrole '["blockdev", "blockchain developer", "10 HYPHA", "10 PRESEED", "10 HVOICE"]' -p hyphdactest

cleos -u http://testnet.telosusa.io push action hypha




# Trail experimentation

cleos -u https://testnet.telos.caleos.io set contract hyphadaobal1 hyphadao/hyphadao
cleos -u https://testnet.telos.caleos.io set contract hyphatoken12 hyphadao/eosiotoken

cleos -u https://testnet.telos.caleos.io push action hyphatoken12 

cleos -u https://testnet.telos.caleos.io get table hyphaboardtk hyphaboard11 config
cleos -u https://testnet.telos.caleos.io get table hyphaboard11 hyphaboard11 nominees
cleos -u https://testnet.telos.caleos.io get table hyphaboard11 hyphaboard11 boardmembers

cleos -u https://testnet.telos.caleos.io get table hyphadaobali hyphadaobali proposals
cleos -u https://testnet.telos.caleos.io get table hyphadaobali hyphadaobali roles
cleos -u https://testnet.telos.caleos.io get table hyphadaobali hyphadaobali assignments
cleos -u https://testnet.telos.caleos.io get table hyphadaobali hyphadaobali 

cleos -u https://testnet.telos.caleos.io get table eosio.trail eosio.trail registries
cleos -u https://testnet.telos.caleos.io get table -l 100 eosio.trail eosio.trail ballots
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
cleos -u https://testnet.telos.caleos.io push action eosio.trail castvote '["hyphamember1", 43, 1]' -p hyphamember1
cleos -u https://testnet.telos.caleos.io push action eosio.trail castvote '["hyphamember2", 43, 1]' -p hyphamember2
cleos -u https://testnet.telos.caleos.io push action eosio.trail castvote '["hyphamember3", 43, 1]' -p hyphamember3
cleos -u https://testnet.telos.caleos.io push action hyphadaobal1 closeprop '["hyphamember2", 2]' -p hyphamember2

cleos -u https://testnet.telos.caleos.io get table hyphatoken12 hyphamember2 accounts

cleos -u https://testnet.telos.caleos.io push action hyphadaobal1 makepayout '[2]' -p hyphadaobal1
cleos -u https://testnet.telos.caleos.io push action hyphadaobali reset '[]' -p hyphadaobali


cleos -u https://testnet.telos.caleos.io push action hyphatoken12 create '["hyphadaobal1", "1000000000000 HHH"]' -p hyphatoken12
cleos -u https://testnet.telos.caleos.io push action hyphatoken12 create '["hyphadaobal1", "1000000.00000000 PPP"]' -p hyphatoken12
cleos -u https://testnet.telos.caleos.io push action hyphatoken12 issue '["hyphamember1", "1 PRESEE", "memo"]' -p hyphadaobal1
cleos -u https://testnet.telos.caleos.io push action hyphatoken12 issue '["hyphamember1", "1 HYPHA", "memo"]' -p hyphadaobali


cleos -u https://testnet.telos.caleos.io push action hyphadaobali setconfig '["hyphatoken12", "hyphatoken12"]' -p hyphadaobali


cleos -u https://testnet.telos.caleos.io push action eosio updateauth '{
    "account": "hyphadaobal1",
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
                    "actor": "hyphadaobal1",
                    "permission": "eosio.code"
                },
                "weight": 1
            }
        ],
        "waits": []
    }
}' -p hyphadaobal1@owner

cleos -u https://testnet.telos.caleos.io push action eosio updateauth '{
    "account": "hyphadaobal1",
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
                    "actor": "hyphadaobal1",
                    "permission": "eosio.code"
                },
                "weight": 1
            }
        ],
        "waits": []
    }
}' -p hyphadaobal1@owner