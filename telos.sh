
dao.hypha
token.hypha


# create HYPHA token
cleos -u https://api.telos.kitchen push action token.hypha create '["dao.hypha", "-1.00 HYPHA"]' -p token.hypha
cleos -u https://api.telos.kitchen push action token.hypha issue '["hyphanewyork", "1.00 HYPHA", "Welcome to Hypha!"]' -p dao.hypha

# set config
# cleos -u https://api.telos.kitchen push action dao.hypha setconfig '["token.hypha", "trailservice"]' -p dao.hypha

# fund the trailservice
cleos -u https://api.telos.kitchen push action eosio.token transfer '["dappbuilders", "dao.hypha", "1000.0000 TLOS", "for HVOICE setup"]' -p teloskitchen
cleos -u https://api.telos.kitchen push action eosio.token transfer '["dao.hypha", "trailservice", "1000.0000 TLOS", "deposit"]' -p dao.hypha

# create new treasury
cleos -u https://api.telos.kitchen push action trailservice newtreasury '["dao.hypha", "1000000000.00 HVOICE", "public"]' -p dao.hypha
# cleos -u https://api.telos.kitchen push action trailservice toggle '["0,HVOICE", "transferable"]' -p dao.hypha

# register to vote
cleos -u https://api.telos.kitchen push action trailservice regvoter '["dao.hypha", "2,HVOICE", null]' -p dao.hypha
cleos -u https://api.telos.kitchen push action trailservice mint '["hyphanewyork", "5.00 HVOICE", "mint"]' -p dao.hypha

# apply to the DAO
cleos -u https://api.telos.kitchen push action dao.hypha apply '["hyphanewyork", "The Times 03/Jan/2009 Chancellor on the brink of second bailout for banks"]' -p hyphanewyork

# propose a 1 HYPHA payout to test







export HOST=https://test.telos.kitchen 

cleos -u $HOST set contract dao.hypha hyphadao/
cleos -u $HOST push action dao.hypha resetperiods '[]' -p dao.hypha
cleos -u $HOST push action dao.hypha resetbankcfg '[]' -p dao.hypha

# update table structures
- add coeffecients to time periods
- remove bankconfig table
- add config table structure to bank class

# set config
node dao.js -f proposals/config.json -h $HOST --config
node loadPhases.js


# add inline actions to pay SEEDS to escrow
- transfer
- create # what is vesting period of each payment