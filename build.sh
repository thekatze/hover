#!/bin/bash

pushd build

cmake --build . --config Release

success=$?;

popd

exit $success;

