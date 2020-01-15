/*

This script parses a proposal, proposes it, and optionally votes to approve it and close it.

Here's an example of it being used.

➜ node dao.js -f proposals/role.json -a -c

Parsing the proposal from :  ex_proposals/role.json
-- title            :  Underwater Basketweaver
-- proposal_type    :  roles
-- proposer         :  johnnyhypha1

Submitting proposal :  ex_proposals/role.json
Transaction Successfull :  c2321c973a94b1f496d1592777ebcbd00d9ab9ac686220ba34dfe4846f27a434

Please wait :  10000  ms ...  Eliminating likelihood of microfork before retrieving proposal...

Approving the proposal
-- calling trailservice::castvote with the following parms:
-- -- voter       :  johnnyhypha1
-- -- ballot_id   :  hypha1.....3i
-- -- options     :  [ 'pass' ]
Transaction Successfull :  7071dd166dac2a3ba23a56f7125bedbfc5c8837777d34f1c59a2855a9f19e8d0

Closing the proposal (after the wait)
-- calling dao.hypha::closeprop with the following parms:
-- -- proposal_type :  roles
-- -- proposal_id   :  0

Please wait :  75000  ms ...  Waiting while the ballot expiration expires...

Transaction Successfull :  c3bd796543edbf281c3474892441cc077a691c8840aa4e7b7068afefd45b2e3a

++++++++++++++++++++++++++++++++++++++++++++++++++++++

To propose the role, assignment, and payout, vote for all proposals, and close all proposals, run the following:

git clone git@gitlab.com:hypha/hypha-dao-smart-contracts.git # OR this: https://gitlab.com/hypha/hypha-dao-smart-contracts
cd hypha-dao-smart-contracts/tests
git checkout migration
yarn
export PRIVATE_KEY=5HwnoWBuuRmNdcqwBzd1LABFRKnTk2RY2kUMYKkZfF8tKodubtK 
node dao.js -f proposals/role.json -a -c && node dao.js -f proposals/assignment.json -a -c && node dao.js -f proposals/payout.json -a -c

++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
*/

const commandLineArgs = require("command-line-args");
const fs = require('fs');

const { Api, JsonRpc } = require("eosjs");
const { JsSignatureProvider } = require("eosjs/dist/eosjs-jssig");
const fetch = require("node-fetch");
const { TextEncoder, TextDecoder } = require("util");

const defaultPrivateKey = process.env.PRIVATE_KEY;
const DAO_CONTRACT = "dao.hypha";
const signatureProvider = new JsSignatureProvider([defaultPrivateKey]);

async function sendtrx (host, contract, action, authorizer, data) {
  const rpc = new JsonRpc(host, { fetch });
  const api = new Api({rpc, signatureProvider, textDecoder: new TextDecoder(), textEncoder: new TextEncoder()});
  const actions = [{account: contract,name: action,authorization: [{actor: authorizer, permission: "active"}],data: data}];
  const result = await api.transact({actions: actions}, {blocksBehind: 3, expireSeconds: 30});
  console.log("Transaction Successfull : ", result.transaction_id);
}

async function getVotingPeriod (host) {
  let rpc;
  let options = {};

  rpc = new JsonRpc(host, { fetch });
  options.code = DAO_CONTRACT;
  options.scope = DAO_CONTRACT; 

  options.json = true;
  options.table = "config";

  const result = await rpc.get_table_rows(options);
  if (result.rows.length > 0) {
    return result.rows[0].ints.find(o => o.key === 'voting_duration_sec').value;
  } else {
    console.log ("ERROR:: Configuration has not been set.");
  }
}

async function getLastCreatedProposal (host) {
  let rpc;
  let options = {};

  rpc = new JsonRpc(host, { fetch });
  options.code = DAO_CONTRACT;
  options.scope = DAO_CONTRACT; 

  options.json = true;
  options.table = "proposals";
  options.index_position = 2; // index #2 is "bycreated"
  options.key_type = 'i64';
  options.reverse = true;
  options.limit = 1;
  
  const result = await rpc.get_table_rows(options);
  if (result.rows.length > 0) {
    return result.rows[0];
  } 
  
  console.log ("There are no proposals of type: ", proposalType);
  return undefined;  
}

class FileDetails {
  constructor (filename) {
    this.filename = filename
    this.exists = fs.existsSync(filename)
  }
}

