#!/bin/sh

########## Testing libraries ##########
echo ==========================================
echo Testing libraries

cd ../test

echo Building commonfunctest
cd commonfunctest
make all
cd ..

echo Building stksockettest...
cd stksockettest
make all
cd ..

echo Building stkthreadtest...
cd stkthreadtest
make all
cd ..

echo Building stkwebapptest
cd stkwebapptest
make all
cd ..

echo ---------------------------------------
echo commonfunctest
./commonfunctest/commonfunctest
echo ---------------------------------------
echo stksockettest
./stksockettest/stksockettest
echo ---------------------------------------
echo stkthreadtest
./stkthreadtest/stkthreadtest
echo ---------------------------------------
echo stkwebapptest
./stkwebapptest/stkwebapptest

echo Testing libraries has done.

