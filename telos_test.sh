NEW TEST NET: 

Private key: 5HwnoWBuuRmNdcqwBzd1LABFRKnTk2RY2kUMYKkZfF8tKodubtK
Public key: EOS5tEdJd32ANvoxSecRnY5ucr1jbzaVN2rQZegj6NxsevGU8JoaJ

dao.hypha  -- the primary account associated with the DAO
token.hypha  -- token account for HYPHA and PRESEED

Users:
johnnyhypha1
samanthahyph
jameshypha11
thomashypha1
haydenhypha1


cleos -u https://test.telos.kitchen set contract dao.hypha hyphadao/hyphadao
cleos -u https://test.telos.kitchen set contract token.hypha ~/dev/token/token/

cleos -u https://test.telos.kitchen push action eosio.token transfer '["teloskitchen", "dao.hypha", "1000.0000 TLOS", "for testing"]' -p teloskitchen
cleos -u https://test.telos.kitchen push action eosio.token transfer '["dao.hypha", "trailservice", "1000.0000 TLOS", "deposit"]' -p dao.hypha
cleos -u https://test.telos.kitchen push action trailservice newtreasury '["dao.hypha", "1000000000.00 HVOICE", "public"]' -p dao.hypha
cleos -u https://test.telos.kitchen push action trailservice toggle '["2,HVOICE", "transferable"]' -p dao.hypha

## Enrolling a user in hypha DAO
cleos -u https://test.telos.kitchen push action trailservice regvoter '["dao.hypha", "2,HVOICE", null]' -p dao.hypha
cleos -u https://test.telos.kitchen push action trailservice mint '["dao.hypha", "1.00 HVOICE", "original mint"]' -p dao.hypha

cleos -u https://test.telos.kitchen push action trailservice regvoter '["johnnyhypha1", "2,HVOICE", null]' -p johnnyhypha1
cleos -u https://test.telos.kitchen push action trailservice mint '["johnnyhypha1", "1.00 HVOICE", "original mint"]' -p dao.hypha

cleos -u https://test.telos.kitchen push action trailservice castvote '["haydenhypha1", "hypha1......2", ["pass"]]' -p haydenhypha1

cleos -u https://test.telos.kitchen push action dao.hypha closeprop '["roles", 0'

cleos -u https://test.telos.kitchen push action dao.hypha proposerole '["johnnyhypha1", "Underwater Basketweaver", "Weave baskets at the bottom of the sea", "We make *great* baskets.", "11 HYPHA", "11.00000000 PRESEED", "11 HVOICE", 0, 10]' -p johnnyhypha1
cleos -u https://test.telos.kitchen push action trailservice castvote '["haydenhypha1", "hypha1.....1d", ["pass"]]' -p haydenhypha1
cleos -u https://test.telos.kitchen push action dao.hypha closeprop '["roles", 26]' -p haydenhypha1


# You can run these statements over and over because the commands end with the same state as the beginning
# The applicant must run these two actions (preferably as the same transaction)
cleos -u https://test.telos.kitchen push action trailservice regvoter '["hyphalondon2", "0,HVOICE", null]' -p hyphalondon2
cleos -u https://test.telos.kitchen push action dao.hypha apply '["hyphalondon2", "I met with Debbie at the regen conference and we talked about Hypha. I would like to join."]' -p hyphalondon2

cleos -u https://test.telos.kitchen push action dao.hypha enroll '["johnnyhypha1", "hyphalondon2", "Debbie confirmed she made this referral"]' -p johnnyhypha1

# The account can be unregistered if they transfer away their HVOICE and call unregvoter
cleos -u https://test.telos.kitchen push action dao.hypha removemember '["hyphalondon2"]' -p dao.hypha  
cleos -u https://test.telos.kitchen push action trailservice transfer '["hyphalondon2", "johnnyhypha1", "1 HVOICE", "memo"]' -p hyphalondon2
cleos -u https://test.telos.kitchen push action trailservice unregvoter '["hyphalondon2", "0,HVOICE"]' -p hyphalondon2


