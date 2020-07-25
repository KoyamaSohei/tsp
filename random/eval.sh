a=("bb" "dp")

for i in "${a[@]}"
do
  echo "build ${i}.cc"
  eval "g++ -O2 -I/usr/include/X11/ -L/usr/lib/ -lX11 -lm util/tspShow.c ${i}.cc -o ${i}.out"
  for j in {10..38}
  do
    echo "target: ${j}"
    eval "time ./${i}.out -n -c random/v${j}.tsp | ./export.out > dist/v${j}.${i}.svg"
    sleep 3
  done
done

