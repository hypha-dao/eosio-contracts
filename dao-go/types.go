package dao

import (
	"context"
	"strconv"
	"time"

	eos "github.com/eoscanada/eos-go"
)

// LoadV1Object loads an existing DAO object by its scope and ID
func LoadV1Object(ctx context.Context, api *eos.API, contract, scope string, ID uint64) V1Object {
	var objects []RawObject
	var request eos.GetTableRowsRequest
	request.Code = contract
	request.Scope = scope
	request.Table = "objects"
	request.Limit = 1
	request.LowerBound = strconv.Itoa(int(ID))
	request.UpperBound = strconv.Itoa(int(ID))
	request.JSON = true
	response, _ := api.GetTableRows(ctx, request)
	response.JSONToStructs(&objects)
	return ToV1Object(objects[0])
}

// LoadV1Objects from a provided eos.API and scope
func LoadV1Objects(ctx context.Context, api *eos.API, contract, scope string) []V1Object {
	var docs []V1Object
	objects := LoadRawObjects(ctx, api, contract, scope)
	docs = make([]V1Object, len(objects))
	for index, object := range objects {
		docs[index] = ToV1Object(object)
	}
	return docs
}

// GetLastObject gets the last object for the scope
func GetLastObject(ctx context.Context, api *eos.API, contract, scope string) V1Object {
	var objects []RawObject
	var request eos.GetTableRowsRequest
	request.Code = contract
	request.Scope = scope
	request.Table = "objects"
	request.Reverse = true
	request.Limit = 1
	request.JSON = true
	response, _ := api.GetTableRows(ctx, request)
	response.JSONToStructs(&objects)
	return ToV1Object(objects[0])
}

// V1Object is a generic object from the objects table
type V1Object struct {
	ID           uint64                        `json:"id"`
	Names        map[string]eos.Name           `json:"names"`
	Strings      map[string]string             `json:"strings"`
	Assets       map[string]eos.Asset          `json:"assets"`
	TimePoints   map[string]eos.BlockTimestamp `json:"time_points"`
	Ints         map[string]uint64             `json:"ints"`
	Transactions map[string]eos.Transaction    `json:"trxs"`
	Floats       map[string]eos.Float128       `json:"floats"`
	CreatedDate  eos.BlockTimestamp            `json:"created_date"`
	UpdatedDate  eos.BlockTimestamp            `json:"updated_date"`
}

// ToMap converts an array of kv pairs from an EOSIO table to a native Go map
func ToMap(strings []StringKV) *map[string]string {
	stringMap := make(map[string]string, len(strings))
	for _, element := range strings {
		stringMap[element.Key] = element.Value
	}
	return &stringMap
}

// ToV1Object Converts a very generic Object to one of type DAO Object
func ToV1Object(objs RawObject) V1Object {

	var daoObject V1Object
	daoObject.Names = make(map[string]eos.Name)
	for index, element := range objs.Names {
		daoObject.Names[element.Key] = objs.Names[index].Value
	}

	daoObject.Assets = make(map[string]eos.Asset)
	for index, element := range objs.Assets {
		daoObject.Assets[element.Key] = objs.Assets[index].Value
	}

	daoObject.TimePoints = make(map[string]eos.BlockTimestamp)
	for index, element := range objs.TimePoints {
		daoObject.TimePoints[element.Key] = objs.TimePoints[index].Value
	}

	daoObject.Ints = make(map[string]uint64)
	for index, element := range objs.Ints {
		daoObject.Ints[element.Key] = objs.Ints[index].Value
	}

	daoObject.Strings = make(map[string]string)
	for index, element := range objs.Strings {
		daoObject.Strings[element.Key] = objs.Strings[index].Value
	}
	daoObject.ID = objs.ID
	daoObject.CreatedDate = objs.CreatedDate
	daoObject.UpdatedDate = objs.UpdatedDate
	return daoObject
}

