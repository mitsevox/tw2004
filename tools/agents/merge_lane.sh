#!/bin/bash
# merge_lane.sh <lane> [globals.tsv ...]: 3-way merges every src/include file the lane changed onto
# the working tree. BASE=<commit> overrides the fork point: use the lane's LAST name.py commit
# (every commit after it, hand edits and globals alike, then comes over; globals are normalized).
# Conflicts: read them; tools/agents/merge_take_theirs.py <conflict> <dest> takes the lane's side.
# (base = the lane's fork point; both sides name-normalized with norm.py), so only
# its hand edits come over. Clean merges are written in place; conflicts go to $W/<file>.conflict.
W=${MERGE_SCRATCH:-/home/user/scratch/tw/agents/merge}; mkdir -p $W; lane=$1; shift
T=$(dirname "$0")
base=${BASE:-$(git merge-base main agent/$lane)}
for f in $(git diff --name-only $base agent/$lane -- src include); do
  git show $base:$f > $W/b 2>/dev/null || : > $W/b
  git show agent/$lane:$f > $W/t
  python $T/merge_norm.py $W/b "$@"; python $T/merge_norm.py $W/t "$@"
  git merge-file -p --diff3 $f $W/b $W/t > $W/o; rc=$?
  if [ $rc -eq 0 ]; then cmp -s $W/o $f || { cp $W/o $f; echo "merged: $f"; }
  else n=$(basename $f); cp $W/o $W/$n.conflict; echo "CONFLICTS ($rc): $f -> $W/$n.conflict"; fi
done
