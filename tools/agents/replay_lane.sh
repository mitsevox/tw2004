#!/bin/bash
# replay_lane.sh <lane> <base> <gfirst|glast> <globals.tsv|-> <batch.tsv>...
# Replays one lane's work on main (the orchestrator's step; agents/README.md):
#   0. check_batches.py on the lane's files: a malformed row or a name clash stops everything;
#   1. the globals file (before or after the batches) with rename.py, then each name.py batch, each
#      its own commit; a refused row, a failed rename or a build that is not OK stops the replay;
#   2. the lane's hand edits with merge_lane.sh (BASE=<base>: the lane's LAST name.py / globals commit);
#      a conflict stops the replay (resolve with merge_pick.py, then re-run from step 3 by hand);
#   3. the reference refresh (skipped with NOREFS=1: then run it once after the round's last lane),
#      wraplong until clean, build, lint since the start;
#   4. lanediff.py: every line where main still differs from the lane, names aside. Read them all:
#      a lint fix the lane made inside a batch commit shows up here and must be carried over.
# The merged hand edits are left uncommitted for review. Exit status: 0 done, 1 stopped.
set -u
cd "$(git rev-parse --show-toplevel)"
L=$1; B=$2; ORD=$3; G=$4; shift 4
START=$(git rev-parse HEAD)
stop() { echo "STOPPED: $*"; exit 1; }
ok() { sha1sum -c config/GW4E69/build.sha1 >/dev/null 2>&1 || stop "build not OK after $1"; }
FILES=("$@"); [ "$G" = - ] || FILES+=("$G")
python tools/agents/check_batches.py "${FILES[@]}" || stop "check_batches.py found problems"
glob() {
  [ "$G" = - ] && return
  out=$(python tools/match/rename.py "$G" 2>&1) || { echo "$out" | tail -5; stop "rename.py refused $G"; }
  echo "$out" | tail -1
  ninja >/dev/null 2>&1; ok globals
  git add -A && git commit -q -m "globals: $L's data names (replayed by orchestrator)"
}
[ "$ORD" = gfirst ] && glob
for b in "$@"; do
  out=$(python tools/match/name.py "$b" --by "$L (replayed by orchestrator)" 2>&1) \
    || { echo "$out" | tail -8; stop "name.py refused $b"; }
  echo "$out" | grep -E 'call sites|long-line' | tail -3
  ok "$b"
  git add -A && git commit -q -m "names: $L $(basename "$b" .tsv) (replayed by orchestrator)"
done
[ "$ORD" = glast ] && glob
if [ "$G" = - ]; then out=$(BASE=$B tools/agents/merge_lane.sh "$L" 2>&1)
else out=$(BASE=$B tools/agents/merge_lane.sh "$L" "$G" 2>&1); fi
echo "$out" | tail -8
echo "$out" | grep -q CONFLICTS && stop "hand-edit conflicts: resolve them (merge_pick.py), then refs, wraplong, build, lanediff"
[ -n "${NOREFS:-}" ] || python tools/match/rename.py config/GW4E69/name_sources.tsv --refs-only 2>&1 | tail -1
for i in $(seq 1 12); do python tools/match/wraplong.py --from-lint --diff HEAD >/dev/null 2>&1; done
python tools/match/wraphdr.py --from-lint --diff HEAD
ninja >/dev/null 2>&1; ok merge
echo "build OK; lint since start: $(python tools/match/lint.py --diff $START 2>&1 | tail -1)"
if [ "$G" = - ]; then python tools/agents/lanediff.py "$L"; else python tools/agents/lanediff.py "$L" "$G"; fi
exit 0
