#!/usr/bin/env python3
"""Two-way guard: doc/test-catalog.html and configure.json agree.

Reads the catalog structurally -- it collects the text of every element
carrying class="rec" (a record name) or class="prog" (a program name),
whatever the tag -- and asserts set equality against configure.json:

    {rec elements}  == set(configure.json keys)
    {prog elements} == {program field, else the record key}

Set equality, not containment. A record added to configure.json but absent
from the catalog fails, and so does a record removed from configure.json but
left behind in the catalog (the phantom-row case a one-directional check
silently passes).

Contract for a name cell: any element carrying class="rec" or class="prog",
whatever the tag, must hold exactly one bare name as its text. An element
left open at end of file fails the guard -- its text is not silently
dropped. The class names are reserved for names only: no other element in
doc/ uses them, so a count can never be read as a record name.

Substring matching is deliberately NOT used: 22 record names are proper
substrings of other record names (cfi-call-fake-vtable is a prefix of
cfi-call-fake-vtable-arg-num) and 154 program names are substrings of record
names, so a page that dropped a row could still textually "contain" its name
inside a sibling's.

Count figures. Every published count derives from the registry and is
asserted against it:

  * doc/test-catalog.html -- the five "<programs> programs, <records>
    records" sentences under the per-category headings.

A category is the directory prefix of the record's `program` field (the
record key when the field is absent). Keying on the record key instead
would misgroup acc-copy-stackra-to-heap-{explicit,implicit}-arith, whose
programs are cfi-jump-func-ra-from-heap-memcpy-* and which the catalog
lists under cfi.

Usage (runs from any working directory):
    python3 script/check_catalog.py [path-to-catalog-page]

The page defaults to <repo>/doc/test-catalog.html; the repo root is
resolved from this script's own location, not the current directory.

Exit 0 on pass, 1 on failure, 2 on internal error (an unreadable or
malformed input fails as one stderr line, never a traceback). Stdlib only.
Idempotent.
"""

import json
import os
import re
import sys
from html.parser import HTMLParser

REPO_ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
CONFIG = os.path.join(REPO_ROOT, "configure.json")
DEFAULT_CATALOG = os.path.join(REPO_ROOT, "doc", "test-catalog.html")

BUCKETS = ("rec", "prog")
CATEGORIES = ("acc", "cfi", "cpi", "mss", "mts")


class CellCollector(HTMLParser):
    """Collect the text of every element carrying class 'rec' or 'prog'.

    Tag-agnostic: the catalog may use <td class="prog"> for a column and
    <span class="rec"> for inline record lists. Text is attributed to the
    innermost tracked element only, so accidental nesting cannot
    double-count.
    """

    def __init__(self):
        super().__init__(convert_charrefs=True)
        self.buckets = {b: [] for b in BUCKETS}
        self._stack = []          # [tag, bucket, buffer] for tracked elements
        self.malformed = []       # entries holding anything but one bare name

    def handle_starttag(self, tag, attrs):
        classes = (dict(attrs).get("class") or "").split()
        bucket = next((c for c in classes if c in BUCKETS), None)
        if bucket is not None:
            self._stack.append([tag, bucket, []])

    def handle_startendtag(self, tag, attrs):
        self.handle_starttag(tag, attrs)
        self.handle_endtag(tag)

    def handle_endtag(self, tag):
        if self._stack and self._stack[-1][0] == tag:
            _, bucket, buf = self._stack.pop()
            text = "".join(buf).strip()
            self.buckets[bucket].append(text)
            if len(text.split()) != 1:
                self.malformed.append((bucket, text[:80]))

    def handle_data(self, data):
        if self._stack:
            self._stack[-1][2].append(data)

    def unclosed(self):
        """Tracked elements still open at end of input, as (tag, bucket, text)."""
        return [(tag, bucket, "".join(buf).strip()[:80])
                for tag, bucket, buf in self._stack]


def fail(msg):
    print(msg, file=sys.stderr)
    sys.exit(1)


def report_figure(problems, where, want, found):
    """Compare a registry-derived (records, programs) figure with the page's."""
    if want == found:
        return
    shown = ("nothing" if found is None
             else "%d programs, %d records" % (found[1], found[0]))
    problems.append("%s: registry-derived figure is %d programs, %d records; page has %s"
                    % (where, want[1], want[0], shown))


