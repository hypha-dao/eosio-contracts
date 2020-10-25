package local_test

import (
	"encoding/json"
	"io/ioutil"
	"log"
	"os"
	"os/exec"
	"testing"
	"time"

	eostest "github.com/digital-scarcity/eos-go-test"
	"github.com/eoscanada/eos-go"
	"github.com/hypha-dao/document/docgraph"
	"gotest.tools/assert"
)

type CloseDocProp struct {
	ProposalHash eos.Checksum256 `json:"proposal_hash"`
}

// Vote represents a set of options being cast as a vote to Telos Decide
type Vote struct {
	Voter      eos.Name   `json:"voter"`
	BallotName eos.Name   `json:"ballot_name"`
	Options    []eos.Name `json:"options"`
}

func setupTestCase(t *testing.T) func(t *testing.T) {
	t.Log("(Re)starting Nodeos")

	_, err := exec.Command("sh", "-c", "pkill -SIGINT nodeos").Output()
	if err == nil {
		time.Sleep(2 * time.Second)
	}

	cmd := exec.Command("./nodeos.sh")
	cmd.Stdout = os.Stdout
	err = cmd.Start()
	assert.NilError(t, err)

	time.Sleep(3 * time.Second)

	return func(t *testing.T) {
		// t.Log("teardown test case")
		// _, err := exec.Command("sh", "-c", "pkill -SIGINT nodeos").Output()
		// assert.NilError(t, err)
	}
}

func TestSimple(t *testing.T) {

	teardownTestCase := setupTestCase(t)
	defer teardownTestCase(t)

	var env Environment
	t.Run("Setting up the DAO environment: ", func(t *testing.T) {
		env := SetupEnvironment(t)
		log.Println()
		log.Println(env.String())
	})

	t.Run("Badges proposals", func(t *testing.T) {
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

		proposals := make([]docgraph.Document, len(tests))
		ballots := make([]eos.Name, len(tests))

		for testIndex, test := range tests {
			t.Run(test.name, func(t *testing.T) {

				err := ProposeBadgeFromFile(env.ctx, &env.api, env.DAO, env.Members[0], test.input)
				assert.NilError(t, err)

				// unmarshal JSON into a Document
				data, err := ioutil.ReadFile(test.input)
				assert.NilError(t, err)
				var documentFromFile docgraph.Document
				err = json.Unmarshal(data, &documentFromFile)
				assert.NilError(t, err)

				lastDoc, err := docgraph.GetLastDocument(env.ctx, &env.api, env.DAO)
				assert.NilError(t, err)
				proposals[testIndex] = lastDoc

				fv, err := lastDoc.GetContent("title")
				assert.NilError(t, err)

				assert.Equal(t, fv.String(), test.title)

				ballot, err := lastDoc.GetContent("ballot_id")
				assert.NilError(t, err)
				ballots[testIndex] = ballot.Impl.(eos.Name)
			})
		}

		time.Sleep(10 * time.Second)

		for ballotIndex := range ballots {

			actions := []*eos.Action{
				{
					Account: env.TelosDecide,
					Name:    eos.ActN("castvote"),
					Authorization: []eos.PermissionLevel{
						{Actor: env.Whale, Permission: eos.PN("active")},
					},
					ActionData: eos.NewActionData(&Vote{
						Voter:      eos.Name(env.Whale),
						BallotName: ballots[ballotIndex],
						Options:    []eos.Name{"pass"},
					}),
				}}

			_, err := eostest.ExecTrx(env.ctx, &env.api, actions)
			assert.NilError(t, err)
		}

		time.Sleep(400 * time.Second)

		for proposalIndex := range proposals {

			actions := []*eos.Action{
				{
					Account: env.DAO,
					Name:    eos.ActN("closedocprop"),
					Authorization: []eos.PermissionLevel{
						{Actor: env.Members[0], Permission: eos.PN("active")},
					},
					ActionData: eos.NewActionData(&CloseDocProp{
						ProposalHash: proposals[proposalIndex].Hash,
					}),
				}}

			_, err := eostest.ExecTrx(env.ctx, &env.api, actions)
			assert.NilError(t, err)
		}
	})
}
