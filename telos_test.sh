NEW TEST NET: 

Private key: 5HwnoWBuuRmNdcqwBzd1LABFRKnTk2RY2kUMYKkZfF8tKodubtK
Public key: EOS5tEdJd32ANvoxSecRnY5ucr1jbzaVN2rQZegj6NxsevGU8JoaJ

hyphadaomain  -- the primary account associated with the DAO
hyphatokens1  -- token account for HYPHA and PRESEED

Users:
johnnyhypha1
samanthahyph
jameshypha11
thomashypha1
haydenhypha1


cleos -u https://test.telos.kitchen set contract hyphadaomain hyphadao/hyphadao
cleos -u https://test.telos.kitchen set contract hyphatokens1 ~/dev/token/token/

cleos -u https://test.telos.kitchen push action eosio.token transfer '["teloskitchen", "hyphadaomain", "1000.0000 TLOS", "for testing"]' -p teloskitchen
cleos -u https://test.telos.kitchen push action eosio.token transfer '["hyphadaomain", "trailservice", "1000.0000 TLOS", "deposit"]' -p hyphadaomain
cleos -u https://test.telos.kitchen push action trailservice newtreasury '["hyphadaomain", "1000000000.00 HVOICE", "public"]' -p hyphadaomain
cleos -u https://test.telos.kitchen push action trailservice toggle '["0,HVOICE", "transferable"]' -p hyphadaomain

## Enrolling a user in hypha DAO
cleos -u https://test.telos.kitchen push action trailservice regvoter '["hyphadaomain", "0,HVOICE", null]' -p hyphadaomain
cleos -u https://test.telos.kitchen push action trailservice mint '["hyphadaomain", "1 HVOICE", "original mint"]' -p hyphadaomain

cleos -u https://test.telos.kitchen push action trailservice regvoter '["haydenhypha1", "0,HYPHA", null]' -p haydenhypha1
cleos -u https://test.telos.kitchen push action trailservice mint '["haydenhypha1", "1 HVOICE", "original mint"]' -p hyphadaomain

cleos -u https://test.telos.kitchen push action trailservice castvote '["haydenhypha1", "hypha1......2", ["pass"]]' -p haydenhypha1

cleos -u https://test.telos.kitchen push action hyphadaomain closeprop '["roles", 0'

cleos -u https://test.telos.kitchen push action hyphadaomain proposerole '["johnnyhypha1", "Underwater Basketweaver", "Weave baskets at the bottom of the sea", "We make *great* baskets.", "11 HYPHA", "11.00000000 PRESEED", "11 HVOICE", 0, 10]' -p johnnyhypha1
cleos -u https://test.telos.kitchen push action trailservice castvote '["haydenhypha1", "hypha1.....1d", ["pass"]]' -p haydenhypha1
cleos -u https://test.telos.kitchen push action hyphadaomain closeprop '["roles", 26]' -p haydenhypha1


# You can run these statements over and over because the commands end with the same state as the beginning
# The applicant must run these two actions (preferably as the same transaction)
cleos -u https://test.telos.kitchen push action trailservice regvoter '["hyphalondon2", "0,HVOICE", null]' -p hyphalondon2
cleos -u https://test.telos.kitchen push action hyphadaomain apply '["hyphalondon2", "I met with Debbie at the regen conference and we talked about Hypha. I would like to join."]' -p hyphalondon2

cleos -u https://test.telos.kitchen push action hyphadaomain enroll '["johnnyhypha1", "hyphalondon2", "Debbie confirmed she made this referral"]' -p johnnyhypha1

# The account can be unregistered if they transfer away their HVOICE and call unregvoter
cleos -u https://test.telos.kitchen push action hyphadaomain removemember '["hyphalondon2"]' -p hyphadaomain  
cleos -u https://test.telos.kitchen push action trailservice transfer '["hyphalondon2", "johnnyhypha1", "1 HVOICE", "memo"]' -p hyphalondon2
cleos -u https://test.telos.kitchen push action trailservice unregvoter '["hyphalondon2", "0,HVOICE"]' -p hyphalondon2


