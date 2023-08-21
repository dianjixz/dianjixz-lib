#!/bin/bash
source build/config/global_config.mk
dir_path=$(pwd)
dir_name=$(basename "$dir_path")
if [ "$CONFIG_USE_STRIP_REDUCE_SIZE_ENABLED" == "y" ]; then
    if [ "$CONFIG_TOOLCHAIN_PREFIX" != "" ]; then
        ${CONFIG_TOOLCHAIN_PATH}/${CONFIG_TOOLCHAIN_PREFIX}strip -s dist/$dir_name
    fi
fi
# END_SEGMENT
