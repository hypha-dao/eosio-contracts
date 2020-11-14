package dao_test

import (
	"encoding/json"
	"fmt"
	"strconv"
	"testing"
	"time"

	eostest "github.com/digital-scarcity/eos-go-test"
	"github.com/eoscanada/eos-go"
	"github.com/hypha-dao/dao-go"
	"github.com/hypha-dao/document/docgraph"
	"github.com/k0kubun/go-ansi"
	progressbar "github.com/schollz/progressbar/v3"
	"gotest.tools/assert"
)

type Balance struct {
	SnapshotTime time.Time
	Period       uint64
	Hypha        eos.Asset
	Hvoice       eos.Asset
	SeedsEscrow  eos.Asset
	Husd         eos.Asset
}

func (b *Balance) String() string {
	s := "\n"
	s += "Period:		" + strconv.Itoa(int(b.Period)) + "\n"
	s += "Time: 		" + b.SnapshotTime.String() + "\n"
	s += "Hypha: 		" + b.Hypha.String() + "\n"
	s += "Husd: 		" + b.Husd.String() + "\n"
	s += "SeedsEscrow:	" + b.SeedsEscrow.String() + "\n"
	s += "Hvoice:		" + b.Hvoice.String() + "\n"
	return s
}

func PercentageChange(old, new int) (delta float64) {
	diff := float64(new - old)
	delta = (diff / float64(old)) * 100
	return
}

func NewBalance() Balance {

	hypha, _ := eos.NewAssetFromString("0.00 HYPHA")
	hvoice, _ := eos.NewAssetFromString("0.00 HVOICE")
	husd, _ := eos.NewAssetFromString("0.00 HUSD")
	seedsEscrow, _ := eos.NewAssetFromString("0.0000 SEEDS")
	return Balance{
		SnapshotTime: time.Now(),
		// Period:       0,
		Hypha:       hypha,
		Hvoice:      hvoice,
		Husd:        husd,
		SeedsEscrow: seedsEscrow,
	}
}

func CalcLastPayment(t *testing.T, env *Environment, prevBal Balance, acct eos.AccountName) Balance {
	currentBalance := GetBalance(t, env, acct)
	return Balance{
		SnapshotTime: time.Now(),
		// Period:       p,
		Hypha:       currentBalance.Hypha.Sub(prevBal.Hypha),
		SeedsEscrow: currentBalance.SeedsEscrow.Sub(prevBal.SeedsEscrow),
		Husd:        currentBalance.Husd.Sub(prevBal.Husd),
	}
}

func GetBalance(t *testing.T, env *Environment, acct eos.AccountName) Balance {

	return Balance{
		SnapshotTime: time.Now(),
		// Period:       p,
		Hypha:       dao.GetBalance(env.ctx, &env.api, string(env.HyphaToken), string(acct)),
		Husd:        dao.GetBalance(env.ctx, &env.api, string(env.HusdToken), string(acct)),
		SeedsEscrow: dao.GetEscrowBalance(env.ctx, &env.api, string(env.SeedsEscrow), string(acct)),
	}
}

// IsClaimed ...
func IsClaimed(periodID uint64, payments []dao.AssignmentPay) bool {
	for _, payment := range payments {
		if payment.PeriodID == periodID {
			return true
		}
	}
	return false
}

