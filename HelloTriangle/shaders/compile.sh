#!/bin/bash
set -o xtrace

DIR=$(dirname $(readlink -f $0))
glslc $DIR/shader.vert -o $DIR/vert.spv
glslc $DIR/shader.frag -o $DIR/frag.spv

cp $DIR/*.spv $DIR/../../build/bin/shaders || true