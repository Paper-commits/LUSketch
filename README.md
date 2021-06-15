# LUSketch

## Introduction
We present LU-Sketch, a novel sketch-heap framework for finding top-k with a guarantee for performance, memory space and precision. 

the source code includes the C++ implementation of the Count-min Heap, HeavyKeeper, SpaceSaving, WavingSketch and LUSketch.

This repo also contains a small demo to show how to use this algorithms with a dataset.

## requirement
- g++ >=5.4
- make >= 4.1

we have test this repository in ubuntu 16.04.

## how to make
you can build these algorithms by
```
$ cd demo
$ make clean
$ make
```
## Usage
You can use the following commands to run demo.
```
$ cd demo
$ ./main.out
```
Next you can input different memory and k for testing. 
The result will be stored in result.csv.
