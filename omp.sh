export OMP_NUM_THREADS=1
./main.exe -i inputs/pianoroom.ray --ppm -o output/pianoroom.ppm -H 500 -W 500 >& pianoroom_1.txt
./main.exe -i inputs/globe.ray --ppm  -a inputs/globe.animate --movie -F 1 >& globe_1.txt
./main.exe -i inputs/elephant.ray --ppm  -a inputs/elephant.animate --movie -F 1 -W 100 -H 100 -o output/sphere.mp4 >& sphere_1.txt
./main.exe -i inputs/realelephant.ray --ppm  -a inputs/elephant.animate --movie -F 1 -W 100 -H 100 -o output/elephant.mp4 >& elephant_1.txt

export OMP_NUM_THREADS=2
./main.exe -i inputs/globe.ray --ppm  -a inputs/globe.animate --movie -F 1 >& globe_2.txt
./main.exe -i inputs/pianoroom.ray --ppm -o output/pianoroom.ppm -H 500 -W 500 >& pianoroom_2.txt
./main.exe -i inputs/elephant.ray --ppm  -a inputs/elephant.animate --movie -F 1 -W 100 -H 100 -o output/sphere.mp4 >& sphere_2.txt
./main.exe -i inputs/realelephant.ray --ppm  -a inputs/elephant.animate --movie -F 1 -W 100 -H 100 -o output/elephant.mp4 >& elephant_2.txt

export OMP_NUM_THREADS=3
./main.exe -i inputs/globe.ray --ppm  -a inputs/globe.animate --movie -F 1 >& globe_3.txt
./main.exe -i inputs/pianoroom.ray --ppm -o output/pianoroom.ppm -H 500 -W 500 >& pianoroom_3.txt
./main.exe -i inputs/elephant.ray --ppm  -a inputs/elephant.animate --movie -F 1 -W 100 -H 100 -o output/sphere.mp4 >& sphere_3.txt
./main.exe -i inputs/realelephant.ray --ppm  -a inputs/elephant.animate --movie -F 1 -W 100 -H 100 -o output/elephant.mp4 >& elephant_3.txt

export OMP_NUM_THREADS=4
./main.exe -i inputs/globe.ray --ppm  -a inputs/globe.animate --movie -F 1 >& globe_4.txt
./main.exe -i inputs/pianoroom.ray --ppm -o output/pianoroom.ppm -H 500 -W 500 >& pianoroom_4.txt
./main.exe -i inputs/elephant.ray --ppm  -a inputs/elephant.animate --movie -F 1 -W 100 -H 100 -o output/sphere.mp4 >& sphere_4.txt
./main.exe -i inputs/realelephant.ray --ppm  -a inputs/elephant.animate --movie -F 1 -W 100 -H 100 -o output/elephant.mp4 >& elephant_4.txt

