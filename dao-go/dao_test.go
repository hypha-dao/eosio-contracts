package dao_test

import (
	"encoding/json"
	"io/ioutil"
	"os"
	"os/exec"
	"testing"
	"time"

	"github.com/eoscanada/eos-go"
	"github.com/hypha-dao/dao-go"
	"github.com/hypha-dao/document/docgraph"
	"gotest.tools/assert"
)

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

func TestTelosDecideProposals(t *testing.T) {

	teardownTestCase := setupTestCase(t)
	defer teardownTestCase(t)

	// var env Environment
	env := SetupEnvironment(t)
	t.Run("Setting up the DAO environment: ", func(t *testing.T) {
		t.Log(env.String())
	})

	t.Run("Badges proposals", func(t *testing.T) {
		tests := []struct {
			name  string
			input string
			title string
			hash  eos.Checksum256
		}{
			{
				name:  "enroller",
				title: "Enroller",
				input: "../scripts/payloads/badges/enroller_badge_proposal.json",
			},
			{
				name:  "librarian",
				title: "Badge Librarian",
				input: "../scripts/payloads/badges/librarian_badge_proposal.json",
			},
			{
				name:  "treasurer",
				title: "Treasurer",
				input: "../scripts/payloads/badges/treasurer_badge_proposal.json",
			},
		}

		badges := make([]docgraph.Document, len(tests))
		ballots := make([]eos.Name, len(tests))

		for testIndex, test := range tests {
			t.Run(test.name, func(t *testing.T) {

				_, err := dao.ProposeBadgeFromFile(env.ctx, &env.api, env.DAO, env.Members[0], test.input)
				assert.NilError(t, err)

				// unmarshal JSON into a Document
				data, err := ioutil.ReadFile(test.input)
				assert.NilError(t, err)
				var documentFromFile docgraph.Document
				err = json.Unmarshal(data, &documentFromFile)
				assert.NilError(t, err)

				lastDoc, err := docgraph.GetLastDocument(env.ctx, &env.api, env.DAO)
				assert.NilError(t, err)
				badges[testIndex] = lastDoc
				tests[testIndex].hash = lastDoc.Hash

				fv, err := lastDoc.GetContent("title")
				assert.NilError(t, err)

				assert.Equal(t, fv.String(), test.title)

				ballot, err := lastDoc.GetContent("ballot_id")
				assert.NilError(t, err)
				ballots[testIndex] = ballot.Impl.(eos.Name)
			})
		}

		time.Sleep(100 * time.Millisecond)
		for ballotIndex := range ballots {
			_, err := dao.TelosDecideVote(env.ctx, &env.api, env.TelosDecide,
				env.Whale, ballots[ballotIndex], eos.Name("pass"))

			assert.NilError(t, err)
		}

		time.Sleep(62 * time.Second)
		for proposalIndex := range badges {
			_, err := dao.CloseProposal(env.ctx, &env.api, env.DAO, env.Members[2], badges[proposalIndex].Hash)
			assert.NilError(t, err)
		}

		time.Sleep(1 * time.Second)
		t.Log("Submit badge assignment proposal for ")

		_, err := dao.ProposeBadgeAssignment(env.ctx, &env.api, env.DAO, env.Members[2],
			badges[0].Hash, "../scripts/payloads/badges/enroller_badge_assignment.json")
		assert.NilError(t, err)

		lastDoc, err := docgraph.GetLastDocument(env.ctx, &env.api, env.DAO)
		assert.NilError(t, err)

		ballot, err := lastDoc.GetContent("ballot_id")
		assert.NilError(t, err)

		time.Sleep(1 * time.Second)
		_, err = dao.TelosDecideVote(env.ctx, &env.api, env.TelosDecide,
			env.Whale, ballot.Impl.(eos.Name), eos.Name("pass"))
		assert.NilError(t, err)

		time.Sleep(61 * time.Second)
		_, err = dao.CloseProposal(env.ctx, &env.api, env.DAO, env.Members[2], lastDoc.Hash)
		assert.NilError(t, err)
	})
}
