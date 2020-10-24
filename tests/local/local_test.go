package local_test

import (
	"log"
	"testing"

	"gotest.tools/assert"
)

func TestSimple(t *testing.T) {

	env := SetupEnvironment(t)
	t.Run("a label for the test", func(t *testing.T) {
		log.Println()
		log.Println(env.String())

	})

	t.Run("a second label", func(t *testing.T) {

	})

	assert.Equal(t, 1, 1)
}
