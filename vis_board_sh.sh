#! /bin/bash

clear
echo "This script launches the board visualizer. (It requires OPENCV to be inistalled)"
echo "The board_vis.cpp file automatically loads the asp_solver.out file to visualize the current solution."

rm -rf board_vis

echo "Compiling..."
g++ -I/usr/local/include/opencv -I/usr/local/include/opencv2 -L/usr/local/lib/ -g -o board_vis board_vis.cpp -lopencv_core -lopencv_imgproc -lopencv_highgui -lopencv_ml -lopencv_video -lopencv_features2d -lopencv_calib3d -lopencv_objdetect -lopencv_contrib -lopencv_legacy -lopencv_stitching

echo "Visualizing...."
./board_vis
