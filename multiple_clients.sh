#!/bin/bash 
for N in {1..10}
do
    g++ client.cpp
    ./a.out
done
wait 