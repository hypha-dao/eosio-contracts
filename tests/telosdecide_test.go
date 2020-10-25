package local_test

import (
	"context"
	"testing"

	eostest "github.com/digital-scarcity/eos-go-test"
	"github.com/eoscanada/eos-go"
	"github.com/stretchr/testify/assert"
)

// const devHome = "/Users/max/dev"
const tdHome = devHome + "/telosnetwork/telos-decide"
const tdWasm = tdHome + "/build/contracts/decide/decide.wasm"
const tdAbi = tdHome + "/build/contracts/decide/decide.abi"

type appVersion struct {
	AppVersion string
}

func initTD(ctx context.Context, api *eos.API, telosDecide eos.AccountName) (string, error) {
	actions := []*eos.Action{
		{
			Account: telosDecide,
			Name:    toActionName("init", "init action name on Telos Decide"),
			Authorization: []eos.PermissionLevel{
				{Actor: telosDecide, Permission: eos.PN("active")},
			},
			ActionData: eos.NewActionData(appVersion{
				AppVersion: "vtest",
			}),
		}}
	return eostest.ExecTrx(ctx, api, actions)
}

type fee struct {
	FeeName   eos.Name
	FeeAmount eos.Asset
}

type tDTreasury struct {
	Manager   eos.AccountName
	MaxSupply eos.Asset
	Access    eos.Name
}

func newTreasury(ctx context.Context, api *eos.API, telosDecide, treasuryManager *eos.AccountName) (string, error) {
	maxSupply, _ := eos.NewAssetFromString("1000000000.00 HVOICE")
	actions := []*eos.Action{
		{
			Account: *telosDecide,
			Name:    toActionName("newtreasury", "creating new treasury within Telos Decide"),
			Authorization: []eos.PermissionLevel{
				{Actor: *treasuryManager, Permission: eos.PN("active")},
			},
			ActionData: eos.NewActionData(tDTreasury{
				Manager:   *treasuryManager,
				MaxSupply: maxSupply,
				Access:    eos.Name("public"),
			}),
		}}
	return eostest.ExecTrx(ctx, api, actions)
}

type transferP struct {
	From     eos.AccountName
	To       eos.AccountName
	Quantity eos.Asset
	Memo     string
}

func Transfer(ctx context.Context, api *eos.API, token, from, to *eos.AccountName, amount eos.Asset, memo string) (string, error) {

	actions := []*eos.Action{
		{
			Account: *token,
			Name:    toActionName("transfer", "transfer quantity of token"),
			Authorization: []eos.PermissionLevel{
				{Actor: *from, Permission: eos.PN("active")},
			},
			ActionData: eos.NewActionData(transferP{
				From:     *from,
				To:       *to,
				Quantity: amount,
				Memo:     memo,
			}),
		}}
	return eostest.ExecTrx(ctx, api, actions)
}

type issuance struct {
	To       eos.AccountName
	Quantity eos.Asset
	Memo     string
}

func issue(ctx context.Context, api *eos.API, token, issuer *eos.AccountName, amount eos.Asset) (string, error) {

	actions := []*eos.Action{
		{
			Account: *token,
			Name:    toActionName("issue", "issue quantity of token"),
			Authorization: []eos.PermissionLevel{
				{Actor: *issuer, Permission: eos.PN("active")},
			},
			ActionData: eos.NewActionData(issuance{
				To:       *issuer,
				Quantity: amount,
				Memo:     "memo",
			}),
		}}
	return eostest.ExecTrx(ctx, api, actions)
}

func mint(ctx context.Context, api *eos.API, telosDecide, issuer, receiver *eos.AccountName, amount eos.Asset) (string, error) {

	actions := []*eos.Action{
		{
			Account: *telosDecide,
			Name:    toActionName("mint", "mint quantity of token"),
			Authorization: []eos.PermissionLevel{
				{Actor: *issuer, Permission: eos.PN("active")},
			},
			ActionData: eos.NewActionData(issuance{
				To:       *receiver,
				Quantity: amount,
				Memo:     "memo",
			}),
		}}
	return eostest.ExecTrx(ctx, api, actions)
}

// type regvoterP struct {
// 	Voter    eos.AccountName
// 	Treasury eos.Symbol
// 	Referrer eos.AccountName
// }

