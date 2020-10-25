package local_test

import (
	"context"
	"encoding/json"
	"fmt"
	"io/ioutil"

	eostest "github.com/digital-scarcity/eos-go-test"
	"github.com/eoscanada/eos-go"
)

// ProposeBadgeFromFile proposes the badge to the specified DAO contract
func ProposeBadgeFromFile(ctx context.Context, api *eos.API, contract, proposer eos.AccountName, fileName string) error {
	data, err := ioutil.ReadFile(fileName)
	if err != nil {
		return fmt.Errorf("ProposeBadgeFromFile : %v", err)
	}

	action := eostest.ToActionName("propose", "action")

	var dump map[string]interface{}
	err = json.Unmarshal(data, &dump)
	if err != nil {
		return fmt.Errorf("ProposeBadgeFromFile : %v", err)
	}

	dump["proposer"] = proposer
	dump["proposal_type"] = "badge"

	actionBinary, err := api.ABIJSONToBin(ctx, contract, eos.Name(action), dump)

	actions := []*eos.Action{
		{
			Account: contract,
			Name:    action,
			Authorization: []eos.PermissionLevel{
				{Actor: proposer, Permission: eos.PN("active")},
			},
			ActionData: eos.NewActionDataFromHexData([]byte(actionBinary)),
		}}

	_, err = eostest.ExecTrx(ctx, api, actions)
	if err != nil {
		return fmt.Errorf("ProposeBadgeFromFile : %v", err)
	}

	return nil
}

// ProposeBadgeAssignmentFromFile proposes the badge to the specified DAO contract
func ProposeBadgeAssignmentFromFile(ctx context.Context, api *eos.API,
	contract, proposer eos.AccountName,
	badgeHash eos.Checksum256,
	startPeriod, endPeriod uint64) error {

	// action := eostest.ToActionName("propose", "action")

	// var dump map[string]interface{}
	// err = json.Unmarshal(data, &dump)
	// if err != nil {
	// 	return fmt.Errorf("ProposeBadgeFromFile : %v", err)
	// }

	// dump["proposer"] = proposer
	// dump["proposal_type"] = "badge"

	// actionBinary, err := api.ABIJSONToBin(ctx, contract, eos.Name(action), dump)

	// actions := []*eos.Action{
	// 	{
	// 		Account: contract,
	// 		Name:    action,
	// 		Authorization: []eos.PermissionLevel{
	// 			{Actor: proposer, Permission: eos.PN("active")},
	// 		},
	// 		ActionData: eos.NewActionDataFromHexData([]byte(actionBinary)),
	// 	}}

	// _, err = eostest.ExecTrx(ctx, api, actions)
	// if err != nil {
	// 	return fmt.Errorf("ProposeBadgeFromFile : %v", err)
	// }

	return nil
}
