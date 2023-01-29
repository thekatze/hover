#/bin/bash

mkdir build
pushd build

conan install .. --build=missing

cmake .. -G Ninja -DCMAKE_EXPORT_COMPILE_COMMANDS=1

popd

ln -s build/compile_commands.json .
