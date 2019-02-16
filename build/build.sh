#! /bin/bash

########## Testing libraries ##########
echo
echo =========================================
echo Build YaizuComLib
echo =========================================

echo
echo Building process for YaizuComLib has started.
echo
if [ -e "deployment" ]; then
  rm -r deployment
fi

cd ../src

echo Building commonfunc...
cd commonfunc
make all
cd ..

echo Building stksocket...
cd stksocket
make all
cd ..

echo Building stkthread...
cd stkthread
make all
cd ..

echo Building stkwebapp
cd stkwebapp
make all
cd ..

