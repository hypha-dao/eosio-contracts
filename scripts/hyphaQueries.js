const { JsonRpc } = require("eosjs");
const fetch = require("node-fetch"); 
var moment = require('moment');
moment().utc().format();

async function getTable (table, scope) { //, index, lower_bound, upper_bound) {
  let rpc;
  let options = {};

  rpc = new JsonRpc("https://api.telos.kitchen", { fetch });
  options.code = "dao.hypha";
  options.scope = scope;

  options.index_position = 4
  options.key_type = 'i64'
  options.lower_bound = 'hyphanewyork'
  options.upper_bound = 'hyphanewyork'
  options.json = true;
  options.table = table;
  options.limit       = 10;  // no more than 100 records

  return (await rpc.get_table_rows(options)).rows;
}

async function main () {
  try {
    // console.log (JSON.stringify(await getTable ("objects", "assignment"), null, "  ")) //, 5, "assignments", "assignments"));
    console.log (await getTable ("objects", "assignment")) //, 5, "assignments", "assignments"));
   
  } catch (e) {
    console.log (e);
  }
}

main ();

