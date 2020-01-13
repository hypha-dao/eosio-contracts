
Hypha DAO Tester

Find and report quality, security, or User Experience (UX) shortcomings and bugs. Advise on UX.

Purpose
- Optimize Hypha DAO's User Experience via Quality Control 

Responsibilities may include:
- Managing the overall Quality Assurance activities
- Designing and Executing automated and manual testing procedures
- Documenting results and clear, reproducible steps 
- Posting results in DevOps (Gitlab) system
- Partner with developers on fixes
- Authoring user docs

> Here's a video of one explanation of a tester role: https://www.youtube.com/watch?v=t7bVVLAP0Mc



-------------


Moving 
- app-manifest.json
    - lists the domain, appmeta, whitelist of contracts

- app-metadata.json
    - name of the application, description, icons, chains
    - there is only one app-metadata.json file

- chain-manifests.json
    - points to the appmeta file (with the hash)


ACTION killtreas (const symbol& treasury) {
    require_auth (get_self());
    treasuries_table t_t (get_self(), get_self().value);
    auto t_itr = t_t.find (treasury.raw().code());
    t_t.erase (t_itr);
}

ACTION killvoter (const name& voter, const symbol& treasury)  {
    require_auth (get_self());
    votes_table v_t (get_self(), voter.value);
    auto v_itr = v_t.find (treasury.raw().code());
    v_t.erase (v_itr);
}

cleos -u https://test.telos.kitchen push action trailservice killvoter '["samanthahyph", "0,HVOICE"]' -p trailservice
cleos -u https://test.telos.kitchen push action trailservice killvoter '["johnnyhypha1", "0,HVOICE"]' -p trailservice
cleos -u https://test.telos.kitchen push action trailservice killvoter '["jameshypha11", "0,HVOICE"]' -p trailservice
cleos -u https://test.telos.kitchen push action trailservice killvoter '["thomashypha1", "0,HVOICE"]' -p trailservice
cleos -u https://test.telos.kitchen push action trailservice killvoter '["haydenhypha1", "0,HVOICE"]' -p trailservice
cleos -u https://test.telos.kitchen push action trailservice killvoter '["cehraphaim11", "0,HVOICE"]' -p trailservice
cleos -u https://test.telos.kitchen push action trailservice killvoter '["hyphadaomain", "0,HVOICE"]' -p trailservice

cleos -u https://test.telos.kitchen push action trailservice killtreas '["0,HVOICE"]' -p trailservice




johnnyhypha1
samanthahyph
jameshypha11
thomashypha1
haydenhypha1

46 HVOICE
cleos -u https://test.telos.kitchen get table -L HYPHA -l 1 trailservice trailservice treasuries

45 HVOICE
cleos -u https://test.telos.kitchen get table trailservice johnnyhypha1 voters
cleos -u https://test.telos.kitchen get table trailservice samanthahyph voters
cleos -u https://test.telos.kitchen get table trailservice jameshypha11 voters
cleos -u https://test.telos.kitchen get table trailservice thomashypha1 voters
cleos -u https://test.telos.kitchen get table trailservice haydenhypha1 voters
cleos -u https://test.telos.kitchen get table trailservice hyphadaomain voters
cleos -u https://test.telos.kitchen get table trailservice hyphalondon2 voters
cleos -u https://test.telos.kitchen get table trailservice cehraphaim11 voters


cleos -u https://test.telos.kitchen get table trailservice cehraphaim11 voters



docker create \
  --name=letsencrypt \
  --cap-add=NET_ADMIN \
  -e PUID=0 \
  -e PGID=0 \
  -e TZ=Europe/London \
  -e URL=digscar.com \
  -e SUBDOMAINS=docs, \
  -e VALIDATION=http \
  -e DNSPLUGIN=cloudflare `#optional` \
  -e DUCKDNSTOKEN=<token> `#optional` \
  -e EMAIL=<e-mail> `#optional` \
  -e DHLEVEL=2048 `#optional` \
  -e ONLY_SUBDOMAINS=false `#optional` \
  -e EXTRA_DOMAINS=<extradomains> `#optional` \
  -e STAGING=false `#optional` \
  -p 443:443 \
  -p 80:80 `#optional` \
  -v </path/to/appdata/config>:/config \
  --restart unless-stopped \
  linuxserver/letsencrypt
