package dao_test

import (
	"testing"
)

func TestRoleProposalDocument(t *testing.T) {
	teardownTestCase := setupTestCase(t)
	defer teardownTestCase(t)

	// var env Environment
	env = SetupEnvironment(t)

	// roles
	proposer := env.Members[0]
	// assignee := env.Members[1]
	closer := env.Members[2]

	t.Run("Configuring the DAO environment: ", func(t *testing.T) {
		t.Log(env.String())
		t.Log("\nDAO Environment Setup complete\n")
	})

	t.Run("Test Roles Document Proposal", func(t *testing.T) {

		tests := []struct {
			name  string
			title string
			role  string
		}{
			{
				name:  "Basketweaver",
				title: "Underwater Basketweaver",
				role:  role1_document,
			},
		}

		// call propose with a role proposal
		// vote on the proposal
		// close the proposal
		// ensure that the proposal closed and the appropriate edges exist
		for _, test := range tests {

			t.Log("\n\nStarting test: ", test.name)
			CreateRole(t, env, proposer, closer, test.title, test.role)

		}
	})
}

const role1 = `{
    "scope": "proposal",
    "names": [
      {
        "key": "type",
        "value": "role"
      },
      {
        "key": "trx_action_name",
        "value": "newrole"
      }
    ],
    "strings": [
      {
        "key": "title",
        "value": "Underwater Basketweaver"
      },
      {
        "key": "description",
        "value": "Weave baskets at the bottom of the sea"
      },
      {
        "key": "content",
        "value": "We make *great* baskets."
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
        "key": "fulltime_capacity_x100",
        "value": 100
      },
      {
        "key": "min_deferred_x100",
        "value": 50
      }
    ],
    "floats": [],
    "trxs": []
  }`

const role2 = `{
	"scope": "proposal",
	"names": [
	  {
		"key": "type",
		"value": "role"
	  },
	  {
		"key": "trx_action_name",
		"value": "newrole"
	  }
	],
	"strings": [
	  {
		"key": "title",
		"value": "Test 2"
	  },
	  {
		"key": "description",
		"value": "Weave baskets at the bottom of the sea"
	  },
	  {
		"key": "content",
		"value": "We make *great* baskets."
	  }
	],
	"assets": [
	  {
		"key": "annual_usd_salary",
		"value": "75000.00 USD"
	  }
	],
	"time_points": [],
	"ints": [
	  {
		"key": "start_period",
		"value": 1
	  },
	  {
		"key": "end_period",
		"value": 9
	  },
	  {
		"key": "fulltime_capacity_x100",
		"value": 120
	  },
	  {
		"key": "min_deferred_x100",
		"value": 50
	  }
	],
	"floats": [],
	"trxs": []
  }`

const role1_document = `{
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
                    "Underwater Basketweaver"
                ]
            },
            {
                "label": "description",
                "value": [
                    "string",
                    "Weave baskets at the bottom of the sea"
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
                      "label": "fulltime_capacity_x100",
                      "value": [
                          "int64",
                          100
                      ]
            },
            {
                "label": "min_deferred_x100",
                "value": [
                    "int64",
                    50
                ]
            }
        ]
    ]
}`
