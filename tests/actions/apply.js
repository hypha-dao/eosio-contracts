const { Api, JsonRpc } = require("eosjs");
const { JsSignatureProvider } = require("eosjs/dist/eosjs-jssig");
const fetch = require("node-fetch");
const { TextEncoder, TextDecoder } = require("util");

const defaultPrivateKey = process.env.PRIVATE_KEY;
const signatureProvider = new JsSignatureProvider([defaultPrivateKey]);

const rpc = new JsonRpc("https://test.telos.kitchen", { fetch });

const api = new Api({
  rpc,
  signatureProvider,
  textDecoder: new TextDecoder(),
  textEncoder: new TextEncoder()
});

const sendTrx = async () => {

  try {
    const actions = [
      {
        account: "hyphadaomain",
        name: "apply",
        authorization: [
          {
            actor: "hyphalondon2",
            permission: "active"
          }
        ],
        data: {
          applicant: "hyphalondon2",
          content: "I met with Debbie at the regen conference and we talked about Hypha. I would like to join."
        }
      },
      {
        account: "trailservice",
        name: "regvoter",
        authorization: [
          {
            actor: "hyphalondon2",
            permission: "active"
          }
        ],
        data: {
          voter: "hyphalondon2",
          treasury_symbol: "0,HVOICE",
          referrer: null
        }
      },
    ];

    const result = await api
      .transact(
        {
          actions: actions
        },
        {
          blocksBehind: 3,
          expireSeconds: 30
        }
      )

    console.log("Successfull : ", result);
  } catch (e) {
    console.error(e)
    console.log('Please, fix an error and run script again')
    process.exit(1);
  }
}

const main = async () => {
  await sendTrx()
}

main()
