package local_test

import (
	"context"
	"encoding/json"
	"io/ioutil"
	"log"
	"testing"
	"time"

	"github.com/alexeyco/simpletable"
	eostest "github.com/digital-scarcity/eos-go-test"
	"github.com/eoscanada/eos-go"
	"github.com/eoscanada/eos-go/ecc"
	"github.com/eoscanada/eosc/cli"
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
var T *testing.T

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

// configVariant may hold a name, int64, asset, string, or time_point
var configVariant = eos.NewVariantDefinition([]eos.VariantType{
	{Name: "name", Type: eos.Name("")},
	{Name: "uint64", Type: uint64(0)},
	{Name: "asset", Type: (*eos.Asset)(nil)}, // Syntax for pointer to a type, could be any struct
	{Name: "string", Type: ""},
	{Name: "time_point", Type: eos.TimePoint(0)},
	{Name: "checksum256", Type: eos.Checksum256([]byte("0"))},
})

type configValue struct {
	eos.BaseVariant
}

// MarshalJSON translates to []byte
func (fv *configValue) MarshalJSON() ([]byte, error) {
	return fv.BaseVariant.MarshalJSON(configVariant)
}

// UnmarshalJSON translates flexValueVariant
func (fv *configValue) UnmarshalJSON(data []byte) error {
	return fv.BaseVariant.UnmarshalJSON(data, configVariant)
}

// UnmarshalBinary ...
func (fv *configValue) UnmarshalBinary(decoder *eos.Decoder) error {
	return fv.BaseVariant.UnmarshalBinaryVariant(decoder, configVariant)
}

func toName(in, field string) eos.Name {
	name, err := cli.ToName(in)
	assert.NoError(T, err)
	return name
}

func toActionName(in, field string) eos.ActionName {
	return eos.ActionName(toName(in, field))
}

func setConfigAtt(ctx context.Context, api *eos.API, contract *eos.AccountName, configAtt string, flexValue *configValue) (string, error) {

	action := toActionName("setconfigatt", "action")

	actionData := make(map[string]interface{})
	actionData["key"] = configAtt
	actionData["value"] = flexValue

	actionBinary, err := api.ABIJSONToBin(ctx, *contract, eos.Name(action), actionData)
	assert.NoError(T, err)

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
	return eostest.ExecTrx(ctx, api, actions)
}

func setNameConfig(ctx context.Context, api *eos.API, contract *eos.AccountName, label string, value eos.AccountName) {
	_, err := setConfigAtt(ctx, api, contract, label, &configValue{
		BaseVariant: eos.BaseVariant{
			TypeID: configVariant.TypeID("name"),
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
	assert.NoError(T, err)

	actions := []*eos.Action{
		{
			Account: *contract,
			Name:    action,
			Authorization: []eos.PermissionLevel{
				{Actor: *contract, Permission: eos.PN("active")},
			},
			ActionData: eos.NewActionDataFromHexData([]byte(actionBinary)),
		}}

	return eostest.ExecTrx(ctx, api, actions)
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

func apply(ctx context.Context, api *eos.API, contract *eos.AccountName, applicant eos.AccountName) (string, error) {
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
	return eostest.ExecTrx(ctx, api, actions)
}

type enrollParm struct {
	Enroller  eos.AccountName
	Applicant eos.AccountName
	Content   string
}

func enroll(ctx context.Context, api *eos.API, contract *eos.AccountName, enroller, applicant eos.AccountName) (string, error) {
	actions := []*eos.Action{
		{
			Account: *contract,
			Name:    toActionName("enroll", "enroll to DAO"),
			Authorization: []eos.PermissionLevel{
				{Actor: enroller, Permission: eos.PN("active")},
			},
			ActionData: eos.NewActionData(enrollParm{
				Enroller:  enroller,
				Applicant: applicant,
				Content:   string("enroll in dao"),
			}),
		}}
	return eostest.ExecTrx(ctx, api, actions)
}

func createRoot(ctx context.Context, api *eos.API, contract *eos.AccountName) (string, error) {
	actionData := make(map[string]interface{})
	actionData["notes"] = "notes"

	actionBinary, err := api.ABIJSONToBin(ctx, *contract, eos.Name("createroot"), actionData)
	if err != nil {
		return "abi error", err
	}

	actions := []*eos.Action{
		{
			Account: *contract,
			Name:    eos.ActN("createroot"),
			Authorization: []eos.PermissionLevel{
				{Actor: *contract, Permission: eos.PN("active")},
			},
			ActionData: eos.NewActionDataFromHexData([]byte(actionBinary)),
		}}
	return eostest.ExecTrx(ctx, api, actions)
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

	return eostest.ExecTrx(ctx, api, periods)
}

type tokenCreate struct {
	Issuer    eos.AccountName
	MaxSupply eos.Asset
}

func SetupEnvironment(t *testing.T) Environment {
	var env Environment
	T = t

	env.api = *eos.New(testingEndpoint)
	// api.Debug = true
	env.ctx = context.Background()

	keyBag := &eos.KeyBag{}
	err := keyBag.ImportPrivateKey(env.ctx, defaultKey)
	assert.NoError(t, err)

	env.api.SetSigner(keyBag)

	// accounts, err := eostest.CreateRandoms(env.ctx, &env.api, 15)
	// assert.NoError(t, err)
	// env.DAO = accounts[0]
	// env.HusdToken = accounts[1]
	// env.HvoiceToken = accounts[2]
	// env.HyphaToken = accounts[3]
	// env.SeedsToken = accounts[4]
	// env.Bank = accounts[5]
	// env.SeedsEscrow = accounts[6]
	// env.SeedsExchange = accounts[7]
	// env.Events = accounts[8]
	// env.TelosDecide = accounts[9]
	// env.Whale = accounts[10]

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

	// env.HvoiceToken = accounts[2]
	// env.HyphaToken = accounts[3]
	// env.SeedsToken = accounts[4]

	// env.Bank = accounts[5]
	// env.SeedsEscrow = accounts[6]
	// env.SeedsExchange = accounts[7]
	// env.Events = accounts[8]
	// env.TelosDecide = accounts[9]
	// env.Whale = accounts[10]

	// env.Members, err = eostest.CreateRandoms(env.ctx, &env.api, 5)
	// assert.NoError(T, err)

	_, err = eostest.SetContract(env.ctx, &env.api, &env.DAO, daoWasm, daoAbi)
	assert.NoError(T, err)

	_, err = eostest.SetContract(env.ctx, &env.api, &env.Bank, treasuryWasm, treasuryAbi)
	assert.NoError(T, err)

	_, err = eostest.SetContract(env.ctx, &env.api, &env.SeedsEscrow, escrowWasm, escrowAbi)
	assert.NoError(T, err)

	_, err = eostest.SetContract(env.ctx, &env.api, &env.SeedsExchange, exchangeWasm, exchangeAbi)
	assert.NoError(T, err)

	_, err = eostest.SetContract(env.ctx, &env.api, &env.Events, monitorWasm, monitorAbi)
	assert.NoError(T, err)

	_, err = createRoot(env.ctx, &env.api, &env.DAO)
	assert.NoError(T, err)

	husdMaxSupply, _ := eos.NewAssetFromString("1000000000.00 HUSD")
	_, err = eostest.DeployAndCreateToken(env.ctx, &env.api, tokenHome, &env.HusdToken, &env.DAO, &husdMaxSupply)
	assert.NoError(T, err)

	hyphaMaxSupply, _ := eos.NewAssetFromString("1000000000.00 HYPHA")
	_, err = eostest.DeployAndCreateToken(env.ctx, &env.api, tokenHome, &env.HyphaToken, &env.DAO, &hyphaMaxSupply)
	assert.NoError(T, err)

	hvoiceMaxSupply, _ := eos.NewAssetFromString("1000000000.00 HVOICE")
	_, err = eostest.DeployAndCreateToken(env.ctx, &env.api, tokenHome, &env.HvoiceToken, &env.DAO, &hvoiceMaxSupply)
	assert.NoError(T, err)

	seedsMaxSupply, _ := eos.NewAssetFromString("1000000000.0000 SEEDS")
	_, err = eostest.DeployAndCreateToken(env.ctx, &env.api, tokenHome, &env.SeedsToken, &env.DAO, &seedsMaxSupply)
	assert.NoError(T, err)

	_, err = setConfigAtt(env.ctx, &env.api, &env.DAO, "voting_duration_sec", &configValue{
		BaseVariant: eos.BaseVariant{
			TypeID: configVariant.TypeID("uint64"),
			Impl:   uint64(300),
		},
	})
	assert.NoError(T, err)

	_, err = setConfigAtt(env.ctx, &env.api, &env.DAO, "seeds_deferral_factor_x100", &configValue{
		BaseVariant: eos.BaseVariant{
			TypeID: configVariant.TypeID("uint64"),
			Impl:   uint64(130),
		},
	})
	assert.NoError(T, err)

	_, err = setConfigAtt(env.ctx, &env.api, &env.DAO, "hypha_deferral_factor_x100", &configValue{
		BaseVariant: eos.BaseVariant{
			TypeID: configVariant.TypeID("uint64"),
			Impl:   uint64(50),
		},
	})
	assert.NoError(T, err)

	_, err = setConfigAtt(env.ctx, &env.api, &env.DAO, "paused", &configValue{
		BaseVariant: eos.BaseVariant{
			TypeID: configVariant.TypeID("uint64"),
			Impl:   uint64(0),
		},
	})
	assert.NoError(T, err)

	setNameConfig(env.ctx, &env.api, &env.DAO, "hypha_token_contract", env.HyphaToken)
	setNameConfig(env.ctx, &env.api, &env.DAO, "hvoice_token_contract", env.HvoiceToken)
	setNameConfig(env.ctx, &env.api, &env.DAO, "husd_token_contract", env.HusdToken)
	setNameConfig(env.ctx, &env.api, &env.DAO, "seeds_token_contract", env.SeedsToken)
	// TODO: deploy/configure escrow contract
	setNameConfig(env.ctx, &env.api, &env.DAO, "seeds_escrow_contract", env.SeedsEscrow)
	setNameConfig(env.ctx, &env.api, &env.DAO, "publisher_contract", env.Events)
	setNameConfig(env.ctx, &env.api, &env.DAO, "telos_decide_contract", env.TelosDecide)

	fiveMins, _ := time.ParseDuration("5m")
	_, err = addPeriods(env.ctx, &env.api, env.DAO, 10, fiveMins)
	assert.NoError(T, err)

	_, err = SetupTelosDecide(T, &env)
	assert.NoError(T, err)

	for _, member := range env.Members {

		_, err = Regvoter(env.ctx, &env.api, &env.TelosDecide, &member)
		assert.NoError(T, err)

		_, err = apply(env.ctx, &env.api, &env.DAO, member)
		assert.NoError(T, err)

		_, err = enroll(env.ctx, &env.api, &env.DAO, env.DAO, member)
		assert.NoError(T, err)
	}

	return env
}