func Regvoter(ctx context.Context, api *eos.API, telosDecide, registrant *eos.AccountName) (string, error) {

	hvoice, _ := eos.NewAssetFromString("1.00 HVOICE")

	actionData := make(map[string]interface{})
	actionData["voter"] = registrant
	actionData["treasury_symbol"] = hvoice.Symbol.String()
	actionData["referrer"] = registrant

	actionBinary, err := api.ABIJSONToBin(ctx, *telosDecide, eos.Name("regvoter"), actionData)
	if err != nil {
		return "abi error", err
	}

	actions := []*eos.Action{
		{
			Account: *telosDecide,
			Name:    eos.ActN("regvoter"),
			Authorization: []eos.PermissionLevel{
				{Actor: *registrant, Permission: eos.PN("active")},
			},
			ActionData: eos.NewActionDataFromHexData([]byte(actionBinary)),
		}}

	// actions := []*eos.Action{
	// 	{
	// 		Account: *telosDecide,
	// 		Name:    toActionName("regvoter", "regvoter"),
	// 		Authorization: []eos.PermissionLevel{
	// 			{Actor: *registrant, Permission: eos.PN("active")},
	// 		},
	// 		ActionData: eos.NewActionData(regvoterP{
	// 			Voter:    *registrant,
	// 			Treasury: "2,HVOICE",
	// 			Referrer: "null",
	// 		}),
	// 	}}
	return eostest.ExecTrx(ctx, api, actions)
}

func setFee(ctx context.Context, api *eos.API, telosDecide eos.AccountName) (string, error) {
	zeroFee, _ := eos.NewAssetFromString("0.0000 TLOS")
	feeNames := []eos.Name{eos.Name("ballot"), eos.Name("treasury"), eos.Name("archival"), eos.Name("committee")}
	var fees []*eos.Action
	for _, feeName := range feeNames {
		fee := eos.Action{
			Account: telosDecide,
			Name:    toActionName("updatefee", "td update fee action"),
			Authorization: []eos.PermissionLevel{
				{Actor: telosDecide, Permission: eos.PN("active")},
			},
			ActionData: eos.NewActionData(fee{
				FeeName:   feeName,
				FeeAmount: zeroFee,
			}),
		}
		fees = append(fees, &fee)
	}
	return eostest.ExecTrx(ctx, api, fees)
}

func SetupTelosDecide(t *testing.T, env *Environment) (string, error) {

	// setup TLOS system contract
	// tlosToken := eos.AN("eosio.token")
	tlosToken, err := eostest.CreateAccountFromString(env.ctx, &env.api, "eosio.token")
	assert.NoError(t, err)

	tlosMaxSupply, _ := eos.NewAssetFromString("1000000000.0000 TLOS")
	_, err = eostest.DeployAndCreateToken(env.ctx, &env.api, tokenHome, &tlosToken, &env.DAO, &tlosMaxSupply)
	assert.NoError(t, err)

	_, err = issue(env.ctx, &env.api, &tlosToken, &env.DAO, tlosMaxSupply)
	assert.NoError(t, err)

	// deploy TD contract
	_, err = eostest.SetContract(env.ctx, &env.api, &env.TelosDecide, tdWasm, tdAbi)
	assert.NoError(t, err)

	// call init action
	_, err = initTD(env.ctx, &env.api, env.TelosDecide)
	assert.NoError(t, err)

	// set fee
	_, err = setFee(env.ctx, &env.api, env.TelosDecide)
	assert.NoError(t, err)

	// transfer
	_, err = Transfer(env.ctx, &env.api, &tlosToken, &env.DAO, &env.TelosDecide, tlosMaxSupply, "deposit")
	assert.NoError(t, err)

	_, err = newTreasury(env.ctx, &env.api, &env.TelosDecide, &env.DAO)
	assert.NoError(t, err)

	_, err = Regvoter(env.ctx, &env.api, &env.TelosDecide, &env.Whale)
	assert.NoError(t, err)

	mintedTokens, _ := eos.NewAssetFromString("100.00 HVOICE")
	_, err = mint(env.ctx, &env.api, &env.TelosDecide, &env.DAO, &env.Whale, mintedTokens)
	assert.NoError(t, err)

	_, err = Regvoter(env.ctx, &env.api, &env.TelosDecide, &env.DAO)
	assert.NoError(t, err)

	daoTokens, _ := eos.NewAssetFromString("1.00 HVOICE")
	_, err = mint(env.ctx, &env.api, &env.TelosDecide, &env.DAO, &env.DAO, daoTokens)
	assert.NoError(t, err)

	return "", nil
}
