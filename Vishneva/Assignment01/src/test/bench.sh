#!/bin/bash
BIN=${1:?path to minirt-test-openmp}
OUT=${2:-results.csv}
W=${W:-1200}
H=${H:-1200}
S=${S:-8}
RUNS=${RUNS:-6}
SCENES=${SCENES:--}
THREADS=${THREADS:-"1 2 4 8 12 16"}

echo "scene,mode,schedule,threads,run,time" > "$OUT"
for scene in $SCENES; do
  name=$(basename "$scene" .txt); [ "$scene" = "-" ] && name=default
  for cfg in "plain static" "plain static,1" "plain dynamic,1" "plain dynamic,16" "plain guided" "collapse static" "collapse dynamic,16"; do
    set -- $cfg
    mode=$1; sched=$2
    for th in $THREADS; do
      for r in $(seq 1 "$RUNS"); do
        t=$(OMP_NUM_THREADS=$th OMP_SCHEDULE="$sched" "$BIN" "$W" "$H" "$S" "$scene" "$mode" | sed -n 's/^Time = //p')
        echo "$name,$mode,${sched//,/:},$th,$r,$t" >> "$OUT"
      done
    done
  done
done
