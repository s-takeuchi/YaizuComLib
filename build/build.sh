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

echo Building stkpl...
cd stkpl
make all
if [ $? -ne 0 ]; then
  exit $?
fi
tar cvf stkpl.tar StkPl.h stkpl.a
gzip stkpl.tar
mv stkpl.tar.gz ../../build/deployment
cd ..

echo Building stksocket...
cd stksocket
make all
if [ $? -ne 0 ]; then
  exit $?
fi
tar cvf stksocket.tar stksocket.h stksocket.a
gzip stksocket.tar
mv stksocket.tar.gz ../../build/deployment
cd ..

echo Building commonfunc...
cd commonfunc
make all
if [ $? -ne 0 ]; then
  exit $?
fi
tar cf commonfunc.tar msgproc.h StkProperties.h StkObject.h StkStringParser.h commonfunc.a
gzip commonfunc.tar
mv commonfunc.tar.gz ../../build/deployment
cd ..

echo Building stkthread...
cd stkthread
make all
if [ $? -ne 0 ]; then
  exit $?
fi
tar cf stkthread.tar stkthread.h stkthread.a
gzip stkthread.tar
mv stkthread.tar.gz ../../build/deployment
cd ..

echo Building stkdata...
cd stkdata
make all
if [ $? -ne 0 ]; then
  exit $?
fi
tar cf stkdata.tar stkdata.h stkdataapi.h stkdata.a
gzip stkdata.tar
mv stkdata.tar.gz ../../build/deployment
cd ..

echo Building stkwebapp
cd stkwebapp
make all
if [ $? -ne 0 ]; then
  exit $?
fi
tar cf stkwebapp.tar StkWebApp.h StkWebAppExec.h stkwebapp.a
gzip stkwebapp.tar
mv stkwebapp.tar.gz ../../build/deployment
cd ..

echo
echo Building process for YaizuComLib has ended.
echo
