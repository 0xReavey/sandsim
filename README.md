# Sandsim

<p align="middle">
  <img src="images/image_0.png" width="150" />
  <img src="images/image_1.png" width="150" /> 
  <img src="images/image_2.png" width="150" />
</p>

## Compilation

To compile you need SFML and CMake installed on your system.
<br>
If above requirements are fulfilled, just run the following
```bash
mkdir build && cd build
CXX=clang++ cmake -GNinja -DCMAKE_BUILD_TYPE=Release ..
ninja
./sandsim
```

## This project is under development and will probably be re-written
