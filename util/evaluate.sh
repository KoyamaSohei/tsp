#!/bin/bash

a=("ni" "nn" "fi" "greedy")
b=("2opt" "sa" "tabu" "combopt")
c=("ulysses22" "pr107" "a280" "u574" "u1060" "u2319" "rl5915" "usa13509")
d=("2" "20" "200")

for k in "${c[@]}"
do
  eval "gzip -d lib/${k}.tsp.gz"
done

for i in "${a[@]}"
do
  for j in "${b[@]}"
  do
    echo "build ${i}_${j}.cc"
    eval "g++ -O2 -I/usr/include/X11/ -L/usr/lib/ -lX11 -lm util/tspShow.c ${i}_${j}.cc -o ${i}_${j}.out"
    for k in "${c[@]}"
    do
      echo "build: ${i} improve: ${j} target: ${k}"
      for l in "${d[@]}"
      do
        export TIME_LIMIT=$l
        eval "time ./${i}_${j}.out -n -c lib/${k}.tsp | ./export.out > dist/${k}.${i}_${j}.${l}s.svg"
        sleep 3
      done
    done
  done
done