const assert = require ('assert');
const eoslime = require ('../../eoslime').init('local');

const HYPHADAC_WASM = '../hyphadac/hyphadac/hyphadac/hyphadac.wasm' 
const HYPHADAC_ABI = '../hyphadac/hyphadac/hyphadac/hyphadac.abi'
const EOSIOTOKEN_WASM = '../hyphadac/hyphadac/eosiotoken/eosiotoken.wasm'
const EOSIOTOKEN_ABI = '../hyphadac/hyphadac/eosiotoken/eosiotoken.abi'

describe ('HyphaDAC Testing', function () {

    this.timeout (150000)

    let hyphadacContract, eosTokenContract;
    let hyphadacAccount, eosTokenAccount;
    let member1, member2, member3, member4, member5;
    let accounts;

    before(async () => {
        accounts = await eoslime.Account.createRandoms(10);
        hyphadacAccount = accounts[0];
        eosTokenAccount = accounts[1];
        member1         = accounts[2]
        member2         = accounts[3]
        member3         = accounts[4]
        member4         = accounts[5]
        member5         = accounts[6]

        console.log(' Hypha DAC Account     : ', hyphadacAccount.name)
        console.log(' Token Account         : ', eosTokenAccount.name)
        console.log(' Member 1              : ', member1.name)
        console.log(' Member 2              : ', member2.name)
        console.log(' Member 3              : ', member3.name)
        console.log(' Member 4              : ', member4.name)
        console.log(' Member 5              : ', member5.name)

        await hyphadacAccount.addPermission (hyphadacAccount.name, 'active', hyphadacAccount.name, 'eosio.code')
        
        hyphadacContract = await eoslime.AccountDeployer.deploy (HYPHADAC_WASM, HYPHADAC_ABI, hyphadacAccount)
        eosTokenContract = await eoslime.AccountDeployer.deploy (EOSIOTOKEN_WASM, EOSIOTOKEN_ABI, eosTokenAccount)

        await hyphadacContract.setconfig (10, eosTokenAccount.name, { from: hyphadacAccount})

        await eosTokenContract.create (hyphadacAccount.name, '100000000 HYPHA')
        await eosTokenContract.create (hyphadacAccount.name, '100000000 HVOICE')
        await eosTokenContract.create (hyphadacAccount.name, '100000000 PRESEED')
    })
    
    it('Should create a new role', async () => {
        await hyphadacContract.newrole ( 'blockchdev', 'Blockchain developer', '10 HYPHA', '10 PRESEED', '10 HVOICE')
        await hyphadacContract.newrole ( 'websitedev', 'Website developer/maintainer', '8 HYPHA', '10 PRESEED', '8 VOICE')
        
        const roles = await hyphadacContract.provider.eos.getTableRows({
            code: hyphadacAccount.name,
            scope: hyphadacAccount.name,
            table: 'roles',
            json: true
        });
        assert.equal (roles.rows.length, 2);
        console.log(roles)
    })

    it('Should assign a user to a role', async () => {
        await hyphadacContract.assign (member1.name, 'blockchdev', 1000, 0.25)
        await hyphadacContract.assign (member2.name, 'websitedev', 100, 1.0)

        const assignments = await hyphadacContract.provider.eos.getTableRows({
            code: hyphadacAccount.name,
            scope: hyphadacAccount.name,
            table: 'assignments',
            json: true
        })
        assert.equal (assignments.rows.length, 2)
        console.log(assignments)
    })
    
    it('Should create a contribution', async () => {
        await hyphadacContract.contribute ( member5.name, 'Purchased conference fees for Rise event',
            '965 HYPHA', '965 PRESEED', 1000)

        const contributions = await hyphadacContract.provider.eos.getTableRows({
            code: hyphadacAccount.name,
            scope: hyphadacAccount.name,
            table: 'contribs',
            json: true
        })
        assert.equal (contributions.rows.length, 1)
        console.log(contributions)
    })

})