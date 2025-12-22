#!/bin/bash

# µDo! Uninstall Script

if [[ $EUID -ne 0 ]]; then
   echo "This script must be run as root (use sudo)"
   exit 1
fi

if [ ! -f build/install_manifest.txt ]; then
    echo "Error: install_manifest.txt not found. Did you install using 'make install'?"
    exit 1
fi

echo "Uninstalling µDo!..."
while read -r file; do
    if [ -f "$file" ] || [ -L "$file" ]; then
        rm -v "$file"
    fi
done < build/install_manifest.txt

echo "Uninstallation complete."