// QrAction ...
type QrAction struct {
	Timestamp      time.Time `json:"@timestamp"`
	TrxID          string    `json:"trx_id"`
	ActionContract string
	ActionName     string
	Data           string
}

// LoadRawObjects from a provided eos.API and scope
func LoadRawObjects(ctx context.Context, api *eos.API, contract, scope string) []RawObject {
	var objects []RawObject
	var request eos.GetTableRowsRequest
	request.Code = contract
	request.Scope = scope
	request.Table = "objects"
	request.Limit = 1000
	request.JSON = true
	request.Reverse = true
	response, _ := api.GetTableRows(ctx, request)
	response.JSONToStructs(&objects)
	return objects
}

// NameKV struct
type NameKV struct {
	Key   string   `json:"key"`
	Value eos.Name `json:"value"`
}

// StringKV struct
type StringKV struct {
	Key   string `json:"key"`
	Value string `json:"value"`
}

// AssetKV struct
type AssetKV struct {
	Key   string    `json:"key"`
	Value eos.Asset `json:"value"`
}

// TimePointKV struct
type TimePointKV struct {
	Key   string             `json:"key"`
	Value eos.BlockTimestamp `json:"value"`
}

// IntKV struct
type IntKV struct {
	Key   string `json:"key"`
	Value uint64 `json:"value"`
}

// TrxKV struct
type TrxKV struct {
	Key   string          `json:"key"`
	Value eos.Transaction `json:"value"`
}

// FloatKV struct
type FloatKV struct {
	Key   string       `json:"key"`
	Value eos.Float128 `json:"value"`
}

// Scope ...
type Scope struct {
	Code  eos.Name `json:"code"`
	Scope eos.Name `json:"scope"`
	Table eos.Name `json:"table"`
	Payer eos.Name `json:"payer"`
	Count uint64   `json:"count"`
}

// RawObject struct
type RawObject struct {
	ID           uint64             `json:"id"`
	Names        []NameKV           `json:"names"`
	Strings      []StringKV         `json:"strings"`
	Assets       []AssetKV          `json:"assets"`
	TimePoints   []TimePointKV      `json:"time_points"`
	Ints         []IntKV            `json:"ints"`
	Transactions []TrxKV            `json:"trxs"`
	Floats       []FloatKV          `json:"floats"`
	CreatedDate  eos.BlockTimestamp `json:"created_date"`
	UpdatedDate  eos.BlockTimestamp `json:"updated_date"`
}

// CreateP ...
type CreateP struct {
	Scope        eos.Name      `json:"scope"`
	Names        []NameKV      `json:"names"`
	Strings      []StringKV    `json:"strings"`
	Assets       []AssetKV     `json:"assets"`
	TimePoints   []TimePointKV `json:"time_points"`
	Ints         []IntKV       `json:"ints"`
	Floats       []FloatKV     `json:"floats"`
	Transactions []TrxKV       `json:"trxs"`
}

// SeedsExchConfigTable ...
type SeedsExchConfigTable struct {
	SeedsPerUsd   eos.Asset `json:"seeds_per_usd"`
	TlosPerUsd    eos.Asset `json:"tlos_per_usd"`
	CitizenLimit  eos.Asset `json:"citizen_limit"`
	ResidentLimit eos.Asset `json:"resident_limit"`
	VisitorLimit  eos.Asset `json:"visitor_limit"`
}

// SeedsPriceHistory ...
type SeedsPriceHistory struct {
	ID       uint64        `json:"id"`
	SeedsUSD eos.Asset     `json:"seeds_usd"`
	Date     eos.TimePoint `json:"date"`
}

// Period represents a period of time aligning to a payroll period, typically a week
type Period struct {
	PeriodID  uint64             `json:"period_id"`
	StartTime eos.BlockTimestamp `json:"start_date"`
	EndTime   eos.BlockTimestamp `json:"end_date"`
	Phase     string             `json:"phase"`
}
