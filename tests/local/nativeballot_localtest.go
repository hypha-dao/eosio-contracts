package local

import (
	"context"
	"testing"

	"github.com/eoscanada/eos-go"
	"github.com/hypha-dao/document/docgraph"
	"github.com/stretchr/testify/suite"
)

// const testingEndpoint = "http://localhost:8888"

// const testProposer = "johnnyhypha1"
// const contract = "dao.hypha"
const numBadges = 3

type NativeBallotTestSuite struct {
	suite.Suite
	api      *eos.API
	ctx      context.Context
	Accounts []eos.AccountName
	DAO      eos.AccountName

	Ballots   [numBadges]eos.Name
	Proposals [numBadges]docgraph.Document
}

func (suite *NativeBallotTestSuite) SetupSuite() {

	// fmt.Println()
	// fmt.Println("**************  Setting up the test environment *****************")
	// fmt.Println()

	// suite.api = eos.New(testingEndpoint)
	// //api.Debug = true
	// suite.ctx = context.Background()

	// keyBag := &eos.KeyBag{}
	// err := keyBag.ImportPrivateKey(suite.ctx, eostest.DefaultKey())
	// if err != nil {
	// 	log.Panicf("cannot import default private key: %s", err)
	// }
	// suite.api.SetSigner(keyBag)

	// suite.Accounts, err = eostest.CreateRandoms(suite.ctx, suite.api, 20)
	// if err != nil {
	// 	log.Panicf("cannot create random accounts: %s", err)
	// }

	// suite.DocsContract = suite.Accounts[0]

	// trxID, err := eostest.SetContract(suite.ctx, suite.api, &suite.DocsContract, "../../hyphadao/hyphadao.wasm", "../docs/docs.abi")
	// if err != nil {
	// 	log.Panicf("cannot set contract: %s", err)
	// }
	// log.Println("Set Docs contract	: ", suite.DocsContract, " : ", trxID)
}

func (suite *NativeBallotTestSuite) TestProposeBadge() {

	// tests := []struct {
	// 	name  string
	// 	input string
	// 	title string
	// }{
	// 	{
	// 		name:  "healer",
	// 		title: "Healer",
	// 		input: "../../scripts/payloads/badges/notd_healer_badge_proposal.json",
	// 	},
	// }

	// for testIndex, test := range tests {
	// 	suite.Run(test.name, func() {

	// 		err := ProposeBadgeFromFile(suite.ctx, suite.api, contract, testProposer, test.input)
	// 		suite.Require().NoError(err)

	// 		// unmarshal JSON into a Document
	// 		data, err := ioutil.ReadFile(test.input)
	// 		suite.Require().NoError(err)
	// 		var documentFromFile docgraph.Document
	// 		err = json.Unmarshal(data, &documentFromFile)
	// 		suite.Require().NoError(err)

	// 		lastDoc, err := GetLastDocument(suite.ctx, suite.api, contract)
	// 		suite.Require().NoError(err)
	// 		suite.Proposals[testIndex] = lastDoc

	// 		fv, err := lastDoc.GetContent("title")
	// 		suite.Require().NoError(err)

	// 		suite.Assert().Equal(fv.String(), test.title)

	// 		// it should not have a ballot
	// 		_, err = lastDoc.GetContent("ballot_id")
	// 		suite.Require().Error(err)

	// 	})
	// }

	// time.Sleep(10 * time.Second)

	// for ballotIndex := range suite.Ballots {

	// 	actions := []*eos.Action{
	// 		{
	// 			Account: eos.AN("trailservice"),
	// 			Name:    eos.ActN("castvote"),
	// 			Authorization: []eos.PermissionLevel{
	// 				{Actor: testProposer, Permission: eos.PN("active")},
	// 			},
	// 			ActionData: eos.NewActionData(&Vote{
	// 				Voter:      testProposer,
	// 				BallotName: suite.Ballots[ballotIndex],
	// 				Options:    []eos.Name{"pass"},
	// 			}),
	// 		}}

	// 	_, err := eostest.ExecTrx(suite.ctx, suite.api, actions)
	// 	suite.Require().NoError(err)
	// }

	// time.Sleep(100 * time.Second)

	// for proposalIndex := range suite.Proposals {

	// 	actions := []*eos.Action{
	// 		{
	// 			Account: contract,
	// 			Name:    eos.ActN("closedocprop"),
	// 			Authorization: []eos.PermissionLevel{
	// 				{Actor: testProposer, Permission: eos.PN("active")},
	// 			},
	// 			ActionData: eos.NewActionData(&CloseDocProp{
	// 				ProposalHash: suite.Proposals[proposalIndex].Hash,
	// 			}),
	// 		}}

	// 	_, err := eostest.ExecTrx(suite.ctx, suite.api, actions)
	// 	suite.Require().NoError(err)
	// }
}

// type CloseDocProp struct {
// 	ProposalHash eos.Checksum256 `json:"proposal_hash"`
// }

// // Vote represents a set of options being cast as a vote to Telos Decide
// type Vote struct {
// 	Voter      eos.Name   `json:"voter"`
// 	BallotName eos.Name   `json:"ballot_name"`
// 	Options    []eos.Name `json:"options"`
// }

// func (suite *TestnetTestSuite) TestCastVoteForBadgeProposal() {

// }

// func (suite *TestnetTestSuite) TestCloseBadgeProposal() {

// }

func TestNativeBallotSuite(t *testing.T) {
	suite.Run(t, new(NativeBallotTestSuite))
}
