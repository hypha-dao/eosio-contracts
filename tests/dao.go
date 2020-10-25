
import "github.com/eoscanada/eos-go"

// configVariant may hold a name, int64, asset, string, or time_point
var configVariant = eos.NewVariantDefinition([]eos.VariantType{
	{Name: "name", Type: eos.Name("")},
	{Name: "uint64", Type: uint64(0)},
	{Name: "asset", Type: (*eos.Asset)(nil)}, // Syntax for pointer to a type, could be any struct
	{Name: "string", Type: ""},
	{Name: "time_point", Type: eos.TimePoint(0)},
	{Name: "checksum256", Type: eos.Checksum256([]byte("0"))},
})

type configValue struct {
	eos.BaseVariant
}

// MarshalJSON translates to []byte
func (fv *configValue) MarshalJSON() ([]byte, error) {
	return fv.BaseVariant.MarshalJSON(configVariant)
}

// UnmarshalJSON translates flexValueVariant
func (fv *configValue) UnmarshalJSON(data []byte) error {
	return fv.BaseVariant.UnmarshalJSON(data, configVariant)
}

// UnmarshalBinary ...
func (fv *configValue) UnmarshalBinary(decoder *eos.Decoder) error {
	return fv.BaseVariant.UnmarshalBinaryVariant(decoder, configVariant)
}
