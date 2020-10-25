package dao_test

import (
	"context"
	"testing"
	"time"

	"github.com/alexeyco/simpletable"
	eostest "github.com/digital-scarcity/eos-go-test"
	"github.com/eoscanada/eos-go"
	"github.com/hypha-dao/dao-go"
	"github.com/stretchr/testify/assert"
)

const defaultKey = "5KQwrPbwdL6PhXujxW37FSSQZ1JiwsST4cqQzDeyXtP79zkvFD3"

const devHome = "/Users/max/dev"

const daoHome = devHome + "/hypha/eosio-contracts"
const daoWasm = daoHome + "/hyphadao/hyphadao.wasm"
const daoAbi = daoHome + "/hyphadao/hyphadao.abi"

const tokenHome = devHome + "/token"
const tokenWasm = tokenHome + "/token/token.wasm"
const tokenAbi = tokenHome + "/token/token.abi"

const tdHome = devHome + "/telosnetwork/telos-decide"
const tdWasm = tdHome + "/build/contracts/decide/decide.wasm"
const tdAbi = tdHome + "/build/contracts/decide/decide.abi"

const treasuryHome = devHome + "/hypha/treasury-contracts"
const treasuryWasm = treasuryHome + "/treasury/treasury.wasm"
const treasuryAbi = treasuryHome + "/treasury/treasury.abi"

const monitorHome = devHome + "/hypha/monitor"
const monitorWasm = monitorHome + "/monitor/monitor.wasm"
const monitorAbi = monitorHome + "/monitor/monitor.abi"

const seedsHome = devHome + "/hypha/seeds-contracts"
const escrowWasm = seedsHome + "/artifacts/escrow.wasm"
const escrowAbi = seedsHome + "/artifacts/escrow.abi"
const exchangeWasm = seedsHome + "/artifacts/exchange.wasm"
const exchangeAbi = seedsHome + "/artifacts/exchange.abi"

const testingEndpoint = "http://localhost:8888"

type Environment struct {
	ctx context.Context
	api eos.API

	DAO           eos.AccountName
	HusdToken     eos.AccountName
	HyphaToken    eos.AccountName
	HvoiceToken   eos.AccountName
	SeedsToken    eos.AccountName
	Bank          eos.AccountName
	SeedsEscrow   eos.AccountName
	SeedsExchange eos.AccountName
	Events        eos.AccountName
	TelosDecide   eos.AccountName
	Whale         eos.AccountName

	Members []eos.AccountName
}

func envHeader() *simpletable.Header {
	return &simpletable.Header{
		Cells: []*simpletable.Cell{
			{Align: simpletable.AlignCenter, Text: "Variable"},
			{Align: simpletable.AlignCenter, Text: "Value"},
		},
	}
}

func (e *Environment) String() string {
	table := simpletable.New()
	table.Header = envHeader()

	kvs := make(map[string]string)
	kvs["DAO"] = string(e.DAO)
	kvs["HUSD Token"] = string(e.HusdToken)
	kvs["HVOICE Token"] = string(e.HvoiceToken)
	kvs["HYPHA Token"] = string(e.HyphaToken)
	kvs["SEEDS Token"] = string(e.SeedsToken)
	kvs["Bank"] = string(e.Bank)
	kvs["Escrow"] = string(e.SeedsEscrow)
	kvs["Exchange"] = string(e.SeedsExchange)
	kvs["Telos Decide"] = string(e.TelosDecide)
	kvs["Whale"] = string(e.Whale)

	for key, value := range kvs {
		r := []*simpletable.Cell{
			{Align: simpletable.AlignLeft, Text: key},
			{Align: simpletable.AlignRight, Text: value},
		}
		table.Body.Cells = append(table.Body.Cells, r)
	}

	return table.String()
}

