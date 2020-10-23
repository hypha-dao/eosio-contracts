package test

import (
	"context"
	"encoding/json"
	"fmt"
	"io/ioutil"
	"log"
	"time"

	"github.com/eoscanada/eos-go"
	"github.com/eoscanada/eos-go/ecc"
	"github.com/eoscanada/eosc/cli"
)

const defaultKey = "5KQwrPbwdL6PhXujxW37FSSQZ1JiwsST4cqQzDeyXtP79zkvFD3"

const devHome = "/Users/max/dev"

const daoHome = devHome + "/hypha/eosio-contracts"
const daoWasm = daoHome + "/hyphadao/hyphadao.wasm"
const daoAbi = daoHome + "/hyphadao/hyphadao.abi"

const tokenHome = devHome + "/token/token"
const tokenWasm = tokenHome + "/token.wasm"
const tokenAbi = tokenHome + "/token.abi"

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

var testingKey ecc.PublicKey

func toName(in, field string) eos.Name {
	name, err := cli.ToName(in)
	if err != nil {
		ErrorCheck(fmt.Sprintf("invalid name format for %q", field), err)
	}

	return name
}

func toActionName(in, field string) eos.ActionName {
	return eos.ActionName(toName(in, field))
}

func setConfigAtt(ctx context.Context, api *eos.API, contract *eos.AccountName, configAtt string, flexValue *FlexValue) (string, error) {

	action := toActionName("setconfigatt", "action")

	actionData := make(map[string]interface{})
	actionData["key"] = configAtt
	actionData["value"] = flexValue

	actionBinary, err := api.ABIJSONToBin(ctx, *contract, eos.Name(action), actionData)
	ErrorCheck("unable to retrieve action binary from JSON via API", err)

	actions := []*eos.Action{
		{
			Account: *contract,
			Name:    action,
			Authorization: []eos.PermissionLevel{
				{Actor: *contract, Permission: eos.PN("active")},
			},
			ActionData: eos.NewActionDataFromHexData([]byte(actionBinary)),
		}}

	log.Println("Setting config	: ", configAtt, " to ", flexValue.Impl)
	return ExecTrx(ctx, api, actions)
}

func setNameConfig(ctx context.Context, api *eos.API, contract *eos.AccountName, label string, value eos.AccountName) {
	_, err := setConfigAtt(ctx, api, contract, label, &FlexValue{
		BaseVariant: eos.BaseVariant{
			TypeID: FlexValueVariant.TypeID("name"),
			Impl:   value,
		},
	})
	if err != nil {
		log.Panicf("cannot set config: %s", err)
	}
}

func setConfig(ctx context.Context, api *eos.API, contract *eos.AccountName, configFile string) (string, error) {
	data, err := ioutil.ReadFile(configFile)
	if err != nil {
		log.Panicf("cannot read configuration: %s", err)
		return "error", err
	}

	action := toActionName("setconfig", "action")

	var dump map[string]interface{}
	err = json.Unmarshal(data, &dump)
	if err != nil {
		log.Panicf("cannot unmarshal configuration: %s", err)
		return "error", err
	}
	actionBinary, err := api.ABIJSONToBin(ctx, *contract, eos.Name(action), dump)
	ErrorCheck("unable to retrieve action binary from JSON via API", err)

	actions := []*eos.Action{
		{
			Account: *contract,
			Name:    action,
			Authorization: []eos.PermissionLevel{
				{Actor: *contract, Permission: eos.PN("active")},
			},
			ActionData: eos.NewActionDataFromHexData([]byte(actionBinary)),
		}}

	return ExecTrx(ctx, api, actions)
}

type appVersion struct {
	AppVersion string
}

type addPeriod struct {
	StartTime eos.TimePoint `json:"start_time"`
	EndTime   eos.TimePoint `json:"end_time"`
	Phase     string        `json:"phase"`
}

type applyParm struct {
	Applicant eos.AccountName
	Notes     string
}

func apply(ctx context.Context, api *eos.API, contract *eos.AccountName, applicant eos.AccountName) {
	actions := []*eos.Action{
		{
			Account: *contract,
			Name:    toActionName("apply", "apply to DAO"),
			Authorization: []eos.PermissionLevel{
				{Actor: applicant, Permission: eos.PN("active")},
			},
			ActionData: eos.NewActionData(applyParm{
				Applicant: applicant,
				Notes:     string("apply to dao"),
			}),
		}}
	ExecTrx(ctx, api, actions)
}

func addPeriods(ctx context.Context, api *eos.API, daoContract eos.AccountName, numPeriods int, periodDuration time.Duration) (string, error) {

	now := time.Now()

	startTime := eos.TimePoint(now.UnixNano() / 1000)
	endTime := eos.TimePoint(now.Add(periodDuration).UnixNano() / 1000)

	var periods []*eos.Action

	for i := 0; i < numPeriods; i++ {
		addPeriodAction := eos.Action{
			Account: daoContract,
			Name:    toActionName("addperiod", "add period action name"),
			Authorization: []eos.PermissionLevel{
				{Actor: daoContract, Permission: eos.PN("active")},
			},
			ActionData: eos.NewActionData(addPeriod{
				StartTime: startTime,
				EndTime:   endTime,
				Phase:     "test phase",
			}),
		}
		periods = append(periods, &addPeriodAction)
	}

	return ExecTrx(ctx, api, periods)
}

type tokenCreate struct {
	Issuer    eos.AccountName
	MaxSupply eos.Asset
}