cleos -u https://test.telos.kitchen push action trailservice castvote '["haydenhypha1", "hypha1.....", ["pass"]]' -p haydenhypha1
cleos -u https://test.telos.kitchen push action hyphadaomain closeprop '["payouts", 0]' -p haydenhypha1
cleos -u https://test.telos.kitchen push action hyphadaomain setlastballt '["hypha1.....1f"]' -p hyphadaomain


cleos -u https://test.telos.kitchen push action -sjd -x 86400 eosio.token transfer '["hyphadaomain", "johnnyhypha1", "1.2345 TLOS", "testing native approval trx"]' -p hyphadaomain > hypha_xfer_test.json
cleos -u https://test.telos.kitchen push action hyphadaomain 
cleos -u https://api.eosnewyork.io multisig propose_trx unpause '[{"actor": "gftma.x", "permission": "active"}, {"actor": "danielflora3", "permission": "active"}]' ./gyftietokens_unpause.json gftma.x

cleos -u https://test.telos.kitchen push action hyphatokens1 issue '["hyphadaomain", "100 HYPHA", "memo"]' -p hyphadaomain
cleos -u https://test.telos.kitchen push action hyphatokens1 transfer '["hyphadaomain", "haydenhypha1", "1 HYPHA", "memo"]' -p hyphadaomain

## new format

cleos -u https://test.telos.kitchen push action hyphadaomain mapit '{"key":"strings":"value":{"key":"tester":"value":"tester value"}}' -p hyphadaomain

cleos -u https://test.telos.kitchen push action hyphadaomain propose '{"proposer":"johnnyhypha1", 
                                                                        "proposal_type":"role", 
                                                                        "trx_action_name":"newrole", 
                                                                        "names":null, 
                                                                        "strings":{   
                                                                            "name":"Underwater Basketweaver", 
                                                                            "description":"Weave baskets at the bottom of the sea",
                                                                            "content":"We make *great* baskets."
                                                                        }, 
                                                                        "assets":{
                                                                            "hypha_amount":"11 HYPHA", 
                                                                            "seeds_amount":"11.00000000 SEEDS", 
                                                                            "hvoice_amount":"11 HVOICE"
                                                                        }, 
                                                                        "time_points":null, 
                                                                        "ints":{
                                                                            "start_period":"41", 
                                                                            "end_period":"51"
                                                                        }, 
                                                                        "floats":null,
                                                                        "trxs":null' -p johnnyhypha1



##
cleos -u https://test.telos.kitchen push action hyphadaomain setconfig '["hyphatokens1", "trailservice"]' -p hyphadaomain

cleos -u https://test.telos.kitchen push action hyphatokens1 create '["hyphadaomain", "1000000000000 HYPHA"]' -p hyphatokens1
cleos -u https://test.telos.kitchen push action hyphatokens1 create '["hyphadaomain", "1000000.00000000 PRESEED"]' -p hyphatokens1
cleos -u https://test.telos.kitchen push action hyphatokens1 issue '["hyphadaomain", "1.00000000 PRESEED", "memo"]' -p hyphadaomain
cleos -u https://test.telos.kitchen push action hyphatokens1 transfer '["hyphadaomain", "johnnyhypha1", "1.00000000 PRESEED", "memo"]' -p hyphadaomain

cleos -u https://test.telos.kitchen push action hyphadaomain addmember '["johnnyhypha1"]' -p hyphadaomain
cleos -u https://test.telos.kitchen push action hyphadaomain addmember '["samanthahyph"]' -p hyphadaomain
cleos -u https://test.telos.kitchen push action hyphadaomain addmember '["jameshypha11"]' -p hyphadaomain
cleos -u https://test.telos.kitchen push action hyphadaomain addmember '["thomashypha1"]' -p hyphadaomain
cleos -u https://test.telos.kitchen push action hyphadaomain addmember '["haydenhypha1"]' -p hyphadaomain


