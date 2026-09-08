#!/bin/bash
# usage: join_anchors.sh OURS_ANCHORS GB_ANCHORS -> pc ours gb diff step (first hits), sorted by ours time
awk 'NR==FNR { if ($1=="ANCHOR" && !($2 in g)) g[$2]=$3; next } $1=="ANCHOR" && !($2 in o) { o[$2]=$3; if ($2 in g) print $2, $3, g[$2] }' "$2" "$1" | sort -k2,2n | awk '{d=$2-$3; print $1, $2, $3, "diff", d, "step", d-last; last=d}'
