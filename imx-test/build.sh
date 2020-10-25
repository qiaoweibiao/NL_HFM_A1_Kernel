#!/bin/sh
make PLATFORM=NL_HFM_A1 LINUXPATH=/home/forlinx/imx8mm/rsc-x-linux_4.14.78/ \
KBUILD_OUTPUT=/home/forlinx/imx8mm/ALSA/ \
CROSS_COMPILE=/home/forlinx/rsc-x-linux-sdk_4.14.78/tools/sdk-toolchain/environment-setup-aarch64-poky-linux
