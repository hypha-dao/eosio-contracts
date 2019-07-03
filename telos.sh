Private key: 5J1gYLAc4GUo7EXNAXyaZTgo3m3SxtxDygdVUsNL4Par5Swfy1q
Public key: EOS6vvAofsMC5RJyY6fRHcyiQLNjDGukX6tRUoF1WEc63idQ3BqJn

hyphadactest
hyphatoken11
hyphaboardtk
hyphamember1
hyphamember2
hyphamember3
hyphamember4


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


cleos -u https://testnet.telos.caleos.io push action hyphaboardtk setconfig '["hyphamember1"]' -p hyphaboardtk


cleos -u https://testnet.telos.caleos.io get table eosio.trail eosio.trail registries