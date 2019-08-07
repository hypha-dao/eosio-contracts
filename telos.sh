
hyphadaomain
hyphadaotokn

cleos -u https://telos.caleos.io push action hyphadaomain setconfig '["hyphadaotokn", "eosio.trail"]' -p hyphadaomain

cleos -u https://telos.caleos.io push action hyphadaomain init '[]' -p hyphadaomain

cleos -u https://telos.caleos.io push action hyphadaotokn create '["hyphadaomain", "-1 HYPHA"]' -p hyphadaotokn
cleos -u https://telos.caleos.io push action hyphadaotokn create '["hyphadaomain", "-1.00000000 PRESEED"]' -p hyphadaotokn


cleos -u https://telos.caleos.io push action hyphadaotokn issue '["hyphadaomain", "1.00000000 PRESEED", "memo"]' -p hyphadaomain

cleos -u https://telos.caleos.io push action eosio.trail issuetoken '["hyphadaomain", "hyphanewyork", "1 HVOICE", false]' -p hyphadaomain
cleos -u https://telos.caleos.io push action hyphadaotokn issue '["hyphanewyork", "1 HYPHA", "Welcome to Hypha!"]' -p hyphadaomain


cleos -u https://telos.caleos.io set contract hyphadaomain hyphdao/hyphadao