export OMP_NUM_THREADS=4
./main.exe -i inputs/globe.ray --ppm  -a inputs/globe.animate --movie -F 1 >& globe_final.txt
./main.exe -i inputs/pianoroom.ray --ppm -o output/pianoroom.ppm -H 500 -W 500 >& pianoroom_final.txt
./main.exe -i inputs/elephant.ray --ppm  -a inputs/elephant.animate --movie -F 1 -W 100 -H 100 -o output/sphere.mp4 >& sphere_final.txt
./main.exe -i inputs/realelephant.ray --ppm  -a inputs/elephant.animate --movie -F 1 -W 100 -H 100 -o output/elephant.mp4 >& elephant_final.txt