a=("bb" "dp")
b=("v12" "v24" "v26" "v32" "v36")

for i in "${a[@]}"
do
  echo "build ${i}.cc"
  eval "g++ -O2 -I/usr/include/X11/ -L/usr/lib/ -lX11 -lm util/tspShow.c ${i}.cc -o ${i}.out"
  for j in "${b[@]}"
  do
    echo "target: ${j}"
    eval "time ./${i}.out -n -c random/${j}.tsp | ./export.out > dist/${j}.${i}.svg"
    sleep 3
  done
done

