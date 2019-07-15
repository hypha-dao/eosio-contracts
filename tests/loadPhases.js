const csv = require("fast-csv");
const fs = require("fs");

const { Api, JsonRpc, RpcError } = require("eosjs");
const { JsSignatureProvider } = require("eosjs/dist/eosjs-jssig"); // development only
const fetch = require("node-fetch"); // node only; not needed in browsers
const { TextEncoder, TextDecoder } = require("util");

const defaultPrivateKey = "5J1gYLAc4GUo7EXNAXyaZTgo3m3SxtxDygdVUsNL4Par5Swfy1q"; // bob
const signatureProvider = new JsSignatureProvider([defaultPrivateKey]);

const rpc = new JsonRpc("https://testnet.telos.caleos.io", { fetch });

const api = new Api({
  rpc,
  signatureProvider,
  textDecoder: new TextDecoder(),
  textEncoder: new TextEncoder()
});

fs.createReadStream("../moon_phases.csv").pipe(
  csv.parse({ headers: true }).on("data", row => {
    const actions = [
      {
        account: "hyphadaobal1",
        name: "addperiod",
        authorization: [
          {
            actor: "hyphadaobal1",
            permission: "active"
          }
        ],
        data: {
          start_time: row.startdate,
          end_time: row.enddate,
          phase: row.phaseatstart
        }
      }
    ];
    console.log ('Actions: ', actions)

    api
      .transact(
        {
          actions: actions
        },
        {
          blocksBehind: 3,
          expireSeconds: 30
        }
      )
      .then(function(result) {
        console.log("Successfully created phase: ", row.startdate);
      })
      .catch(function(e) {
        console.log(e);
      });
    // console.log(row);
  })
);