def load_registry():
    """Load configure.json and derive the per-category counts."""
    with open(CONFIG, encoding="utf-8") as fh:
        config = json.load(fh)
    if not isinstance(config, dict):
        raise TypeError("configure.json: top level is not a JSON object")

    records = dict.fromkeys(CATEGORIES, 0)
    programs = {c: set() for c in CATEGORIES}
    for key, value in config.items():
        if not isinstance(value, dict):
            raise TypeError("configure.json: record %r is not a JSON object" % key)
        program = value.get("program", key)
        if not isinstance(program, str):
            raise TypeError("configure.json: record %r has a non-string program field" % key)
        category = program.split("-", 1)[0]
        if category not in programs:
            raise TypeError("configure.json: record %r names program %r outside the known categories"
                            % (key, program))
        records[category] += 1
        programs[category].add(program)
    return config, records, programs


def check_counts(catalog, catalog_text, records, programs, problems):
    """Assert the published count figures against the registry-derived ones."""
    # doc/test-catalog.html: the per-category "N programs, M records" sentences.
    sentences = {cat: (int(recs), int(progs)) for cat, progs, recs in re.findall(
        r'<h2>[^<>]*\(<code>(acc|cfi|cpi|mss|mts)/</code>\)</h2>\s*<p>(\d+) programs, (\d+) records',
        catalog_text)}
    for cat in CATEGORIES:
        report_figure(problems, "%s, section %s/" % (catalog, cat),
                      (records[cat], len(programs[cat])), sentences.get(cat))


def check(catalog):
    for path in (CONFIG, catalog):
        if not os.path.isfile(path):
            print("internal error: %s not found" % path, file=sys.stderr)
            sys.exit(2)

    config, records, programs = load_registry()

    parser = CellCollector()
    with open(catalog, encoding="utf-8") as fh:
        catalog_text = fh.read()
    parser.feed(catalog_text)
    parser.close()

    # run-test.cpp case_parser(): the program is the explicit 'program' field,
    # otherwise the record key itself.
    want_recs = set(config)
    want_progs = set().union(*programs.values())
    got_recs = set(parser.buckets["rec"])
    got_progs = set(parser.buckets["prog"])

    problems = []
    if parser.malformed:
        problems.append("%d cell(s) do not hold exactly one bare name, e.g. %r"
                        % (len(parser.malformed), parser.malformed[0]))
    unclosed = parser.unclosed()
    if unclosed:
        tag, bucket, text = unclosed[0]
        problems.append("%d classed element(s) left open at end of %s, e.g. <%s class=\"%s\"> holding %r"
                        % (len(unclosed), catalog, tag, bucket, text))
    for label, cells, distinct in (("record", parser.buckets["rec"], got_recs),
                                   ("program", parser.buckets["prog"], got_progs)):
        if len(cells) != len(distinct):
            dupes = sorted(n for n in distinct if cells.count(n) > 1)
            problems.append("%s cells: %d classed cell(s) but only %d distinct name(s); duplicated: %s"
                            % (label, len(cells), len(distinct), ", ".join(dupes)))
    for label, want, got in (("record", want_recs, got_recs),
                             ("program", want_progs, got_progs)):
        missing = sorted(want - got)
        extra = sorted(got - want)
        if missing:
            problems.append("%s set: %d missing name(s): %s"
                            % (label, len(missing), ", ".join(missing)))
        if extra:
            problems.append("%s set: %d extra name(s): %s"
                            % (label, len(extra), ", ".join(extra)))

    check_counts(catalog, catalog_text, records, programs, problems)

    if problems:
        print("FAIL: catalog/config disagreement —", file=sys.stderr)
        for p in problems:
            print("  - " + p, file=sys.stderr)
        sys.exit(1)

    print("catalog set-equality OK: %d/%d records and %d/%d programs match "
          "configure.json in both directions (no missing, no extra)"
          % (len(got_recs), len(want_recs), len(got_progs), len(want_progs)))
    print("count figures OK: %s; totals %d records / %d programs"
          % (", ".join("%s %d/%d" % (c, records[c], len(programs[c])) for c in CATEGORIES),
             sum(records.values()), len(want_progs)))
    return 0


def main():
    catalog = sys.argv[1] if len(sys.argv) > 1 else DEFAULT_CATALOG
    try:
        return check(catalog)
    except Exception as exc:
        print("internal error: %s: %s" % (type(exc).__name__, exc), file=sys.stderr)
        return 2


if __name__ == "__main__":
    sys.exit(main())
