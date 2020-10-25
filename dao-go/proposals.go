package dao

import (
	"context"
	"encoding/json"
	"fmt"
	"io/ioutil"

	eostest "github.com/digital-scarcity/eos-go-test"
	"github.com/eoscanada/eos-go"
	"github.com/hypha-dao/document/docgraph"
)

// CloseDocProp is closing proposals for document proposals
type CloseDocProp struct {
	ProposalHash eos.Checksum256 `json:"proposal_hash"`
}

// Vote represents a set of options being cast as a vote to Telos Decide
type Vote struct {
	Voter      eos.AccountName `json:"voter"`
	BallotName eos.Name        `json:"ballot_name"`
	Options    []eos.Name      `json:"options"`
}

// Proposal is a document to be proposed
type Proposal struct {
	Proposer      eos.AccountName         `json:"proposer"`
	ProposalType  eos.Name                `json:"proposal_type"`
	ContentGroups []docgraph.ContentGroup `json:"content_groups"`
}

// ProposeBadgeFromFile proposes the badge to the specified DAO contract
func ProposeBadgeFromFile(ctx context.Context, api *eos.API, contract, proposer eos.AccountName, fileName string) (string, error) {
	data, err := ioutil.ReadFile(fileName)
	if err != nil {
		return "error", fmt.Errorf("ProposeBadgeFromFile : %v", err)
	}

	action := eostest.ToActionName("propose", "action")

	var dump map[string]interface{}
	err = json.Unmarshal(data, &dump)
	if err != nil {
		return "error", fmt.Errorf("ProposeBadgeFromFile : %v", err)
	}

	dump["proposer"] = proposer
	dump["proposal_type"] = "badge"

	actionBinary, err := api.ABIJSONToBin(ctx, contract, eos.Name(action), dump)

	actions := []*eos.Action{{
		Account: contract,
		Name:    action,
		Authorization: []eos.PermissionLevel{
			{Actor: proposer, Permission: eos.PN("active")},
		},
		ActionData: eos.NewActionDataFromHexData([]byte(actionBinary)),
	}}

	return eostest.ExecTrx(ctx, api, actions)
}

// ProposeBadgeAssignment proposes the badge assignment to the specified DAO contract
func ProposeBadgeAssignment(ctx context.Context, api *eos.API,
	contract, proposer eos.AccountName,
	badgeHash eos.Checksum256, fileName string) (string, error) {

	var badgeAssignmentDoc docgraph.Document

	data, err := ioutil.ReadFile(fileName)
	if err != nil {
		return "error", fmt.Errorf("ProposeBadgeAssignment : %v", err)
	}

	err = json.Unmarshal(data, &badgeAssignmentDoc)
	if err != nil {
		return "error", fmt.Errorf("ProposeBadgeAssignment unmarshal : %v", err)
	}

	action := eos.ActN("propose")

	fv := &docgraph.FlexValue{
		BaseVariant: eos.BaseVariant{
			TypeID: docgraph.GetVariants().TypeID("checksum256"),
			Impl:   badgeHash,
		}}

	var ci docgraph.ContentItem
	ci.Label = "badge"
	ci.Value = fv

	badgeAssignmentDoc.ContentGroups[0] = append(badgeAssignmentDoc.ContentGroups[0], ci)

	err = json.Unmarshal(data, &badgeAssignmentDoc)

	actions := []*eos.Action{{
		Account: contract,
		Name:    action,
		Authorization: []eos.PermissionLevel{
			{Actor: proposer, Permission: eos.PN("active")},
		},
		ActionData: eos.NewActionData(Proposal{
			Proposer:      proposer,
			ProposalType:  eos.Name("badgeassign"),
			ContentGroups: badgeAssignmentDoc.ContentGroups,
		})}}

	return eostest.ExecTrx(ctx, api, actions)
}

// TelosDecideVote ...
func TelosDecideVote(ctx context.Context, api *eos.API,
	telosDecide, voter eos.AccountName, ballot,
	passFail eos.Name) (string, error) {

	actions := []*eos.Action{{
		Account: telosDecide,
		Name:    eos.ActN("castvote"),
		Authorization: []eos.PermissionLevel{
			{Actor: voter, Permission: eos.PN("active")},
		},
		ActionData: eos.NewActionData(&Vote{
			Voter:      voter,
			BallotName: ballot,
			Options:    []eos.Name{passFail},
		}),
	}}

	return eostest.ExecTrx(ctx, api, actions)
}

// DocumentVote ....
func DocumentVote(ctx context.Context, api *eos.API,
	contract, proposer eos.AccountName,
	badgeHash eos.Checksum256, notes string,
	startPeriod, endPeriod uint64) error {

	return nil
}

// CloseProposal ...
func CloseProposal(ctx context.Context, api *eos.API, contract, closer eos.AccountName,
	proposalHash eos.Checksum256) (string, error) {

	actions := []*eos.Action{{
		Account: contract,
		Name:    eos.ActN("closedocprop"),
		Authorization: []eos.PermissionLevel{
			{Actor: closer, Permission: eos.PN("active")},
		},
		ActionData: eos.NewActionData(&CloseDocProp{
			ProposalHash: proposalHash,
		}),
	}}

	return eostest.ExecTrx(ctx, api, actions)
}
