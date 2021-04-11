#! /bin/bash

########## Testing libraries ##########
echo
echo ==========================================
echo Testing libraries
echo

cd ../test

echo Building stkpltest
cd stkpltest
make all
if [ $? -ne 0 ]; then
  exit $?
fi
cd ..

echo Building commonfunctest
cd commonfunctest
make all
if [ $? -ne 0 ]; then
  exit $?
fi
cd ..

echo Building stksockettest...
cd stksockettest
make all
if [ $? -ne 0 ]; then
  exit $?
fi
cd ..

echo Building stkthreadtest...
cd stkthreadtest
make all
if [ $? -ne 0 ]; then
  exit $?
fi
cd ..

echo Building stkdatatest...
cd stkdatatest
make all
if [ $? -ne 0 ]; then
  exit $?
fi
cd ..

echo Building stkwebapptest
cd stkwebapptest
make all
if [ $? -ne 0 ]; then
  exit $?
fi
cd ..

echo ---------------------------------------
echo stkpltest
./stkpltest/stkpltest
if [ $? -ne 0 ]; then
  ls -l
  ls -l abc
  exit $?
fi
echo ---------------------------------------
echo commonfunctest
cp ./commonfunctest/x64/Release/*.prop ./commonfunctest
./commonfunctest/commonfunctest
if [ $? -ne 0 ]; then
  exit $?
fi
echo ---------------------------------------
echo stksockettest
cd stksockettest
./stksockettest
cd ..
if [ $? -ne 0 ]; then
  exit $?
fi
echo ---------------------------------------
echo stkthreadtest
./stkthreadtest/stkthreadtest
if [ $? -ne 0 ]; then
  exit $?
fi
echo ---------------------------------------
echo stkdatatest
./stkdatatest/stkdatatest
if [ $? -ne 0 ]; then
  exit $?
fi
echo ---------------------------------------
echo stkwebapptest
./stkwebapptest/stkwebapptest
if [ $? -ne 0 ]; then
  exit $?
fi

echo
echo Testing libraries has done.
echo