func SetupEnvironment(t *testing.T) *Environment {
	var env Environment

	env.api = *eos.New(testingEndpoint)
	// api.Debug = true
	env.ctx = context.Background()

	keyBag := &eos.KeyBag{}
	err := keyBag.ImportPrivateKey(env.ctx, defaultKey)
	assert.NoError(t, err)

	env.api.SetSigner(keyBag)

	env.DAO, _ = eostest.CreateAccountFromString(env.ctx, &env.api, "dao.hypha")
	env.Bank, _ = eostest.CreateAccountFromString(env.ctx, &env.api, "bank.hypha")
	env.HusdToken, _ = eostest.CreateAccountFromString(env.ctx, &env.api, "husd.hypha")
	env.HvoiceToken, _ = eostest.CreateAccountFromString(env.ctx, &env.api, "hvoice.hypha")
	env.HyphaToken, _ = eostest.CreateAccountFromString(env.ctx, &env.api, "token.hypha")
	env.Events, _ = eostest.CreateAccountFromString(env.ctx, &env.api, "publsh.hypha")

	env.SeedsToken, _ = eostest.CreateAccountFromString(env.ctx, &env.api, "seeds.token")
	env.SeedsEscrow, _ = eostest.CreateAccountFromString(env.ctx, &env.api, "escrow.seeds")
	env.SeedsExchange, _ = eostest.CreateAccountFromString(env.ctx, &env.api, "tlosto.seeds")

	env.TelosDecide, _ = eostest.CreateAccountFromString(env.ctx, &env.api, "telos.decide")
	env.Whale, _ = eostest.CreateAccountFromString(env.ctx, &env.api, "whale")

	env.Members = make([]eos.AccountName, 5)
	env.Members[0], _ = eostest.CreateAccountFromString(env.ctx, &env.api, "member1")
	env.Members[1], _ = eostest.CreateAccountFromString(env.ctx, &env.api, "member2")
	env.Members[2], _ = eostest.CreateAccountFromString(env.ctx, &env.api, "member3")
	env.Members[3], _ = eostest.CreateAccountFromString(env.ctx, &env.api, "member4")
	env.Members[4], _ = eostest.CreateAccountFromString(env.ctx, &env.api, "member5")

	_, err = eostest.SetContract(env.ctx, &env.api, &env.DAO, daoWasm, daoAbi)
	assert.NoError(t, err)

	_, err = eostest.SetContract(env.ctx, &env.api, &env.Bank, treasuryWasm, treasuryAbi)
	assert.NoError(t, err)

	_, err = eostest.SetContract(env.ctx, &env.api, &env.SeedsEscrow, escrowWasm, escrowAbi)
	assert.NoError(t, err)

	_, err = eostest.SetContract(env.ctx, &env.api, &env.SeedsExchange, exchangeWasm, exchangeAbi)
	assert.NoError(t, err)

	_, err = eostest.SetContract(env.ctx, &env.api, &env.Events, monitorWasm, monitorAbi)
	assert.NoError(t, err)

	_, err = dao.CreateRoot(env.ctx, &env.api, &env.DAO)
	assert.NoError(t, err)

	husdMaxSupply, _ := eos.NewAssetFromString("1000000000.00 HUSD")
	_, err = eostest.DeployAndCreateToken(env.ctx, &env.api, tokenHome, &env.HusdToken, &env.DAO, &husdMaxSupply)
	assert.NoError(t, err)

	hyphaMaxSupply, _ := eos.NewAssetFromString("1000000000.00 HYPHA")
	_, err = eostest.DeployAndCreateToken(env.ctx, &env.api, tokenHome, &env.HyphaToken, &env.DAO, &hyphaMaxSupply)
	assert.NoError(t, err)

	hvoiceMaxSupply, _ := eos.NewAssetFromString("1000000000.00 HVOICE")
	_, err = eostest.DeployAndCreateToken(env.ctx, &env.api, tokenHome, &env.HvoiceToken, &env.DAO, &hvoiceMaxSupply)
	assert.NoError(t, err)

	seedsMaxSupply, _ := eos.NewAssetFromString("1000000000.0000 SEEDS")
	_, err = eostest.DeployAndCreateToken(env.ctx, &env.api, tokenHome, &env.SeedsToken, &env.DAO, &seedsMaxSupply)
	assert.NoError(t, err)

	_, err = dao.Issue(env.ctx, &env.api, &env.SeedsToken, &env.DAO, seedsMaxSupply)
	assert.NoError(t, err)

	_, err = dao.SetIntConfigAtt(env.ctx, &env.api, &env.DAO, "voting_duration_sec", 60)
	assert.NoError(t, err)

	_, err = dao.SetIntConfigAtt(env.ctx, &env.api, &env.DAO, "seeds_deferral_factor_x100", 139)
	assert.NoError(t, err)

	_, err = dao.SetIntConfigAtt(env.ctx, &env.api, &env.DAO, "hypha_deferral_factor_x100", 50)
	assert.NoError(t, err)

	_, err = dao.SetIntConfigAtt(env.ctx, &env.api, &env.DAO, "paused", 0)
	assert.NoError(t, err)

	dao.SetNameConfigAtt(env.ctx, &env.api, &env.DAO, "hypha_token_contract", env.HyphaToken)
	dao.SetNameConfigAtt(env.ctx, &env.api, &env.DAO, "hvoice_token_contract", env.HvoiceToken)
	dao.SetNameConfigAtt(env.ctx, &env.api, &env.DAO, "husd_token_contract", env.HusdToken)
	dao.SetNameConfigAtt(env.ctx, &env.api, &env.DAO, "seeds_token_contract", env.SeedsToken)
	// TODO: deploy/configure escrow contract
	dao.SetNameConfigAtt(env.ctx, &env.api, &env.DAO, "seeds_escrow_contract", env.SeedsEscrow)
	dao.SetNameConfigAtt(env.ctx, &env.api, &env.DAO, "publisher_contract", env.Events)
	dao.SetNameConfigAtt(env.ctx, &env.api, &env.DAO, "telos_decide_contract", env.TelosDecide)

	fiveMins, _ := time.ParseDuration("5m")
	_, err = dao.AddPeriods(env.ctx, &env.api, env.DAO, 10, fiveMins)
	assert.NoError(t, err)

	// setup TLOS system contract
	tlosToken, err := eostest.CreateAccountFromString(env.ctx, &env.api, "eosio.token")
	assert.NoError(t, err)

	tlosMaxSupply, _ := eos.NewAssetFromString("1000000000.0000 TLOS")
	_, err = eostest.DeployAndCreateToken(env.ctx, &env.api, tokenHome, &tlosToken, &env.DAO, &tlosMaxSupply)
	assert.NoError(t, err)

	_, err = dao.Issue(env.ctx, &env.api, &tlosToken, &env.DAO, tlosMaxSupply)
	assert.NoError(t, err)

	// deploy TD contract
	_, err = eostest.SetContract(env.ctx, &env.api, &env.TelosDecide, tdWasm, tdAbi)
	assert.NoError(t, err)

	// call init action
	_, err = dao.InitTD(env.ctx, &env.api, env.TelosDecide)
	assert.NoError(t, err)

	// transfer
	_, err = dao.Transfer(env.ctx, &env.api, &tlosToken, &env.DAO, &env.TelosDecide, tlosMaxSupply, "deposit")
	assert.NoError(t, err)

	_, err = dao.NewTreasury(env.ctx, &env.api, &env.TelosDecide, &env.DAO)
	assert.NoError(t, err)

	_, err = dao.RegVoter(env.ctx, &env.api, &env.TelosDecide, &env.Whale)
	assert.NoError(t, err)

	mintedTokens, _ := eos.NewAssetFromString("100.00 HVOICE")
	_, err = dao.Mint(env.ctx, &env.api, &env.TelosDecide, &env.DAO, &env.Whale, mintedTokens)
	assert.NoError(t, err)

	_, err = dao.RegVoter(env.ctx, &env.api, &env.TelosDecide, &env.DAO)
	assert.NoError(t, err)

	daoTokens, _ := eos.NewAssetFromString("1.00 HVOICE")
	_, err = dao.Mint(env.ctx, &env.api, &env.TelosDecide, &env.DAO, &env.DAO, daoTokens)
	assert.NoError(t, err)

	for _, member := range env.Members {

		_, err = dao.RegVoter(env.ctx, &env.api, &env.TelosDecide, &member)
		assert.NoError(t, err)

		_, err = dao.Apply(env.ctx, &env.api, &env.DAO, member, "apply to DAO")
		assert.NoError(t, err)

		_, err = dao.Enroll(env.ctx, &env.api, &env.DAO, env.DAO, member)
		assert.NoError(t, err)
	}

	return &env
}
