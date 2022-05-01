# The Vulkan Tutorial

"Drawing a Triangle" with Vulkan. An implementation of the tutorial at https://vulkan-tutorial.com/Drawing_a_triangle/Setup/Base_code.

Screenshot of the demo running on Ubuntu 21.10 on an AMD Ryzen 9 5900HX:

![Triangle in Vulkan](https://github.com/adilbaig/vulkan-tut/blob/79dbc48204369d011acdda6c46d1b1b246681e43/Screenshot%20from%202022-03-23%2008-50-52.png)

## Build Instructions

You will need Cmake and Vulkan installed. On Ubuntu 21, do this:

```
sudo apt install cmake libvulkan1 mesa-vulkan-drivers vulkan-utils
```

Run

```
cmake -S HelloTriangle/ -B build && cmake --build build
./build/bin/HelloTriangle 
```

## CustomCompute

An experiment with Compute Shaders and benchmark them against GPUs

```bash
cd CustomCompute
cmake -S . -B build && cmake --build build
time ./build/bin/CustomCompute
time ./build/bin/CppBenchmark
```