// ClaimNextPeriod claims a period of pay for an assignment
func ClaimNextPeriod(t *testing.T, env *Environment, claimer eos.AccountName, assignment dao.V1Object) (string, error) {

	var payments []dao.AssignmentPay
	var request eos.GetTableRowsRequest
	request.Code = string(env.DAO)
	request.Scope = string(env.DAO)
	request.Table = "payments"
	request.Index = "4"
	request.KeyType = "i64"
	request.Limit = 1000
	request.LowerBound = strconv.Itoa(int(assignment.ID))
	request.UpperBound = strconv.Itoa(int(assignment.ID))
	request.JSON = true
	response, _ := env.api.GetTableRows(env.ctx, request)
	response.JSONToStructs(&payments)

	periods, err := dao.LoadPeriods(&env.api, true, true)
	assert.NilError(t, err)

	periodIndex := assignment.Ints["start_period"]

	// t.Log("Assignment Created Date	: ", assignment.CreatedDate.Time.String())

	for periodIndex <= assignment.Ints["end_period"] {
		// t.Log("Period: ", periodIndex, " Start Time: ", periods[periodIndex].StartTime.Time.String())
		if assignment.CreatedDate.Time.After(periods[periodIndex].StartTime.Time) || IsClaimed(periodIndex, payments) {
			periodIndex++
			continue
		}
		break
	}

	if time.Now().Before(periods[periodIndex].EndTime.Time) {
		t.Log("Waiting for a period to lapse...")
		pause(t, periodPause, "", "Waiting...")
	}

	return dao.ClaimPay(env.ctx, &env.api, env.DAO, claimer, assignment.ID, periodIndex)
}

func pause(t *testing.T, seconds time.Duration, headline, prefix string) {
	if headline != "" {
		t.Log(headline)
	}

	bar := progressbar.NewOptions(100,
		progressbar.OptionSetWriter(ansi.NewAnsiStdout()),
		progressbar.OptionEnableColorCodes(true),
		progressbar.OptionSetWidth(90),
		// progressbar.OptionShowIts(),
		progressbar.OptionSetDescription("[cyan]"+fmt.Sprintf("%20v", prefix)),
		progressbar.OptionSetTheme(progressbar.Theme{
			Saucer:        "[green]=[reset]",
			SaucerHead:    "[green]>[reset]",
			SaucerPadding: " ",
			BarStart:      "[",
			BarEnd:        "]",
		}))

	chunk := seconds / 100
	for i := 0; i < 100; i++ {
		bar.Add(1)
		time.Sleep(chunk)
	}
	fmt.Println()
	fmt.Println()
}

func createVoteAndClose(t *testing.T, env *Environment,
	proposer, closer eos.AccountName, obj dao.RawObject) dao.V1Object {

	action := eos.ActN("create")

	createP := dao.CreateP{
		Scope:        eos.Name("proposal"),
		Names:        obj.Names,
		Strings:      obj.Strings,
		Assets:       obj.Assets,
		TimePoints:   obj.TimePoints,
		Ints:         obj.Ints,
		Floats:       obj.Floats,
		Transactions: obj.Transactions,
	}

	actions := []*eos.Action{{
		Account: env.DAO,
		Name:    action,
		Authorization: []eos.PermissionLevel{
			{Actor: proposer, Permission: eos.PN("active")},
		},
		ActionData: eos.NewActionData(createP)}}

	t.Log("Member: " + string(proposer) + " is submitting a role proposal")
	pause(t, chainResponsePause, "", "Submitting...")
	_, err := eostest.ExecTrx(env.ctx, &env.api, actions)
	assert.NilError(t, err)

	proposal := dao.GetLastObject(env.ctx, &env.api, string(env.DAO), "proposal")
	ballot := proposal.Names["ballot_id"]
	assert.NilError(t, err)

	voteToPassTD(t, env, ballot)

	t.Log("Member: ", closer, ", is closing proposal: ", string(ballot))
	_, err = dao.CloseProposalOld(env.ctx, &env.api, env.DAO, closer, proposal.ID)
	assert.NilError(t, err)

	return proposal
}

func createAssignment(t *testing.T, env *Environment, role dao.V1Object, proposer, assignee, closer eos.AccountName, content string) dao.V1Object {

	var assignmentProposal dao.RawObject
	err := json.Unmarshal([]byte(content), &assignmentProposal)
	assert.NilError(t, err)

	assignmentProposal.Names = append(assignmentProposal.Names, dao.NameKV{Key: "owner", Value: eos.Name(proposer)})
	assignmentProposal.Names = append(assignmentProposal.Names, dao.NameKV{Key: "assigned_account", Value: eos.Name(assignee)})
	assignmentProposal.Ints = append(assignmentProposal.Ints, dao.IntKV{Key: "role_id", Value: role.Ints["role_id"]})

	return createVoteAndClose(t, env, proposer, closer, assignmentProposal)
}

