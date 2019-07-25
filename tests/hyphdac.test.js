/* eslint-disable prettier/prettier */
const eoslime = require("../../eoslime").init("local");
const assert = require('assert');

const HYPHADAO_WASM = "../hyphadac/hyphadao/hyphadao/hyphadao.wasm";
const HYPHADAO_ABI = "../hyphadac/hyphadao/hyphadao/hyphadao.abi";
const EOSIOTOKEN_WASM = "../hyphadac/hyphadao/eosiotoken/eosiotoken.wasm";
const EOSIOTOKEN_ABI = "../hyphadac/hyphadao/eosiotoken/eosiotoken.abi";
const TRAIL_WASM = "../telos.contracts/eosio.trail/eosio.trail.wasm";
const TRAIL_ABI = "../telos.contracts/eosio.trail/eosio.trail.abi";
// const BOARD_WASM = "../hyphadac/hyphadao/telos.tfvt/telos.tfvt.wasm";
// const BOARD_ABI = "../hyphadac/hyphadao/telos.tfvt/telos.tfvt.abi";

describe("HyphaDAC Testing", function() {
  this.timeout(150000);

  let hyphadaoContract, eosTokenContract, trailContract, hyphaBoardContract;
  let hyphadaoAccount, eosTokenAccount, trailAccount, hyphaBoardAccount;
  let member1, member2, member3, member4, member5;
  let accounts;

  before(async () => {
    accounts = await eoslime.Account.createRandoms(10);
    hyphadaoAccount = accounts[0];
    eosTokenAccount = accounts[1];
    member1 = accounts[2];
    member2 = accounts[3];
    member3 = accounts[4];
    member4 = accounts[5];
    member5 = accounts[6];
    trailAccount = accounts[7];
    hyphaBoardAccount = accounts[8];

    console.log(" Hypha DAC Account     : ", hyphadaoAccount.name);
    console.log(" Token Account         : ", eosTokenAccount.name);
    console.log(" Hypha Board Contract  : ", hyphaBoardAccount.name);
    console.log(" Trail Service         : ", trailAccount.name);
    console.log(" Member 1              : ", member1.name);
    console.log(" Member 2              : ", member2.name);
    console.log(" Member 3              : ", member3.name);
    console.log(" Member 4              : ", member4.name);
    console.log(" Member 5              : ", member5.name);

    await hyphadaoAccount.addPermission(
      hyphadaoAccount.name,
      "active",
      hyphadaoAccount.name,
      "eosio.code"
    );
    // await hyphaBoardAccount.addPermission(
    //   hyphaBoardAccount.name,
    //   "active",
    //   hyphaBoardAccount.name,
    //   "eosio.code"
    // );
    await trailAccount.addPermission(
      trailAccount.name,
      "active",
      trailAccount.name,
      "eosio.code"
    );

    hyphadaoContract = await eoslime.AccountDeployer.deploy(
      HYPHADAO_WASM,
      HYPHADAO_ABI,
      hyphadaoAccount
    );
    eosTokenContract = await eoslime.AccountDeployer.deploy(
      EOSIOTOKEN_WASM,
      EOSIOTOKEN_ABI,
      eosTokenAccount
    );
    // hyphaBoardContract = await eoslime.AccountDeployer.deploy(
    //   BOARD_WASM,
    //   BOARD_ABI,
    //   hyphaBoardAccount
    // );
    trailContract = await eoslime.AccountDeployer.deploy(
      TRAIL_WASM,
      TRAIL_ABI,
      trailAccount
    );

    console.log(" Set config on hypha dao")
    await hyphadaoContract.setconfig(eosTokenAccount.name, trailAccount.name, {from: hyphadaoAccount });
    console.log("initialize hypha dao")
    await hyphadaoContract.init({ from: hyphadaoAccount });
    // await hyphaBoardContract.setconfig(member1.name, { from: hyphaBoardAccount});
    // await hyphaBoardContract.inittfvt("https://joinseeds.com", { from: hyphaBoardAccount })
    // await hyphaBoardContract.inittfboard("https://joinseeds.com", { from: hyphaBoardAccount })

    console.log ("issugin tokens to members");
    await trailContract.issuetoken(hyphadaoContract.name, member1.name, "1 HVOICE", 0, { from: hyphadaoAccount })
    await trailContract.issuetoken(hyphadaoContract.name, member2.name, "1 HVOICE", 0, { from: hyphadaoAccount })
    await trailContract.issuetoken(hyphadaoContract.name, member3.name, "1 HVOICE", 0, { from: hyphadaoAccount })
    await trailContract.issuetoken(hyphadaoContract.name, member4.name, "1 HVOICE", 0, { from: hyphadaoAccount })
    await trailContract.issuetoken(hyphadaoContract.name, member5.name, "1 HVOICE", 0, { from: hyphadaoAccount })
      console.log ("comleted issuance")

    console.log (" Creating HYPHA and PRESEED tokens...")
    await eosTokenContract.create(hyphadaoAccount.name, "100000000 HYPHA");
    // await eosTokenContract.create(hyphadaoAccount.name, "100000000 HVOICE");
    await eosTokenContract.create(hyphadaoAccount.name, "100000000 PRESEED");
  });

  it("Should create a new role", async () => {

    // await hyphadaoContract.nominate(member1.name, member1.name, { from: member1 })
    // await hyphadaoContract.makeelection(member1.name, "string", { from: member1 })
    // await hyphadaoContract.addcand(member1.name, "string", {from: member1 })
    
    await hyphadaoContract.proposerole(
      member1.name,
      "blockchdev",
      "https://joinseeds.com",
      "Blockchain developer",
      "10 HYPHA",
      "10.00000000 PRESEED",
      "10 HVOICE", { from: member1 }
    );
    await hyphadaoContract.proposerole(
      member1.name,
      "websitedev",
      "https://joinseeds.com",
      "Website developer/maintainer",
      "8 HYPHA",
      "10.00000000 PRESEED",
      "8 HVOICE", { from: member1 }
    );

    const roleprops = await hyphadaoContract.provider.eos.getTableRows({
      code: hyphadaoAccount.name,
      scope: hyphadaoAccount.name,
      table: "roleprops",
      json: true
    });
    assert.equal(roleprops.rows.length, 2);
    console.log(roleprops);
  });

  it("Should propose to assign a user to a role", async () => {
    await hyphadaoContract.propassign(member1.name, member1.name, 
        0, "https://joinseeds.com", "Description", 1, 0.500000000000, { from: member1 });
    await hyphadaoContract.propassign(member2.name, member2.name, 
      1, "https://joinseeds.com", "Description", 2, 1.000000000000, { from: member2 });

    const assprops = await hyphadaoContract.provider.eos.getTableRows({
      code: hyphadaoAccount.name,
      scope: hyphadaoAccount.name,
      table: "assprops",
      json: true
    });
    assert.equal(assprops.rows.length, 2);
    console.log(assprops);
  });

  it("Should create a contribution", async () => {
    await hyphadaoContract.proppayout(
      member5.name,
      member5.name,
      "Purchased conference fees for Rise event",
      "https://joinseeds.com",
      "965 HYPHA",
      "965.00000000 PRESEED",
      "45 HVOICE",
      1000, { from: member5 }
    );

    const payoutprops = await hyphadaoContract.provider.eos.getTableRows({
      code: hyphadaoAccount.name,
      scope: hyphadaoAccount.name,
      table: "payoutprops",
      json: true
    });
    assert.equal(payoutprops.rows.length, 1);
    console.log(payoutprops);
  });

  it ("Should vote to approve role", async() => {
    await trailContract.castvote(member1.name, 0, 1, { from: member1 });
    await trailContract.castvote(member2.name, 0, 1, { from: member2 });
    await trailContract.castvote(member3.name, 0, 1, { from: member3 });

    await hyphadaoContract.closeprop (member1.name, 0, { from: member1 });

    const roles = await hyphadaoContract.provider.eos.getTableRows({
      code: hyphadaoAccount.name,
      scope: hyphadaoAccount.name,
      table: "roles",
      json: true
    });
    assert.equal(roles.rows.length, 2);
    console.log(roles);

  });
});
