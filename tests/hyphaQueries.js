const { JsonRpc } = require("eosjs");
const fetch = require("node-fetch"); 
var moment = require('moment');
moment().utc().format();

async function printLastCreatedProposal (proposalType) {
  let rpc;
  let options = {};

  rpc = new JsonRpc("https://test.telos.kitchen", { fetch });
  options.code = "hyphadaomain";
  options.scope = proposalType; 

  options.json = true;
  options.table = "proposals";
  options.index_position = 2; // index #2 is "bycreated"
  options.key_type = 'i64';
  options.reverse = true;
  options.limit = 1;
  
  rpc.get_table_rows(options).then( result => {
    if (result.rows.length > 0) {
      console.log (JSON.stringify(result.rows, null, 2));
    } else {
      console.log ("There are no proposals of type: ", proposalType);
    }
  })
}

async function getCurrentPeriod () {
  let rpc;
  let options = {};
  const recordsPerQuery = 50;

  rpc = new JsonRpc("https://test.telos.kitchen", { fetch });
  options.code = "hyphadaomain";
  options.scope = "hyphadaomain";

  options.json = true;
  options.limit = recordsPerQuery;
  options.table = "periods";
  options.lower_bound = 0;
  options.upper_bound = recordsPerQuery;

  let result = await rpc.get_table_rows(options);

  while (result.more) {
    
    if (result.rows.length > 0) {

      for await (const period of result.rows) {
        const start_date = moment(period.start_date); //.add(5, 'hours');
        const end_date = moment(period.end_date); //.add(5, 'hours');

        // check if NOW is within the current period
        if (moment().add(5, 'hours').isAfter(start_date) && moment().add(5, 'hours').isBefore(end_date)) {
          console.log ("Now: ", moment().toString(), "is after: ", start_date.toString(), "... and before: ", end_date.toString());
          return period.period_id;
        }
      }
      
      options.lower_bound = options.upper_bound;
      options.upper_bound += recordsPerQuery;
      result = await rpc.get_table_rows(options);
    }
  }

  console.log ("There is no period that matches the current time.");
  return -2;
}

async function printByProposer (proposalType, proposer) {
  let rpc;
  let options = {};

  rpc = new JsonRpc("https://test.telos.kitchen", { fetch });
  options.code = "hyphadaomain";
  options.scope = proposalType;

  options.json = true;
  options.table = "proposals";
  options.index_position = 4; // index #5 is "byproposer"
  options.key_type = 'i64';
  options.lower_bound = proposer;
  options.upper_bound = proposer;

  rpc.get_table_rows(options).then( result => {
    if (result.rows.length > 0) {
      console.log (JSON.stringify(result.rows, null, 2));
    } else {
      console.log ("There are no proposals for that type and proposer");
    }
  })
}

async function main () {
  try {

    console.log ('Current Period: ')
    console.log (await getCurrentPeriod());
    // Print the last created role proposal
    // await printLastCreatedProposal ("roles");

    // // Print the role proposals proposed by this account
    // await printByProposer ("roles", "johnnyhypha1");

  } catch (e) {
    console.log (e);
  }
}

main ();