async function loadOptions() {
  const optionDefinitions = [
    { name: "file", alias: "f", type: filename => new FileDetails(filename)  },
    { name: "host", alias: "h", type: String, defaultValue: "https://test.telos.kitchen" },
    { name: "approve", alias: "a", type: Boolean, defaultValue: false },
    { name: "close", alias: "c", type: Boolean, defaultValue: false },
    { name: "propose", alias: "p", type: Boolean, defaultValue: true },
    { name: "config", type: Boolean, defaultValue: false }
    // see here to add new options:
    //   - https://github.com/75lb/command-line-args/blob/master/doc/option-definition.md
  ];

  return commandLineArgs(optionDefinitions);
}

function sleep(ms, msg){
  console.log ( "\nPlease wait : ", ms, " ms ... ", msg, "\n");
  return new Promise(resolve=>{
    setTimeout(resolve,ms)
  })
}

const main = async () => {
  const opts = await loadOptions();

  // setting configuration
  if (opts.file && opts.config) {
    const config = JSON.parse(fs.readFileSync(opts.file.filename, 'utf8'));
    console.log ("\nParsing the configuration from : ", opts.file.filename);
    console.log ("-- hypha_token_contract   : ", config.data.names.find(o => o.key === 'hypha_token_contract').value);
    console.log ("-- seeds_token_contract   : ", config.data.names.find(o => o.key === 'seeds_token_contract').value);
    console.log ("-- telos_decide_contract  : ", config.data.names.find(o => o.key === 'telos_decide_contract').value);

    console.log ("\nSubmitting configuration : ", opts.file.filename);
    await sendtrx(opts.host, DAO_CONTRACT, "setconfig", DAO_CONTRACT, config.data);
  }

  // proposing
  else if (opts.file && opts.propose) {

    const proposal = JSON.parse(fs.readFileSync(opts.file.filename, 'utf8'));
    console.log ("\nParsing the proposal from : ", opts.file.filename);
    console.log ("-- title            : ", proposal.data.strings.find(o => o.key === 'title').value);
    console.log ("-- proposal_type    : ", proposal.data.names.find(o => o.key === 'proposal_type').value);
    console.log ("-- proposer         : ", proposal.data.names.find(o => o.key === 'proposer').value);

    console.log ("\nSubmitting proposal : ", opts.file.filename);
    await sendtrx(opts.host, DAO_CONTRACT, "propose", 
      proposal.data.names.find(o => o.key === 'proposer').value, 
      proposal.data);

    if (opts.approve || opts.close) {

      // sleep to ensure the table is written
      await sleep (10000, "Eliminating likelihood of microfork before retrieving proposal...");
      const lastProposal = await getLastCreatedProposal(opts.host);

      if (opts.approve) {

        // vote for the proposal
        var options = [];
        options.push ("pass");
  
        console.log ("\Approving the proposal");
        console.log ("-- calling trailservice::castvote with the following parms:");
        console.log ("-- -- voter       : ", proposal.data.names.find(o => o.key === 'proposer').value);
        console.log ("-- -- ballot_id   : ", lastProposal.names.find(o => o.key === 'ballot_id').value);
        console.log ("-- -- options     : ", options);
  
        await sendtrx(opts.host, "trailservice", "castvote", 
          proposal.data.names.find(o => o.key === 'proposer').value, 
          { "voter":proposal.data.names.find(o => o.key === 'proposer').value, 
            "ballot_name":lastProposal.names.find(o => o.key === 'ballot_id').value, 
            "options":options });
      }
  
      if (opts.close) {
        
        console.log ("\nClosing the proposal (after the wait)");
        console.log ("-- calling dao.hypha::closeprop with the following parms:");
        console.log ("-- -- proposal_id   : ", lastProposal.id);

        // sleep another 1 hour to ensure the ballot open window is complete
        const sleepDuration = (await getVotingPeriod(opts.host) * 1000) + 20000;
        await sleep (sleepDuration, "Waiting " + sleepDuration + " seconds while the ballot expiration expires...");
   
        // close the proposal
        await sendtrx(opts.host, DAO_CONTRACT, "closeprop", 
          proposal.data.names.find(o => o.key === 'proposer').value, 
          { "proposal_id":lastProposal.id });
      }
    }
  } else {
    console.log ("You must use the -f for the json file.");
  }  
}

main()
