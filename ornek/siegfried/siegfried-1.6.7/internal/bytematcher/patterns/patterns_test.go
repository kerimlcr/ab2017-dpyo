package patterns_test

import (
	"testing"

	"github.com/richardlehane/siegfried/internal/persist"

	. "github.com/richardlehane/siegfried/internal/bytematcher/patterns"
	. "github.com/richardlehane/siegfried/internal/bytematcher/patterns/tests"
)

func TestContains(t *testing.T) {
	if !Contains(TestSequences[0], TestSequences[1]) {
		t.Error("Contains fail: equal sequences")
	}
	if !Contains(TestLists[0], TestLists[0]) {
		t.Error("Contains fail: equal lists")
	}
	if !Contains(TestSequences[2], TestSequences[1]) {
		t.Error("Contains fail: testy contains test")
	}
	if Contains(TestSequences[1], TestSequences[2]) {
		t.Error("Contains fail: test doesn't contain testy")
	}
}

func TestSequence(t *testing.T) {
	if !TestSequences[0].Equals(TestSequences[1]) {
		t.Error("Seq fail: Equality")
	}
	if r, _ := TestSequences[0].Test([]byte{'t', 'o', 'o', 't'}); r {
		t.Error("Sequence fail: shouldn't match")
	}
	if _, l := TestSequences[2].Test([]byte{'t', 'e', 's', 't', 'y'}); l != 5 {
		t.Error("Sequence fail: should match")
	}
	reverseSeq := TestSequences[2].Reverse()
	if reverseSeq[1] != 't' || reverseSeq[2] != 's' || reverseSeq[3] != 'e' || reverseSeq[4] != 't' {
		t.Error("Sequence fail: Reverse")
	}
	saver := persist.NewLoadSaver(nil)
	TestSequences[0].Save(saver)
	loader := persist.NewLoadSaver(saver.Bytes())
	_ = loader.LoadByte()
	p := loader.LoadBytes()
	if len(p) != 4 {
		t.Errorf("expecting %v, got %v", TestSequences[0], string(p))
	}
}

func TestChoice(t *testing.T) {
	if !TestChoices[0].Equals(TestChoices[1]) {
		t.Error("Choice fail: Equality")
	}
	if _, l := TestChoices[0].Test([]byte{'t', 'e', 's', 't'}); l != 4 {
		t.Error("Choice test fail: Test")
	}
	if TestChoices[0].NumSequences() != 2 {
		t.Error("Choice fail: NumSequences; expecting 2 got", TestChoices[0].NumSequences())
	}
	seqs := TestChoices[0].Sequences()
	if seqs[0][0] != 't' || seqs[1][0] != 't' {
		t.Error("Choice fail: Sequences; expecting t, t got ", seqs[0][0], seqs[1][0])
	}
}

func TestList(t *testing.T) {
	if TestLists[0].Equals(TestLists[1]) {
		t.Error("List fail: equality")
	}
	if _, l := TestLists[0].Test([]byte{'t', 'e', 's', 't', 't', 'e', 's', 't', 'y'}); l != 9 {
		t.Error("List test fail: Test")
	}
	if TestLists[0].NumSequences() != 1 {
		t.Error("List fail: NumSequences; expecting 1 got", TestLists[0].NumSequences())
	}
	seqs := TestLists[0].Sequences()
	if seqs[0][0] != 't' || seqs[0][8] != 'y' {
		t.Error("List fail: Sequences; expecting t, y got ", seqs[0][0], seqs[0][8])
	}
}

func TestNotSequence(t *testing.T) {
	if !TestNotSequences[0].Equals(TestNotSequences[1]) {
		t.Error("NotSequence fail: Equality test")
	}
	if r, _ := TestNotSequences[0].Test([]byte{'t', 'e', 's', 't'}); r {
		t.Error("NotSequence fail: Test")
	}
	if _, l := TestNotSequences[0].Test([]byte{'t', 'o', 'o', 't'}); l != 4 {
		t.Error("NotSequence fail: Test")
	}
	seqs := TestNotSequences[2].Sequences()
	if len(seqs) != 255 {
		t.Error("NotSequence fail: Sequences")
	}
	seqs = TestNotSequences[3].Sequences()
	if len(seqs) != 255 {
		t.Error("NotSequence fail: Sequences")
	}
	seqs = TestNotSequences[4].Sequences()
	if len(seqs) != 255 {
		t.Error("NotSequence fail: Sequences")
	}
}

func TestMask(t *testing.T) {
	mask := TestMasks[0]
	if r, _ := mask.Test([]byte{0xEE}); !r {
		t.Errorf("mask fail: 0xEE should match")
	}
	if r, _ := mask.Test([]byte{0x0A}); r {
		t.Errorf("mask fail: expected 0x0A not to match!")
	}
	num := mask.NumSequences()
	if num != 16 {
		t.Fatal("mask fail: expecting 16 sequences")
	}
	seqs := mask.Sequences()
	if len(seqs) != 16 {
		t.Fatal("mask fail: expecting 16 sequences")
	}
	for i, v := range seqs {
		if v[0] == 0xEE {
			break
		}
		if i == len(seqs)-1 {
			t.Fatal("mask fail: expecting 0xEE amongst sequences")
		}
	}
}

func TestAnyMask(t *testing.T) {
	amask := TestAnyMasks[0]
	if r, _ := amask.Test([]byte{0x0A}); !r {
		t.Errorf("any mask fail: 0x0A should match")
	}
	if r, _ := amask.Test([]byte{5}); r {
		t.Errorf("any mask fail: expected 5 not to match!")
	}
	num := amask.NumSequences()
	if num != 240 {
		t.Fatal("any mask fail: expecting 240 sequences")
	}
	seqs := amask.Sequences()
	if len(seqs) != 240 {
		t.Fatal("any mask fail: expecting 240 sequences")
	}
	for i, v := range seqs {
		if v[0] == 0x0A {
			break
		}
		if i == len(seqs)-1 {
			t.Fatal("any mask fail: expecting 0x0A amongst sequences")
		}
	}
}
