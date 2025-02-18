pushd ..

make clean; make -j ENABLE_OPT_CACHE=0 ENABLE_OMP=0 ENABLE_INPLACE=0;

./main.exe -i inputs/globe.ray --ppm  -a inputs/globe.animate --movie -F 1 >& globe_original.txt
./main.exe -i inputs/pianoroom.ray --ppm -o output/pianoroom.ppm -H 500 -W 500 >& pianoroom_original.txt
./main.exe -i inputs/elephant.ray --ppm  -a inputs/elephant.animate --movie -F 1 -W 100 -H 100 -o output/sphere.mp4 >& sphere_original.txt
./main.exe -i inputs/realelephant.ray --ppm  -a inputs/elephant.animate --movie -F 1 -W 100 -H 100 -o output/elephant.mp4 >& elephant_original.txt

popd