// Copyright 2016 Richard Lehane. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

package xmlmatcher

import (
	"strings"
	"testing"

	"github.com/richardlehane/siegfried/internal/siegreader"
	"github.com/richardlehane/siegfried/pkg/core"
)

var (
	testSet = SignatureSet{
		{"MD_metadata", ""},
		{"MD_metadata", "http://www.isotc211.org/2005/gmd"},
		{"", "http://purl.org/rss/1.0/"},
	}
	testCases = []struct {
		name   string
		val    string
		expect []int
	}{
		{"notXML", "this is not xml!", []int{}},
		{"mdXML", "<MD_metadata>bla bla", []int{0}},
		{"mdXMLns", "<MD_metadata xmlns='http://www.isotc211.org/2005/gmd'>bla bla", []int{1, 0}},
		{"rssXMLns", "<atom xmlns='http://purl.org/rss/1.0/'>", []int{2}},
	}
)

func TestAdd(t *testing.T) {
	_, i, err := Add(nil, testSet, nil)
	if err != nil || i != 3 {
		t.Errorf("expecting no errors and three signatures added, got %v and %d", err, i)
	}

}

func identifyString(m Matcher, s string) ([]core.Result, error) {
	rdr := strings.NewReader(s)
	bufs := siegreader.New()
	buf, _ := bufs.Get(rdr)
	res, err := m.Identify("", buf)
	if err != nil {
		return nil, err
	}
	ret := []core.Result{}
	for r := range res {
		ret = append(ret, r)
	}
	return ret, nil
}

func TestIdentify(t *testing.T) {
	m, i, e := Add(nil, testSet, nil)
	if i != 3 || e != nil {
		t.Fatal("failed to create matcher")
	}
	for _, tc := range testCases {
		res, err := identifyString(m.(Matcher), tc.val)
		if err != nil {
			t.Fatalf("error identifying %s: %v", tc.name, err)
		}
		if len(res) == len(tc.expect) {
			for i := range res {
				if res[i].Index() != tc.expect[i] {
					t.Errorf("bad results for %s: got index %d, expected %d, basis %s", tc.name, res[i].Index(), tc.expect[i], res[i].Basis())
				}
			}
		} else {

			t.Errorf("bad results for %s: got %d results, expected %d %s %d %s %d ", tc.name, len(res), len(tc.expect), res[0].Basis(), res[0].Index(), res[1].Basis(), res[1].Index())
		}
	}
}
