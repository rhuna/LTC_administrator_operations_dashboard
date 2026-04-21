#!/usr/bin/env bash
set -e

if [ -n "$QT_PREFIX" ]; then
  cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH="$QT_PREFIX"
else
  cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
fi
cmake --build build -j

echo

echo "Build completed. Run: ./build/LTCAdministratorOperationsDashboard.app or ./build/LTCAdministratorOperationsDashboard"