cleos -u https://test.telos.kitchen push action hyphatokens1 issue '["hyphadaomain", "1 HYPHA", "memo"]' -p hyphadaomain
cleos -u https://test.telos.kitchen push action hyphatokens1 transfer '["hyphadaomain", "johnnyhypha1", "1 HYPHA", "memo"]' -p hyphadaomain

# cleos -u https://test.telos.kitchen push action hyphatokens1 issue '["samanthahyph", "1 HYPHA", "memo"]' -p hyphadaomain
# cleos -u https://test.telos.kitchen push action hyphatokens1 issue '["jameshypha11", "1 HYPHA", "memo"]' -p hyphadaomain
# cleos -u https://test.telos.kitchen push action hyphatokens1 issue '["thomashypha1", "1 HYPHA", "memo"]' -p hyphadaomain
# cleos -u https://test.telos.kitchen push action hyphatokens1 issue '["haydenhypha1", "1 HYPHA", "memo"]' -p hyphadaomain

cleos -u https://test.telos.kitchen push action eosio.trail regtoken '["1000000000000 HVOIC", "hyphadaomain", "https://dao.hypha.earth"]' -p hyphadaomain
cleos -u https://test.telos.kitchen push action eosio.trail issuetoken '["hyphadaomain", "johnnyhypha1", "1 HVOIC", false]' -p hyphadaomain

# cleos -u https://test.telos.kitchen push action eosio.trail issuetoken '["hyphadaomain", "hyphamember2", "1 HVOICE", false]' -p hyphadaomain
# cleos -u https://test.telos.kitchen push action eosio.trail issuetoken '["hyphadaomain", "hyphamember3", "1 HVOICE", false]' -p hyphadaomain
# cleos -u https://test.telos.kitchen push action eosio.trail issuetoken '["hyphadaomain", "hyphamember4", "1 HVOICE", false]' -p hyphadaomain
# cleos -u https://test.telos.kitchen push action eosio.trail issuetoken '["hyphadaomain", "hyphamember5", "1 HVOICE", false]' -p hyphadaomain


cleos -u https://test.telos.kitchen push action hyphadaomain proposerole '["johnnyhypha1", "Underwater Basketweaver", "Weave baskets at the bottom of the sea", "We make *great* baskets.", "11 HYPHA", "11.00000000 PRESEED", "11 HVOICE", 0, 10]' -p johnnyhypha1
cleos -u https://test.telos.kitchen push action hyphadaomain propassign '["johnnyhypha1", "johnnyhypha1", 0, "https://joinseeds.com", "I am a professional basket maker and scuba diver", 0, 6, 1.000000000]' -p johnnyhypha1

cleos -u https://test.telos.kitchen push action eosio.trail castvote '["johnnyhypha1", 1, 1]' -p johnnyhypha1

# cleos -u https://test.telos.kitchen push action eosio.trail castvote '["hyphamember2", 66, 1]' -p hyphamember2
# cleos -u https://test.telos.kitchen push action eosio.trail castvote '["hyphamember3", 66, 1]' -p hyphamember3
# cleos -u https://test.telos.kitchen push action eosio.trail castvote '["hyphamember4", 60, 0]' -p hyphamember4

cleos -u https://test.telos.kitchen push action hyphadaomain closeprop '[0]' -p hyphamember1

cleos -u https://test.telos.kitchen push action hyphadaomain payassign '[0, 0]' -p hyphamember3
cleos -u https://test.telos.kitchen push action hyphadaomain resetperiods '[]' -p hyphadaomain
cleos -u https://test.telos.kitchen push action hyphadaomain assign '[2]' -p hyphadaomain











doesnt work: 3fbdca985be8751bcfd3917bdbae80ea9e33770a
does work: 


62b2e3dd38533cd6f3ced50c3f53c76f40e98651







# Trail experimentation

