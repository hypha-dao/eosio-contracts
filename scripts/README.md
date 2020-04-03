
# Scripts commands:

Here are the options:
```
const optionDefinitions = [
    { name: "file", alias: "f", type: filename => new FileDetails(filename)  },
    { name: "host", alias: "h", type: String, defaultValue: "https://test.telos.kitchen" },
    { name: "approve", alias: "a", type: Boolean, defaultValue: false },
    { name: "close", alias: "c", type: Boolean, defaultValue: false },
    { name: "closepropid", type: String},
    { name: "propose", alias: "p", type: Boolean, defaultValue: false },
    { name: "config", type: Boolean, defaultValue: false },
    { name: "updstrings", type: Boolean, defaultValue: false},
    { name: "closeall", type: Boolean, defaultValue: false},
    { name: "print_proposal", alias: "x", type: String},
    { name: "prod", type: Boolean, defaultValue: false}
    // see here to add new options:
    //   - https://github.com/75lb/command-line-args/blob/master/doc/option-definition.md
  ];
```

When simulating votes on the testnet, it's best to set the voting period duration to 60 seconds since you have to wait. The scripts will automatically check this duration and wait before closing the proposal.

## Setting the Config
```
node dao.js -f payloads/config.json --config
```

## Making a proposal, approving it, and then closing it
```
node dao.js -f payloads/contribution-test1.json --propose --approve --close
```

There are many test payloads in tests/proposals/tests.

## Print a proposal
```
node dao.js --print_proposal 3
```

## Running in production
You can also run the queries in production by changing the host.