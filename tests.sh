#!/bin/bash
# Unit test script
echo Starting test program
sleep 1
clear
echo Changing \#define TEST to 1
sleep 1
clear
sed -i '17 c\
#define TEST 1 ' src/main.c
echo Building a project
sleep 1
clear
make
echo Starting project
sleep 1
clear
./cppro
sleep 1
clear
echo Changing \#define TEST to 0
sleep 1
clear
sed -i '17 c\
#define TEST 0 ' src/main.c