cleos -u https://test.telos.kitchen get table hyphadaomain hyphadaomain applicants


cleos -u https://test.telos.kitchen get table hyphadaomain hyphadaomain config
cleos -u https://test.telos.kitchen get table hyphadaomain hyphadaomain nominees
cleos -u https://test.telos.kitchen get table hyphadaomain hyphadaomain boardmembers

cleos -u https://test.telos.kitchen get table hyphadaomain hyphadaomain proposals
cleos -u https://test.telos.kitchen get table hyphadaomain hyphadaomain roles
cleos -u https://test.telos.kitchen get table hyphadaomain hyphadaomain assignments
cleos -u https://test.telos.kitchen get table hyphadaomain hyphadaomain roleprops

cleos -u https://test.telos.kitchen get table eosio.trail eosio.trail registries
cleos -u https://test.telos.kitchen get table -lower 50  eosio.trail eosio.trail ballots
cleos -u https://test.telos.kitchen get table eosio.trail eosio.trail elections
cleos -u https://test.telos.kitchen get table eosio.trail eosio.trail leaderboards --lower 4
cleos -u https://test.telos.kitchen get table eosio.trail eosio.trail proposals

cleos -u https://test.telos.kitchen push action hyphadaobali reset '[]' -p hyphadaobali
cleos -u https://test.telos.kitchen push action hyphadaobal1 inithvoice '["https://joinseeds.com"]' -p hyphadaobal1
cleos -u https://test.telos.kitchen push action hyphadaobali initsteward '["https://joinseeds.com"]' -p hyphaboard11

cleos -u https://test.telos.kitchen push action eosio.trail issuetoken '["hyphadaobal1", "hyphamember1", "1 HYVO", false]' -p hyphadaobal1
cleos -u https://test.telos.kitchen push action eosio.trail issuetoken '["hyphadaobal1", "hyphamember2", "1 HYVO", false]' -p hyphadaobal1
cleos -u https://test.telos.kitchen push action eosio.trail issuetoken '["hyphadaobal1", "hyphamember3", "1 HYVO", false]' -p hyphadaobal1
cleos -u https://test.telos.kitchen push action eosio.trail issuetoken '["hyphadaobal1", "hyphamember4", "1 HYVO", false]' -p hyphadaobal1
cleos -u https://test.telos.kitchen push action eosio.trail issuetoken '["hyphadaobal1", "hyphamember5", "1 HYVO", false]' -p hyphadaobal1

cleos -u https://test.telos.kitchen push action hyphaboard11 nominate '["hyphamember1", "hyphamember1"]' -p hyphamember1
cleos -u https://test.telos.kitchen push action hyphaboard11 makeelection '["hyphamember1", "https://joinseeds.com"]' -p hyphamember1

cleos -u https://test.telos.kitchen push action hyphaboard11 addcand '["hyphamember1", "https://joinseeds.com"]' -p hyphamember1


cleos -u https://test.telos.kitchen push action eosio.trail castvote '["hyphamember1", 19, 0]' -p hyphamember1
cleos -u https://test.telos.kitchen push action eosio.trail castvote '["hyphamember2", 19, 1]' -p hyphamember2
cleos -u https://test.telos.kitchen push action eosio.trail castvote '["hyphamember3", 19, 1]' -p hyphamember3
cleos -u https://test.telos.kitchen push action eosio.trail castvote '["hyphamember4", 19, 1]' -p hyphamember4

cleos -u https://test.telos.kitchen push action hyphaboard11 endelection '["hyphamember1"]' -p hyphamember1


##### Propose a role
cleos -u https://test.telos.kitchen push action hyphadaobal1 proposerole '["hyphamember1", "Strawberry Gatherer", "https://joinseeds.com", "Farmer growing food", "12 HYPHA", "9 PRESEED", "15 HYVOICE"]' -p hyphamember1
cleos -u https://test.telos.kitchen push action eosio.trail castvote '["hyphamember4", 41, 1]' -p hyphamember4
cleos -u https://test.telos.kitchen push action eosio.trail castvote '["hyphamember2", 41, 1]' -p hyphamember2
cleos -u https://test.telos.kitchen push action eosio.trail castvote '["hyphamember3", 41, 1]' -p hyphamember3
cleos -u https://test.telos.kitchen push action hyphadaobal1 closeprop '["hyphamember1", 0]' -p hyphamember1

