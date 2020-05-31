#!/bin/sh
make NL_HFM_A1_defconfig
unset LDFLAGS
make
echo "build Image successful"
cp arch/arm64/boot/Image /home/forlinx/imx8mm/rsc-x-linux_4.14.78/imgs/NL_HFM_A1/boot
cp arch/arm64/boot/dts/freescale/NL_HFM_A1.dtb /home/forlinx/imx8mm/rsc-x-linux_4.14.78/imgs/NL_HFM_A1/boot/

echo "cp arch/arm64/boot/Image ~/rsc-x-linux_4.14.78/imgs/NL_HFM_A1/boot/
cp arch/arm64/boot/dts/freescale/NL_HFM_A1.dtb /home/forlinx/imx8mm/rsc-x-linux_4.14.78/imgs/NL_HFM_A1/boot/"
