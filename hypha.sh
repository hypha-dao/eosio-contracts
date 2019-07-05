
cleos create account eosio hyphadao EOS6vvAofsMC5RJyY6fRHcyiQLNjDGukX6tRUoF1WEc63idQ3BqJn EOS6vvAofsMC5RJyY6fRHcyiQLNjDGukX6tRUoF1WEc63idQ3BqJn
cleos create account eosio member1 EOS6vvAofsMC5RJyY6fRHcyiQLNjDGukX6tRUoF1WEc63idQ3BqJn EOS6vvAofsMC5RJyY6fRHcyiQLNjDGukX6tRUoF1WEc63idQ3BqJn
cleos create account eosio member2 EOS6vvAofsMC5RJyY6fRHcyiQLNjDGukX6tRUoF1WEc63idQ3BqJn EOS6vvAofsMC5RJyY6fRHcyiQLNjDGukX6tRUoF1WEc63idQ3BqJn
cleos create account eosio member3 EOS6vvAofsMC5RJyY6fRHcyiQLNjDGukX6tRUoF1WEc63idQ3BqJn EOS6vvAofsMC5RJyY6fRHcyiQLNjDGukX6tRUoF1WEc63idQ3BqJn
cleos create account eosio token EOS6vvAofsMC5RJyY6fRHcyiQLNjDGukX6tRUoF1WEc63idQ3BqJn EOS6vvAofsMC5RJyY6fRHcyiQLNjDGukX6tRUoF1WEc63idQ3BqJn

source perm.json

cleos set contract hyphadao hyphadao/hyphadao
cleos set contract token hyphadao/eosiotoken

cleos push action hyphadao proposerole '["member1", "blockdev", "https://joinseeds.com", "develop cool shit", "10 HYPHA", "500 PRESEED", "10 HVOICE"]' -p member1
cleos push action hyphadao closeprop '["member1", 0]' -p member1 

cleos push action hyphadao propassign '["member1", "member1", "blockdev", "https://joinseeds.com", "I cant wait to start this assignment", 0, 1.00000000]' -p member1
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


