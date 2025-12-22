#!/bin/bash

# µDo! Install Script

if [[ $EUID -ne 0 ]]; then
   echo "This script must be run as root (use sudo)"
   exit 1
fi

echo "Building µDo!..."
mkdir -p build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)

echo "Installing µDo!..."
make install

echo "Installation complete!"
echo "You can now run 'udo' from your terminal or application menu."
