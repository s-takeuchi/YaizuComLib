#! /bin/bash

########## Testing libraries ##########
echo
echo =========================================
echo Build YaizuComLib
echo =========================================

echo
echo Building process for YaizuComLib has started.
echo

echo
echo ==========================================
echo Deleting previous build folders...
echo

if [ -e "deployment" ]; then
  rm -r deployment
fi
mkdir deployment

echo
echo ==========================================
echo Making libraries
echo

cd ../src

echo Building stksocket...
cd stksocket
make all
tar cvf stksocket.tar stksocket.h stksocket.a
gzip stksocket.tar
mv stksocket.tar.gz ../../build/deployment
cd ..

echo Building commonfunc...
cd commonfunc
make all
tar cf commonfunc.tar msgproc.h StkGeneric.h StkProperties.h StkObject.h StkStringParser.h commonfunc.a
gzip commonfunc.tar
mv commonfunc.tar.gz ../../build/deployment
cd ..

echo Building stkthread...
cd stkthread
make all
tar cf stkthread.tar stkthread.h stkthread.a
gzip stkthread.tar
mv stkthread.tar.gz ../../build/deployment
cd ..

echo Building stkwebapp
cd stkwebapp
make all
tar cf stkwebapp.tar StkWebApp.h StkWebAppExec.h stkwebapp.a
gzip stkwebapp.tar
mv stkwebapp.tar.gz ../../build/deployment
cd ..

echo
echo Building process for YaizuComLib has ended.
echo
