package testnet

import (
	"context"
	"encoding/json"
	"fmt"
	"io/ioutil"
	"log"
	"os"
	"testing"
	"time"

	eostest "github.com/digital-scarcity/eos-go-test"
	"github.com/eoscanada/eos-go"
	"github.com/hypha-dao/document/docgraph"
	"github.com/stretchr/testify/suite"
)

const testingEndpoint = "https://test.telos.kitchen"
const testProposer = "johnnyhypha1"
const contract = "dao.hypha"
const numBadges = 3

type TestnetTestSuite struct {
	suite.Suite
	api *eos.API
	ctx context.Context

	Ballots   [numBadges]eos.Name
	Proposals [numBadges]docgraph.Document
}

func (suite *TestnetTestSuite) SetupSuite() {

	fmt.Println()
	fmt.Println("**************  Setting up the test environment *****************")
	fmt.Println()

	suite.api = eos.New(testingEndpoint)
	//api.Debug = true
	suite.ctx = context.Background()

	keyBag := &eos.KeyBag{}
	err := keyBag.ImportPrivateKey(suite.ctx, os.Getenv("TESTNET_KEY"))
	if err != nil {
		log.Panicf("cannot import default private key: %s", err)
	}
	suite.api.SetSigner(keyBag)
}

func (suite *TestnetTestSuite) TestProposeBadge() {

	tests := []struct {
		name  string
		input string
		title string
	}{
		{
			name:  "enroller",
			title: "Enroller",
			input: "../../scripts/payloads/badges/enroller_badge_proposal.json",
		},
		{
			name:  "librarian",
			title: "Badge Librarian",
			input: "../../scripts/payloads/badges/librarian_badge_proposal.json",
		},
		{
			name:  "treasurer",
			title: "Treasurer",
			input: "../../scripts/payloads/badges/treasurer_badge_proposal.json",
		},
	}

	for testIndex, test := range tests {
		suite.Run(test.name, func() {

			err := ProposeBadgeFromFile(suite.ctx, suite.api, contract, testProposer, test.input)
			suite.Require().NoError(err)

			// unmarshal JSON into a Document
			data, err := ioutil.ReadFile(test.input)
			suite.Require().NoError(err)
			var documentFromFile docgraph.Document
			err = json.Unmarshal(data, &documentFromFile)
			suite.Require().NoError(err)

			lastDoc, err := GetLastDocument(suite.ctx, suite.api, contract)
			suite.Require().NoError(err)
			suite.Proposals[testIndex] = lastDoc

			fv, err := lastDoc.GetContent("title")
			suite.Require().NoError(err)

			suite.Assert().Equal(fv.String(), test.title)

			ballot, err := lastDoc.GetContent("ballot_id")
			suite.Require().NoError(err)
			suite.Ballots[testIndex] = ballot.Impl.(eos.Name)
		})
	}

	time.Sleep(10 * time.Second)

	for ballotIndex := range suite.Ballots {

		actions := []*eos.Action{
			{
				Account: eos.AN("trailservice"),
				Name:    eos.ActN("castvote"),
				Authorization: []eos.PermissionLevel{
					{Actor: testProposer, Permission: eos.PN("active")},
				},
				ActionData: eos.NewActionData(&Vote{
					Voter:      testProposer,
					BallotName: suite.Ballots[ballotIndex],
					Options:    []eos.Name{"pass"},
				}),
			}}

		_, err := eostest.ExecTrx(suite.ctx, suite.api, actions)
		suite.Require().NoError(err)
	}

	time.Sleep(100 * time.Second)

	for proposalIndex := range suite.Proposals {

		actions := []*eos.Action{
			{
				Account: contract,
				Name:    eos.ActN("closedocprop"),
				Authorization: []eos.PermissionLevel{
					{Actor: testProposer, Permission: eos.PN("active")},
				},
				ActionData: eos.NewActionData(&CloseDocProp{
					ProposalHash: suite.Proposals[proposalIndex].Hash,
				}),
			}}

		_, err := eostest.ExecTrx(suite.ctx, suite.api, actions)
		suite.Require().NoError(err)
	}
}

type CloseDocProp struct {
	ProposalHash eos.Checksum256 `json:"proposal_hash"`
}

// Vote represents a set of options being cast as a vote to Telos Decide
type Vote struct {
	Voter      eos.Name   `json:"voter"`
	BallotName eos.Name   `json:"ballot_name"`
	Options    []eos.Name `json:"options"`
}

func (suite *TestnetTestSuite) TestCastVoteForBadgeProposal() {

}

func (suite *TestnetTestSuite) TestCloseBadgeProposal() {

}

func TestContractSuite(t *testing.T) {
	suite.Run(t, new(TestnetTestSuite))
}
