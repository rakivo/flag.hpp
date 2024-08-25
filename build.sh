#!bin/bash

CXX=g++
OUTFILE=example
CXXFLAGS=-std=c++17

set -xe

$CXX -o $OUTFILE $CXXFLAGS $OUTFILE.cpp