cleos -u https://test.telos.kitchen push action trailservice castvote '["haydenhypha1", "hypha1.....", ["pass"]]' -p haydenhypha1
cleos -u https://test.telos.kitchen push action dao.hypha closeprop '["payouts", 0]' -p haydenhypha1
cleos -u https://test.telos.kitchen push action dao.hypha setlastballt '["hypha1.....1f"]' -p dao.hypha


cleos -u https://test.telos.kitchen push action -sjd -x 86400 eosio.token transfer '["dao.hypha", "johnnyhypha1", "1.2345 TLOS", "testing native approval trx"]' -p dao.hypha > hypha_xfer_test.json
cleos -u https://test.telos.kitchen push action dao.hypha 
cleos -u https://api.eosnewyork.io multisig propose_trx unpause '[{"actor": "gftma.x", "permission": "active"}, {"actor": "danielflora3", "permission": "active"}]' ./gyftietokens_unpause.json gftma.x

cleos -u https://test.telos.kitchen push action token.hypha issue '["dao.hypha", "100 HYPHA", "memo"]' -p dao.hypha
cleos -u https://test.telos.kitchen push action token.hypha transfer '["dao.hypha", "haydenhypha1", "1 HYPHA", "memo"]' -p dao.hypha

## new format

cleos -u https://test.telos.kitchen push action dao.hypha mapit '{"key":"strings":"value":{"key":"tester":"value":"tester value"}}' -p dao.hypha

cleos -u https://test.telos.kitchen push action dao.hypha propose '{"proposer":"johnnyhypha1", 
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
cleos -u https://test.telos.kitchen push action dao.hypha setconfig '["token.hypha", "trailservice"]' -p dao.hypha

cleos -u https://test.telos.kitchen push action token.hypha create '["dao.hypha", "-1.00 HYPHA"]' -p token.hypha



cleos -u https://test.telos.kitchen push action token.hypha issue '["dao.hypha", "1.00 HYPHA", "memo"]' -p dao.hypha
cleos -u https://test.telos.kitchen push action token.hypha transfer '["dao.hypha", "johnnyhypha1", "1.00000000 PRESEED", "memo"]' -p dao.hypha

cleos -u https://test.telos.kitchen push action dao.hypha addmember '["johnnyhypha1"]' -p dao.hypha
cleos -u https://test.telos.kitchen push action dao.hypha addmember '["samanthahyph"]' -p dao.hypha
cleos -u https://test.telos.kitchen push action dao.hypha addmember '["jameshypha11"]' -p dao.hypha
cleos -u https://test.telos.kitchen push action dao.hypha addmember '["thomashypha1"]' -p dao.hypha
cleos -u https://test.telos.kitchen push action dao.hypha addmember '["haydenhypha1"]' -p dao.hypha


cleos -u https://test.telos.kitchen push action token.hypha issue '["dao.hypha", "1 HYPHA", "memo"]' -p dao.hypha
cleos -u https://test.telos.kitchen push action token.hypha transfer '["dao.hypha", "johnnyhypha1", "1 HYPHA", "memo"]' -p dao.hypha

# cleos -u https://test.telos.kitchen push action token.hypha issue '["samanthahyph", "1 HYPHA", "memo"]' -p dao.hypha
# cleos -u https://test.telos.kitchen push action token.hypha issue '["jameshypha11", "1 HYPHA", "memo"]' -p dao.hypha
# cleos -u https://test.telos.kitchen push action token.hypha issue '["thomashypha1", "1 HYPHA", "memo"]' -p dao.hypha
# cleos -u https://test.telos.kitchen push action token.hypha issue '["haydenhypha1", "1 HYPHA", "memo"]' -p dao.hypha

cleos -u https://test.telos.kitchen push action eosio.trail regtoken '["1000000000000 HVOIC", "dao.hypha", "https://dao.hypha.earth"]' -p dao.hypha
cleos -u https://test.telos.kitchen push action eosio.trail issuetoken '["dao.hypha", "johnnyhypha1", "1 HVOIC", false]' -p dao.hypha

