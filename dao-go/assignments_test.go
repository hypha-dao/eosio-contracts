package dao_test

import "testing"

func TestAssignmentProposalDocument(t *testing.T) {
	teardownTestCase := setupTestCase(t)
	defer teardownTestCase(t)

	// var env Environment
	env = SetupEnvironment(t)

	// roles
	proposer := env.Members[0]
	assignee := env.Members[1]
	closer := env.Members[2]

	t.Run("Configuring the DAO environment: ", func(t *testing.T) {
		t.Log(env.String())
		t.Log("\nDAO Environment Setup complete\n")
	})

	t.Run("Test Assignment Document Proposal", func(t *testing.T) {

		// call propose with a role proposal
		// vote on the proposal
		// close the proposal
		// ensure that the proposal closed and the appropriate edges exist

	})
}

const assignment1 = `{
    "scope": "proposal",
    "names": [
      {
        "key": "type",
        "value": "assignment"
      },
      {
        "key": "trx_action_name",
        "value": "assign"
      }
    ],
    "strings": [
      {
        "key": "title",
        "value": "Underwater Basketweaver - Atlantic"
      },
      {
        "key": "description",
        "value": "Weave baskets at the bottom of the sea - Atlantic Ocean"
      },
      {
        "key": "url",
        "value": "null"
      }
    ],
    "assets": [
      {
        "key": "annual_usd_salary",
        "value": "150000.00 USD"
      }
    ],
    "time_points": [],
    "ints": [
      {
        "key": "start_period",
        "value": 0
      },
      {
        "key": "end_period",
        "value": 9
      },
      {
        "key": "time_share_x100",
        "value": 100
      },
      {
        "key": "deferred_perc_x100",
        "value": 50
      }
    ],
    "floats": [],
    "trxs": []
  }`

const assignment1_document = `{
    "content_groups": [
        [
            {
                "label": "content_group_label",
                "value": [
                    "string",
                    "details"
                ]
            },
            {
                "label": "title",
                "value": [
                    "string",
                    "Underwater Basketweaver - Atlantic"
                ]
            },
            {
                "label": "description",
                "value": [
                    "string",
                    "Weave baskets at the bottom of the sea - Atlantic Ocean"
                ]
            },
            {
              "label": "url",
              "value": [
                  "string",
                  "https://dho.hypha.earth"
              ]
            },
            {
                "label": "annual_usd_salary",
                "value": [
                    "asset",
                    "150000.00 USD"
                ]
            },
            {
                "label": "start_period",
                "value": [
                    "int64",
                    0
                ]
            },
            {
                "label": "end_period",
                "value": [
                    "int64",
                    9
                ]
            },
            {
                "label": "time_share_x100",
                "value": [
                    "int64",
                    100
                ]
            },
            {
                "label": "deferred_perc_x100",
                "value": [
                    "int64",
                    50
                ]
            }
        ]
    ]
}`
