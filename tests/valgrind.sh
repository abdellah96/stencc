#!/bin/sh

make

valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes \
./stencc < code/while.sten
