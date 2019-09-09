const csv = require("fast-csv");
const fs = require("fs");

const { Api, JsonRpc, RpcError } = require("eosjs");
const { JsSignatureProvider } = require("eosjs/dist/eosjs-jssig"); // development only
const fetch = require("node-fetch"); // node only; not needed in browsers
const { TextEncoder, TextDecoder } = require("util");

const defaultPrivateKey = process.env.PRIVATE_KEY; //"<insert key here>"; // bob
const signatureProvider = new JsSignatureProvider([defaultPrivateKey]);

const rpc = new JsonRpc("https://test.telos.kitchen", { fetch });

const api = new Api({
  rpc,
  signatureProvider,
  textDecoder: new TextDecoder(),
  textEncoder: new TextEncoder()
});

// const resetPeriods = () => {
//   const actions = [{
//     account: "hyphadaotst1",
//     name: "reset",
//     authorization: [
//       {
//         actor: "hyphadaotst1",
//         permission: "active"
//       }
//     ],
//     data: {}
//   }]

//   return api
//     .transact(
//       {
//         actions: actions
//       },
//       {
//         blocksBehind: 3,
//         expireSeconds: 30
//       }
//     )
//     .then(function(result) {
//       console.log('Flush database: ' + result.transaction_id);
//     })
//     .catch(function(e) {
//       console.log(e);
//     })
// }

const addPeriodsSync = async (periods) => {
  for (let i = 0; i < periods.length; i++) {
    try {
      const { startdate, enddate, phaseatstart } = periods[i]

      const actions = [
        {
          account: "hyphadaomain",
          name: "addperiod",
          authorization: [
            {
              actor: "hyphadaomain",
              permission: "active"
            }
          ],
          data: {
            start_time: startdate,
            end_time: enddate,
            phase: phaseatstart
          }
        }
      ]

      await api
        .transact(
          {
            actions: actions
          },
          {
            blocksBehind: 3,
            expireSeconds: 30
          }
        )

      console.log("Successfully created phase: ", startdate)
    } catch (e) {
      console.error(e)
      console.log('Please, fix an error and run script again')
    }
  }
}

const loadPeriods = () => {
  let periods = []

  const handler = csv.parse({ headers: true })

  handler.on('data', row => periods.push(row))

  handler.on('end', () => addPeriodsSync(periods))

  fs.createReadStream("../moon_phases.csv").pipe(handler)
}

const main = async () => {
  //await resetPeriods()

  await loadPeriods()
}

main()
