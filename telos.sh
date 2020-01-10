
dao.hypha
token.hypha


# create HYPHA token
cleos -u https://api.telos.kitchen push action token.hypha create '["dao.hypha", "-1.00 HYPHA"]' -p token.hypha
cleos -u https://api.telos.kitchen push action token.hypha issue '["hyphanewyork", "1.00 HYPHA", "Welcome to Hypha!"]' -p dao.hypha

# set config
cleos -u https://api.telos.kitchen push action dao.hypha setconfig '["token.hypha", "trailservice"]' -p dao.hypha

# fund the trailservice
cleos -u https://api.telos.kitchen push action eosio.token transfer '["dappbuilders", "dao.hypha", "1000.0000 TLOS", "for HVOICE setup"]' -p teloskitchen
cleos -u https://api.telos.kitchen push action eosio.token transfer '["dao.hypha", "trailservice", "1000.0000 TLOS", "deposit"]' -p dao.hypha

# create new treasury
cleos -u https://api.telos.kitchen push action trailservice newtreasury '["dao.hypha", "1000000000 HVOICE", "public"]' -p dao.hypha
cleos -u https://api.telos.kitchen push action trailservice toggle '["0,HVOICE", "transferable"]' -p dao.hypha

# register to vote
cleos -u https://api.telos.kitchen push action trailservice regvoter '["hyphanewyork", "0,HVOICE", null]' -p hyphanewyork
cleos -u https://api.telos.kitchen push action trailservice mint '["dao.hypha", "1 HVOICE", "original mint"]' -p dao.hypha

# apply to the DAO
cleos -u https://api.telos.kitchen push action dao.hypha apply '["hyphanewyork", "I would like to join please"]' -p hyphanewyork

# propose a 1 HYPHA payout to test
