#!/bin/bash

./build.sh

if [ $? -ne 0 ]; then
  echo Build failed, aborting
  exit -1
fi

./build/bin/hover

