package testnet

import (
	"context"
	"log"

	eos "github.com/eoscanada/eos-go"
	"github.com/hypha-dao/document/docgraph"
)

// GetLastDocument retrieves the last document that was created from the contract
func GetLastDocument(ctx context.Context, api *eos.API, contract eos.AccountName) (docgraph.Document, error) {
	var docs []docgraph.Document
	var request eos.GetTableRowsRequest
	request.Code = string(contract)
	request.Scope = string(contract)
	request.Table = "documents"
	request.Reverse = true
	request.Limit = 1
	request.JSON = true
	response, err := api.GetTableRows(ctx, request)
	if err != nil {
		log.Println("Error with GetTableRows: ", err)
		return docgraph.Document{}, err
	}

	err = response.JSONToStructs(&docs)
	if err != nil {
		log.Println("Error with JSONToStructs: ", err)
		return docgraph.Document{}, err
	}
	return docs[0], nil
}

// GetEdges retrieves all edges from contract (up to 1000)
func GetEdges(ctx context.Context, api *eos.API, contract eos.AccountName) ([]docgraph.Edge, error) {
	var edges []docgraph.Edge
	var request eos.GetTableRowsRequest
	request.Code = string(contract)
	request.Scope = string(contract)
	request.Table = "edges"
	request.Limit = 1000
	request.JSON = true
	response, err := api.GetTableRows(ctx, request)
	if err != nil {
		log.Println("Error with GetTableRows: ", err)
		return []docgraph.Edge{}, err
	}

	err = response.JSONToStructs(&edges)
	if err != nil {
		log.Println("Error with JSONToStructs: ", err)
		return []docgraph.Edge{}, err
	}
	return edges, nil
}
