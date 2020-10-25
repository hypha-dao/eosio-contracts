package dao

import (
	"context"
	"encoding/json"
	"fmt"
	"io/ioutil"
	"time"

	eostest "github.com/digital-scarcity/eos-go-test"
	"github.com/eoscanada/eos-go"
)

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

// SetConfigAtt sets a single attribute on the configuration
func SetConfigAtt(ctx context.Context, api *eos.API, contract *eos.AccountName, configAtt string, flexValue *configValue) (string, error) {

	action := eos.ActN("setconfigatt")
	actionData := make(map[string]interface{})
	actionData["key"] = configAtt
	actionData["value"] = flexValue

	actionBinary, err := api.ABIJSONToBin(ctx, *contract, eos.Name(action), actionData)
	if err != nil {
		return "error", fmt.Errorf("cannot pack action data %v: %v", configAtt, err)
	}

	actions := []*eos.Action{{
		Account: *contract,
		Name:    action,
		Authorization: []eos.PermissionLevel{
			{Actor: *contract, Permission: eos.PN("active")},
		},
		ActionData: eos.NewActionDataFromHexData([]byte(actionBinary)),
	}}

	return eostest.ExecTrx(ctx, api, actions)
}

// SetNameConfigAtt is a helper for setting a single name type configuration item
func SetNameConfigAtt(ctx context.Context, api *eos.API, contract *eos.AccountName, label string, value eos.AccountName) (string, error) {
	return SetConfigAtt(ctx, api, contract, label, &configValue{
		BaseVariant: eos.BaseVariant{
			TypeID: configVariant.TypeID("name"),
			Impl:   value,
		},
	})
}

// SetIntConfigAtt is a helper for setting a single name type configuration item
func SetIntConfigAtt(ctx context.Context, api *eos.API, contract *eos.AccountName, label string, value uint64) (string, error) {
	return SetConfigAtt(ctx, api, contract, label, &configValue{
		BaseVariant: eos.BaseVariant{
			TypeID: configVariant.TypeID("uint64"),
			Impl:   value,
		},
	})
}

// SetConfig sets the configuration based on file
func SetConfig(ctx context.Context, api *eos.API, contract *eos.AccountName, configFile string) (string, error) {
	data, err := ioutil.ReadFile(configFile)
	if err != nil {
		return "error", fmt.Errorf("cannot read file %v: %v", configFile, err)
	}

	action := eos.ActN("setconfig")

	var dump map[string]interface{}
	err = json.Unmarshal(data, &dump)
	if err != nil {
		return "error", fmt.Errorf("cannot unmarshal configuration %v: %v", configFile, err)
	}

	actionBinary, err := api.ABIJSONToBin(ctx, *contract, eos.Name(action), dump)
	if err != nil {
		return "error", fmt.Errorf("cannot pack action data %v: %v", configFile, err)
	}

	actions := []*eos.Action{{
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

// AddPeriods adds the number of periods with the corresponding duration to the DAO
func AddPeriods(ctx context.Context, api *eos.API, daoContract eos.AccountName, numPeriods int, periodDuration time.Duration) (string, error) {

	now := time.Now()
	startTime := eos.TimePoint(now.UnixNano() / 1000)
	endTime := eos.TimePoint(now.Add(periodDuration).UnixNano() / 1000)

	var periods []*eos.Action
	for i := 0; i < numPeriods; i++ {
		addPeriodAction := eos.Action{
			Account: daoContract,
			Name:    eos.ActN("addperiod"),
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

type applyParm struct {
	Applicant eos.AccountName
	Notes     string
}

// Apply applies for membership to the DAO
func Apply(ctx context.Context, api *eos.API, contract *eos.AccountName,
	applicant eos.AccountName, notes string) (string, error) {

	actions := []*eos.Action{{
		Account: *contract,
		Name:    eos.ActN("apply"),
		Authorization: []eos.PermissionLevel{
			{Actor: applicant, Permission: eos.PN("active")},
		},
		ActionData: eos.NewActionData(applyParm{
			Applicant: applicant,
			Notes:     notes,
		}),
	}}
	return eostest.ExecTrx(ctx, api, actions)
}

type enrollParm struct {
	Enroller  eos.AccountName
	Applicant eos.AccountName
	Content   string
}

// Enroll an applicant in the DAO
func Enroll(ctx context.Context, api *eos.API, contract *eos.AccountName, enroller, applicant eos.AccountName) (string, error) {
	actions := []*eos.Action{
		{
			Account: *contract,
			Name:    eos.ActN("enroll"),
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

// CreateRoot creates the root node
func CreateRoot(ctx context.Context, api *eos.API, contract *eos.AccountName) (string, error) {
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