# cleos -u https://test.telos.kitchen push action eosio.trail issuetoken '["dao.hypha", "hyphamember2", "1 HVOICE", false]' -p dao.hypha
# cleos -u https://test.telos.kitchen push action eosio.trail issuetoken '["dao.hypha", "hyphamember3", "1 HVOICE", false]' -p dao.hypha
# cleos -u https://test.telos.kitchen push action eosio.trail issuetoken '["dao.hypha", "hyphamember4", "1 HVOICE", false]' -p dao.hypha
# cleos -u https://test.telos.kitchen push action eosio.trail issuetoken '["dao.hypha", "hyphamember5", "1 HVOICE", false]' -p dao.hypha


cleos -u https://test.telos.kitchen push action dao.hypha proposerole '["johnnyhypha1", "Underwater Basketweaver", "Weave baskets at the bottom of the sea", "We make *great* baskets.", "11 HYPHA", "11.00000000 PRESEED", "11 HVOICE", 0, 10]' -p johnnyhypha1
cleos -u https://test.telos.kitchen push action dao.hypha propassign '["johnnyhypha1", "johnnyhypha1", 0, "https://joinseeds.com", "I am a professional basket maker and scuba diver", 0, 6, 1.000000000]' -p johnnyhypha1

cleos -u https://test.telos.kitchen push action eosio.trail castvote '["johnnyhypha1", 1, 1]' -p johnnyhypha1

# cleos -u https://test.telos.kitchen push action eosio.trail castvote '["hyphamember2", 66, 1]' -p hyphamember2
# cleos -u https://test.telos.kitchen push action eosio.trail castvote '["hyphamember3", 66, 1]' -p hyphamember3
# cleos -u https://test.telos.kitchen push action eosio.trail castvote '["hyphamember4", 60, 0]' -p hyphamember4

cleos -u https://test.telos.kitchen push action dao.hypha closeprop '[0]' -p hyphamember1

cleos -u https://test.telos.kitchen push action dao.hypha payassign '[0, 0]' -p hyphamember3
cleos -u https://test.telos.kitchen push action dao.hypha resetperiods '[]' -p dao.hypha
cleos -u https://test.telos.kitchen push action dao.hypha assign '[2]' -p dao.hypha











doesnt work: 3fbdca985be8751bcfd3917bdbae80ea9e33770a
does work: 


62b2e3dd38533cd6f3ced50c3f53c76f40e98651







# Trail experimentation

cleos -u https://test.telos.kitchen get table dao.hypha dao.hypha applicants


cleos -u https://test.telos.kitchen get table dao.hypha dao.hypha config
cleos -u https://test.telos.kitchen get table dao.hypha dao.hypha nominees
cleos -u https://test.telos.kitchen get table dao.hypha dao.hypha boardmembers

cleos -u https://test.telos.kitchen get table dao.hypha dao.hypha proposals
cleos -u https://test.telos.kitchen get table dao.hypha dao.hypha roles
cleos -u https://test.telos.kitchen get table dao.hypha dao.hypha assignments
cleos -u https://test.telos.kitchen get table dao.hypha dao.hypha roleprops

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

cleos -u https://test.telos.kitchen get table token.hypha hyphamember2 accounts

cleos -u https://test.telos.kitchen push action hyphadaobal1 makepayout '[2]' -p hyphadaobal1
cleos -u https://test.telos.kitchen push action hyphadaobal1 reset '[]' -p hyphadaobal1





cleos -u https://test.telos.kitchen push action hyphadaobali setconfig '["token.hypha", "token.hypha"]' -p hyphadaobali


cleos -u https://test.telos.kitchen push action eosio updateauth '{
    "account": "dao.hypha",
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
                    "actor": "dao.hypha",
                    "permission": "eosio.code"
                },
                "weight": 1
            }
        ],
        "waits": []
    }
}' -p dao.hypha@owner

cleos -u https://test.telos.kitchen push action eosio updateauth '{
    "account": "dao.hypha",
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
                    "actor": "dao.hypha",
                    "permission": "eosio.code"
                },
                "weight": 1
            }
        ],
        "waits": []
    }
}' -p dao.hypha@owner




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
token.hypha