func createRole(t *testing.T, env *Environment, proposer, closer eos.AccountName, content string) dao.V1Object {

	var roleProposal dao.RawObject
	err := json.Unmarshal([]byte(content), &roleProposal)
	assert.NilError(t, err)

	roleProposal.Names = append(roleProposal.Names, dao.NameKV{Key: "owner", Value: eos.Name(proposer)})

	return createVoteAndClose(t, env, proposer, closer, roleProposal)
}

func loadSeedsTablesFromProd(t *testing.T, env *Environment, prodEndpoint string) {
	prodApi := *eos.New(prodEndpoint)

	var config []dao.SeedsExchConfigTable
	var request eos.GetTableRowsRequest
	request.Code = "tlosto.seeds"
	request.Scope = "tlosto.seeds"
	request.Table = "config"
	request.Limit = 1
	request.JSON = true
	response, _ := prodApi.GetTableRows(env.ctx, request)
	response.JSONToStructs(&config)

	action := eos.ActN("updateconfig")

	actions := []*eos.Action{{
		Account: env.SeedsExchange,
		Name:    action,
		Authorization: []eos.PermissionLevel{
			{Actor: env.SeedsExchange, Permission: eos.PN("active")},
		},
		ActionData: eos.NewActionData(config[0])}}

	t.Log("Copying configuration table from production for 	: " + string(env.SeedsExchange))
	_, err := eostest.ExecTrx(env.ctx, &env.api, actions)
	assert.NilError(t, err)

	var priceHistory []dao.SeedsPriceHistory
	var request2 eos.GetTableRowsRequest
	request2.Code = "tlosto.seeds"
	request2.Scope = "tlosto.seeds"
	request2.Table = "pricehistory"
	request2.Limit = 1000
	request2.JSON = true
	response2, _ := prodApi.GetTableRows(env.ctx, request2)
	response2.JSONToStructs(&priceHistory)

	action = eos.ActN("inshistory")

	t.Log("Copying Seeds price history records from production for 	: " + string(env.SeedsExchange))
	for _, record := range priceHistory {
		actions := []*eos.Action{{
			Account: env.SeedsExchange,
			Name:    action,
			Authorization: []eos.PermissionLevel{
				{Actor: env.SeedsExchange, Permission: eos.PN("active")},
			},
			ActionData: eos.NewActionData(record)},
		}

		_, err := eostest.ExecTrx(env.ctx, &env.api, actions)
		assert.NilError(t, err)
	}
}

func checkEdge(t *testing.T, env *Environment, fromEdge, toEdge docgraph.Document, edgeName eos.Name) {
	exists, err := docgraph.EdgeExists(env.ctx, &env.api, env.DAO, fromEdge, toEdge, edgeName)
	assert.NilError(t, err)
	if !exists {
		t.Log("Edge does not exist	: ", fromEdge.Hash.String(), "	-- ", edgeName, "	--> 	", toEdge.Hash.String())
	}
	assert.Check(t, exists)
}

func voteToPassTD(t *testing.T, env *Environment, ballot eos.Name) {
	t.Log("Voting all members to 'pass' on ballot: " + ballot)

	_, err := dao.TelosDecideVote(env.ctx, &env.api, env.TelosDecide, env.Whale.Member, ballot, eos.Name("pass"))
	assert.NilError(t, err)

	for _, member := range env.Members {
		_, err = dao.TelosDecideVote(env.ctx, &env.api, env.TelosDecide, member.Member, ballot, eos.Name("pass"))
		assert.NilError(t, err)
	}
	t.Log("Allowing the ballot voting period to lapse")
	pause(t, votingPause, "", "Voting...")
}