func Setup() {
	api := eos.New(testingEndpoint)
	// api.Debug = true
	ctx := context.Background()

	keyBag := &eos.KeyBag{}
	err := keyBag.ImportPrivateKey(ctx, defaultKey)
	if err != nil {
		log.Panicf("cannot import default private key: %s", err)
	}
	api.SetSigner(keyBag)

	accounts, err := CreateRandoms(ctx, api, 20)
	if err != nil {
		log.Panicf("cannot create random accounts: %s", err)
	}

	daoContract := accounts[0]
	husdTokenContract := accounts[1]
	hyphaTokenContract := accounts[2]
	hvoiceTokenContract := accounts[3]
	seedsTokenContract := accounts[4]

	bankContract := accounts[5]
	seedsEscrowContract := accounts[6]
	seedsExchangeContract := accounts[7]

	member1 := accounts[8]
	member2 := accounts[9]

	eventContract := accounts[10]

	trxID, err := SetContract(ctx, api, daoContract, daoWasm, daoAbi)
	if err != nil {
		log.Panicf("cannot set contract: %s", err)
	}
	log.Println("Set DAO contract	: ", *daoContract, " : ", trxID)

	trxID, err = SetContract(ctx, api, bankContract, treasuryWasm, treasuryAbi)
	if err != nil {
		log.Panicf("cannot set contract: %s", err)
	}
	log.Println("Set Bank contract	: ", *bankContract, " : ", trxID)

	trxID, err = SetContract(ctx, api, seedsEscrowContract, escrowWasm, escrowAbi)
	if err != nil {
		log.Panicf("cannot set contract: %s", err)
	}
	log.Println("Set SEEDS Escrow contract	: ", *seedsEscrowContract, " : ", trxID)

	trxID, err = SetContract(ctx, api, seedsExchangeContract, exchangeWasm, exchangeAbi)
	if err != nil {
		log.Panicf("cannot set contract: %s", err)
	}
	log.Println("Set SEEDS Exchange contract	: ", *seedsExchangeContract, " : ", trxID)

	trxID, err = SetContract(ctx, api, eventContract, monitorWasm, monitorAbi)
	if err != nil {
		log.Panicf("cannot set contract: %s", err)
	}
	log.Println("Set Event Monitor contract	: ", *eventContract, " : ", trxID)

	husdMaxSupply, _ := eos.NewAssetFromString("1000000000.00 HUSD")
	deployAndCreateToken(ctx, api, husdTokenContract, daoContract, &husdMaxSupply)

	hyphaMaxSupply, _ := eos.NewAssetFromString("1000000000.00 HYPHA")
	deployAndCreateToken(ctx, api, hyphaTokenContract, daoContract, &hyphaMaxSupply)

	hvoiceMaxSupply, _ := eos.NewAssetFromString("1000000000.00 HVOICE")
	deployAndCreateToken(ctx, api, hvoiceTokenContract, daoContract, &hvoiceMaxSupply)

	seedsMaxSupply, _ := eos.NewAssetFromString("1000000000.0000 SEEDS")
	deployAndCreateToken(ctx, api, seedsTokenContract, daoContract, &seedsMaxSupply)

	trxID, err = setConfigAtt(ctx, api, daoContract, "voting_duration_sec", &FlexValue{
		BaseVariant: eos.BaseVariant{
			TypeID: FlexValueVariant.TypeID("uint64"),
			Impl:   uint64(300),
		},
	})
	if err != nil {
		log.Panicf("cannot set config: %s", err)
	}

	trxID, err = setConfigAtt(ctx, api, daoContract, "seeds_deferral_factor_x100", &FlexValue{
		BaseVariant: eos.BaseVariant{
			TypeID: FlexValueVariant.TypeID("uint64"),
			Impl:   uint64(130),
		},
	})
	if err != nil {
		log.Panicf("cannot set config: %s", err)
	}

	trxID, err = setConfigAtt(ctx, api, daoContract, "hypha_deferral_factor_x100", &FlexValue{
		BaseVariant: eos.BaseVariant{
			TypeID: FlexValueVariant.TypeID("uint64"),
			Impl:   uint64(50),
		},
	})
	if err != nil {
		log.Panicf("cannot set config: %s", err)
	}

	trxID, err = setConfigAtt(ctx, api, daoContract, "paused", &FlexValue{
		BaseVariant: eos.BaseVariant{
			TypeID: FlexValueVariant.TypeID("uint64"),
			Impl:   uint64(0),
		},
	})
	if err != nil {
		log.Panicf("cannot set config: %s", err)
	}

	setNameConfig(ctx, api, daoContract, "hypha_token_contract", *hyphaTokenContract)
	setNameConfig(ctx, api, daoContract, "hvoice_token_contract", *hvoiceTokenContract)
	setNameConfig(ctx, api, daoContract, "husd_token_contract", *husdTokenContract)
	setNameConfig(ctx, api, daoContract, "seeds_token_contract", *seedsTokenContract)
	// TODO: deploy/configure escrow contract
	setNameConfig(ctx, api, daoContract, "seeds_escrow_contract", *seedsEscrowContract)
	setNameConfig(ctx, api, daoContract, "publisher_contract", *eventContract)

	fiveMins, _ := time.ParseDuration("5m")
	trxID, err = addPeriods(ctx, api, *daoContract, 10, fiveMins)
	if err != nil {
		log.Panicf("cannot add periods: %s", err)
	}
	log.Println("Added periods to DAO contract: ", trxID)

	apply(ctx, api, daoContract, *member1)
	apply(ctx, api, daoContract, *member2)

}