#####  Propose an assignment
cleos -u https://test.telos.kitchen push action hyphadaobal1 propassign '["hyphamember1", "hyphamember1", 0, "https://joinseeds.com", "I would like this job", 0, 1.000000000]' -p hyphamember1
cleos -u https://test.telos.kitchen push action eosio.trail castvote '["hyphamember1", 34, 1]' -p hyphamember1
cleos -u https://test.telos.kitchen push action eosio.trail castvote '["hyphamember2", 34, 1]' -p hyphamember2
cleos -u https://test.telos.kitchen push action eosio.trail castvote '["hyphamember3", 34, 1]' -p hyphamember3
cleos -u https://test.telos.kitchen push action hyphadaobal1 closeprop '[1]' -p hyphamember1

##### Propose a contribution
cleos -u https://test.telos.kitchen set contract hyphadaobal1 hyphadao/hyphadao
cleos -u https://test.telos.kitchen push action hyphadaobal1 proppayout '["hyphamember2", "hyphamember2", "Investment", "https://joinseeds.com", "2000 HHH", "10000.00000000 PPP", "5 HYVO", "2019-07-05T05:49:01.500"]' -p hyphamember2
cleos -u https://test.telos.kitchen push action eosio.trail castvote '["hyphamember1", 50, 1]' -p hyphamember1
cleos -u https://test.telos.kitchen push action eosio.trail castvote '["hyphamember2", 50, 1]' -p hyphamember2
cleos -u https://test.telos.kitchen push action eosio.trail castvote '["hyphamember3", 50, 1]' -p hyphamember3
cleos -u https://test.telos.kitchen push action hyphadaobal1 closeprop '["hyphamember2", 9]' -p hyphamember2

cleos -u https://test.telos.kitchen get table hyphatokens1 hyphamember2 accounts

cleos -u https://test.telos.kitchen push action hyphadaobal1 makepayout '[2]' -p hyphadaobal1
cleos -u https://test.telos.kitchen push action hyphadaobal1 reset '[]' -p hyphadaobal1





cleos -u https://test.telos.kitchen push action hyphadaobali setconfig '["hyphatokens1", "hyphatokens1"]' -p hyphadaobali


cleos -u https://test.telos.kitchen push action eosio updateauth '{
    "account": "hyphadaomain",
    "permission": "owner",
    "parent": "",
    "auth": {
        "keys": [
            {
                "key": "EOS5tEdJd32ANvoxSecRnY5ucr1jbzaVN2rQZegj6NxsevGU8JoaJ",
                "weight": 1
            }
        ],
        "threshold": 1,
        "accounts": [
            {
                "permission": {
                    "actor": "hyphadaomain",
                    "permission": "eosio.code"
                },
                "weight": 1
            }
        ],
        "waits": []
    }
}' -p hyphadaomain@owner

cleos -u https://test.telos.kitchen push action eosio updateauth '{
    "account": "hyphadaomain",
    "permission": "active",
    "parent": "owner",
    "auth": {
        "keys": [
            {
                "key": "EOS5tEdJd32ANvoxSecRnY5ucr1jbzaVN2rQZegj6NxsevGU8JoaJ",
                "weight": 1
            }
        ],
        "threshold": 1,
        "accounts": [
            {
                "permission": {
                    "actor": "hyphadaomain",
                    "permission": "eosio.code"
                },
                "weight": 1
            }
        ],
        "waits": []
    }
}' -p hyphadaomain@owner




------------------------------------------------------------------------------------------------
OLD TEST NET: 

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
hyphatokens1
