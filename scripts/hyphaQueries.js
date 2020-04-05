const { JsonRpc } = require("eosjs");
const fetch = require("node-fetch"); 
var moment = require('moment');
moment().utc().format();

async function printLastCreatedProposal () {
  let rpc;
  let options = {};

  rpc = new JsonRpc("https://test.telos.kitchen", { fetch });
  options.code = "hyphadaomain";
  options.scope = "hyphadaomain"; 

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
      console.log ("There are no proposals");
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
        if (moment().isAfter(start_date) && moment().isBefore(end_date)) {

          console.log ("Current Period Found.  Period ID: ", period.period_id);
          console.log ("  Now               : ", moment().toString());
          console.log ("  Period Start      : ", start_date.toString());
          console.log ("  Period End        : ", end_date.toString());
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

async function getTable (table, scope, index, lower_bound, upper_bound) {
  let rpc;
  let options = {};

  rpc = new JsonRpc("https://test.telos.kitchen", { fetch });
  options.code = "hyphadaomain";
  options.scope = scope;

  options.json = true;
  options.table = table;
  options.index_position = index; // index #5 is "byproposer"
  options.key_type = 'i64';
  options.lower_bound = lower_bound;
  options.upper_bound = upper_bound;
  options.limit       = 100;  // no more than 100 records

  return (await rpc.get_table_rows(options)).rows;
}

async function printOpenProposalsByProposerByType (proposalType, proposer) {
  let rpc;
  let options = {};

  rpc = new JsonRpc("https://test.telos.kitchen", { fetch });
  options.code = "hyphadaomain";
  options.scope = "hyphadaomain";

  options.json = true;
  options.table = "proposals";
  options.index_position = 5; // index #5 is "bytype
  options.key_type = 'i64';
  options.lower_bound = proposalType;
  options.upper_bound = proposalType;

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

    await getCurrentPeriod();

    // // Print the last created role proposal
    // await printLastCreatedProposal ();

    // // all OPEN proposals made from johnnyhypha1
    // console.log (await getTable ("proposals", "hyphadaomain", 4, "johnnyhypha1", "johnnyhypha1"));

    // // all CLOSED proposals made from haydenhypha1
    // console.log (await getTable ("proposals", "archive", 4, "haydenhypha1", "haydenhypha1"));

    // // all OPEN proposals of type "assignments"
    console.log (await getTable ("proposals", "hyphadaomain", 5, "assignments", "assignments"));

    // // my current active assignments
    // console.log (await getTable ("assignments", "hyphadaomain", 2, "johnnyhypha1", "johnnyhypha1"));

    // my current open applications
    // step 1 - get all user's open proposals
    // let openProposals = await getTable ("proposals", "hyphadaomain", 4, "johnnyhypha1", "johnnyhypha1");
    // console.log (typeof openProposals);

    // var values = function(x) {
    //   return Object.keys(x).map(function(k){return x[k]})
    // }

    // var openAssignmentProposals = openProposals.filter(function(proposal) {
    //   console.log (typeof proposal);
    //   console.log (Object.keys(proposal.names));
    //   // console.log (Object.entries(proposal.names));
    //   // console.log (values(proposal.names).indexOf('proposal_type'));
    //   // console.log (proposal.names);
    //   // const proposal_type = proposal.names.some(function (p) { return p.key == "proposal_type"; });
    //   // console.log (proposal_type);
    //   return true; // === "assignments";
    // });
    
    // console.log (openAssignmentProposals);


    // console.log (openProposals.rows.filter(proposal => proposal.names.proposal_type === "assignments"));



    
  } catch (e) {
    console.log (e);
  }
}

main ();

