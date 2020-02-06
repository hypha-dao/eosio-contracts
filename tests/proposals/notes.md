
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






 4
  title: July Audit - Genesis Tracking Contributions - Voice Fix
description: Correcting the previous request for voice. Mapping over the new rules for 2X for voice.
content: Correcting the previous request for voice. Mapping over the new rules for 2X for voice. 


2
 title: July Audit - Genesis Contribution Tracking
description: Claiming contributions to Hypha and SEEDS from August 2017 to July 2019
content: For about 2 years I spent most of every day dreaming, writing, constructing and serving this monumentally intimidating vision of facilitating the creation of a Regenerative Financial System. I always knew in my heart that we would get to this point- although my brain certainly had its moments of doubt and depression. I’m so incredibly excited, honored and in love that it has come this far! I’m not looking for a disproportionate holding of tokens. My biggest reward is that this will exist. I also desire that the amazing souls who have been dedicating their lives to this vision get a good share of this movement. For this reason I’ve heavily reduced the claimed value of my time during this nearly 2 year period to 1/4 of what they would have been if using the same value tracking after this period. Thank you all so much for sharing this dream with me. So excited to see where this goes and what we can accomplish together! 