# Get Badges from Blockchain
To retrieve badges directly from the blockchain, you can query the 'badge' edge name, which is reserved for approved badges.  To be sure, each record should have the from_node as the root node, but we cannot verify this in the query because we only have one index to use.

``` json
cleos -u https://test.telos.kitchen get table -l 100 --index 4 --key-type i64 -L badge -U badge dao.hypha dao.hypha edges
```

> NOTE: the preferred query for data will come from DGraph of course


## Step 1: Propose a new badge:
``` json
eosc -u https://testnet.telos.caleos.io --vault-file ../eosc-testnet-vault.json tx create dao.hypha propose '{                                                                                                                                        
    "proposer": "johnnyhypha1",
    "proposal_type": "badge",
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
                    "Healer"
                ]
            },
            {
                "label": "description",
                "value": [
                    "string",
                    "Holder of indigenous wisdom ready to transfer the knowledge to others willing to receive"
                ]
            },
            {
                "label": "icon",
                "value": [
                    "string",
                    "https://assets.hypha.earth/badges/badge_explorer.png?X-Amz-Algorithm=AWS4-HMAC-SHA256&X-Amz-Credential=minioadmin%2F20201007%2F%2Fs3%2Faws4_request&X-Amz-Date=20201007T153744Z&X-Amz-Expires=432000&X-Amz-SignedHeaders=host&X-Amz-Signature=78194bf2d97352305f9dd4f1214da5cba13e39289965143b44f31325f228992d"
                ]
            },
            {
                "label": "seeds_coefficient_x10000",
                "value": [
                    "int64",
                    10010
                ]
            },
            {
                "label": "hypha_coefficient_x10000",
                "value": [
                    "int64",
                    10015
                ]
            },
            {
                "label": "hvoice_coefficient_x10000",
                "value": [
                    "int64",
                    10000
                ]
            }, {
                "label": "husd_coefficient_x10000",
                "value": [
                    "int64",
                    10100
                ]
            }
        ]
    ]
}' -p johnnyhypha1
```

### Check document
Retrieve the last created document (which would be the proposal)
```
cleos -u https://test.telos.kitchen get table -r -l 1 dao.hypha dao.hypha documents
{
  "rows": [{
      "id": 37,
      "hash": "41e04c97bfd074751f356f7927cfe9a7db35ae783455f78841ea1be9cb94fc3c",
      "creator": "dao.hypha",
      "content_groups": [[{
            "label": "content_group_label",
            "value": [
              "string",
    <snip>
```

The hash above is the hash associated with this newly created badge proposal.

### Check edges
Check the newly created edges:
```
cleos -u https://test.telos.kitchen get table -r -l 2 dao.hypha dao.hypha edges


```

Verify the details of the nodes on each of the newly created edges.

node ../document/js/index.js --get --hash 61aaf984e245d887db702d96edbca69a2cf536f6b2375af959abcfbeb19e8fe0 --contract dao.hypha
