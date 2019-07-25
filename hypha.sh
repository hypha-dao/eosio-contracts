
cleos create account eosio hyphadao EOS6vvAofsMC5RJyY6fRHcyiQLNjDGukX6tRUoF1WEc63idQ3BqJn EOS6vvAofsMC5RJyY6fRHcyiQLNjDGukX6tRUoF1WEc63idQ3BqJn
cleos create account eosio member1 EOS6vvAofsMC5RJyY6fRHcyiQLNjDGukX6tRUoF1WEc63idQ3BqJn EOS6vvAofsMC5RJyY6fRHcyiQLNjDGukX6tRUoF1WEc63idQ3BqJn
cleos create account eosio member2 EOS6vvAofsMC5RJyY6fRHcyiQLNjDGukX6tRUoF1WEc63idQ3BqJn EOS6vvAofsMC5RJyY6fRHcyiQLNjDGukX6tRUoF1WEc63idQ3BqJn
cleos create account eosio member3 EOS6vvAofsMC5RJyY6fRHcyiQLNjDGukX6tRUoF1WEc63idQ3BqJn EOS6vvAofsMC5RJyY6fRHcyiQLNjDGukX6tRUoF1WEc63idQ3BqJn
cleos create account eosio token EOS6vvAofsMC5RJyY6fRHcyiQLNjDGukX6tRUoF1WEc63idQ3BqJn EOS6vvAofsMC5RJyY6fRHcyiQLNjDGukX6tRUoF1WEc63idQ3BqJn
cleos create account eosio member4 EOS6vvAofsMC5RJyY6fRHcyiQLNjDGukX6tRUoF1WEc63idQ3BqJn EOS6vvAofsMC5RJyY6fRHcyiQLNjDGukX6tRUoF1WEc63idQ3BqJn
cleos create account eosio member5 EOS6vvAofsMC5RJyY6fRHcyiQLNjDGukX6tRUoF1WEc63idQ3BqJn EOS6vvAofsMC5RJyY6fRHcyiQLNjDGukX6tRUoF1WEc63idQ3BqJn
cleos create account eosio eosio.trail EOS6vvAofsMC5RJyY6fRHcyiQLNjDGukX6tRUoF1WEc63idQ3BqJn EOS6vvAofsMC5RJyY6fRHcyiQLNjDGukX6tRUoF1WEc63idQ3BqJn

source perm.json

cleos set contract hyphadao hyphadao/hyphadao
cleos set contract token hyphadao/eosiotoken
cleos set contract eosio.trail ../telos.contracts/eosio.trail

cleos push action hyphadao setconfig '["token", "eosio.trail"]' -p hyphadao
cleos push action hyphadao init '[]' -p hyphadao
cleos push action eosio.trail issuetoken '["hyphadao", "member1", "1 HVOICE", 0]' -p hyphadao
cleos push action eosio.trail issuetoken '["hyphadao", "member2", "1 HVOICE", 0]' -p hyphadao
cleos push action eosio.trail issuetoken '["hyphadao", "member3", "1 HVOICE", 0]' -p hyphadao
cleos push action eosio.trail issuetoken '["hyphadao", "member4", "1 HVOICE", 0]' -p hyphadao
cleos push action eosio.trail issuetoken '["hyphadao", "member5", "1 HVOICE", 0]' -p hyphadao

cleos push action token create '["hyphadao", "1000000000 HYPHA"]' -p token
cleos push action token create '["hyphadao", "1000000000.00000000 PRESEED"]' -p token


cleos push action hyphadao proposerole '["member1", "blockdev", "https://joinseeds.com", "develop cool shit", "10 HYPHA", "500.00000000 PRESEED", "10 HVOICE"]' -p member1
cleos push action hyphadao proposerole '["member2", "webdev", "https://joinseeds.com", "dev website", "14 HYPHA", "450.00000000 PRESEED", "10 HVOICE"]' -p member2
cleos push action eosio.trail castvote '["member1", 0, 1]' -p member1
cleos push action eosio.trail castvote '["member2", 0, 1]' -p member2
cleos push action eosio.trail castvote '["member3", 0, 1]' -p member3

cleos push action hyphadao closeprop '["member1", 0]' -p member1 

cleos push action hyphadao propassign '["member1", "member1", 0, "https://joinseeds.com", "testing assignment", 0, 1.00000000]' -p member1
cleos push action hyphadao propassign '["member2", "member2", 1, "https://joinseeds.com", "testing assignment", 0, 0.50000000]' -p member2

cleos push action hyphadao closeprop '["member1", 1]' -p member1 



cleos get table hyphadao hyphadao proposals
cleos push action hyphadao eraseprop '[0]' -p member1 

# proposals
cleos get table hyphadao hyphadao proposals
cleos get table hyphadao hyphadao payoutprops
cleos get table hyphadao hyphadao roleprops
cleos get table hyphadao hyphadao assprops

# holocracy
cleos get table hyphadao hyphadao roles
cleos get table hyphadao hyphadao assignments


