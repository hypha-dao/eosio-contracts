# Root node
The root node is the DHO document itself.  It is a document in the document table with content that identifies the contract (get_self()) as the root_node.  This creates a hash, which is used in the edges table to connect the DHO to all of its data.

The hash of the root node is saved in the config table of dao.hypha.

To generate the hash in the config table, call "createroot".

```
 eosc -u https://testnet.telos.caleos.io --vault-file ../eosc-testnet-vault.json tx create dao.hypha createroot '{"notes":"creating the root node"}' -p dao.hypha
```

### Check root node document
Retrieve the hash from the config table.
```
cleos -u https://test.telos.kitchen get table dao.hypha dao.hypha config
```

### Check the document contents
```
cleos -u https://test.telos.kitchen get table -l 1 --index 2 --key-type sha256 -L 19d212d1cfec20561334884c556757914b1a31e9af93e8e4029f5209981784aa dao.hypha dao.hypha documents
```

Or use the JS utility:
```
node ../document/js/index.js --get --hash b1ba4f3a5294212fc68d83ec0cac1873f7dea380dec19a15994d63b16c52b49d --contract dao.hypha
```

# Member nodes
If the DHO has existing members in the members table, there needs to be a document created for each one.  Run "makememdocs".

```
 eosc -u https://testnet.telos.caleos.io --vault-file ../eosc-testnet-vault.json tx create dao.hypha makememdocs '{"notes":"making mem docs"}' -p dao.hypha
```

## Checking 

# Erasing docs 
In test environments, there is an action to remove the entire graph, including all documents and edges.
```
 eosc -u https://test.telos.kitchen --vault-file ../eosc-testnet-vault.json tx create dao.hypha erasealldocs '{"notes":"erasing all docs"}' -p dao.hypha
```

You can also erase docs one by one via hash:
```
eosc -u https://test.telos.kitchen --vault-file ../eosc-testnet-vault.json tx create dao.hypha erasedochash '{"doc":"41e04c97bfd074751f356f7927cfe9a7db35ae783455f78841ea1be9cb94fc3c"}' -p dao.hypha
```