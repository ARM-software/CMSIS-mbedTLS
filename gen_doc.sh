#!/bin/bash
# Generate documentation

# Configuration header
CONFIG_H='include/mbedtls/mbedtls_config.h'

# Backup configuration
CONFIG_BAK=${CONFIG_H}.bak
cp -p $CONFIG_H $CONFIG_BAK

# Full configuration (all defines)
python scripts/config.py realfull

# Generate documentation
cd doxygen
doxygen mbedtls.doxyfile
cd ..

# Restore configuration header
mv $CONFIG_BAK $CONFIG_H
