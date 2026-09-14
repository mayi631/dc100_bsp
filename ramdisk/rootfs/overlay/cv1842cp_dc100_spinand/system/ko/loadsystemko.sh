#!/bin/sh
${CVI_SHOPTS}
#
# Define kernel modules sequence
#

modules_seq="
/system/ko/cv184x_osal.ko
/system/ko/cv184x_base.ko
/system/ko/cvi_ipcm.ko
/system/ko/hyn_ts.ko
/system/ko/aic8800_bsp.ko
/system/ko/bmtpu.ko
"

# SD0_PWR_EN pinmux to gpio as TP interrupt
devmem 0x03001038 32 3

#
# Start to insert kernel modules
#
if [ -n "$modules_seq" ]; then
    for mod in $modules_seq; do
        insmod "$mod"
    done
fi

# wifi log INF | ERROR
insmod /system/ko/aic8800_fdrv.ko aicwf_dbg_level=3
insmod /system/ko/3rd/8189fs.ko

# 640*480*4/1024=1200, double buffer 2400 KB. 270 degree rotation
insmod /system/ko/cv184x_gfbg.ko video="gfbg:vram0_size:2400" tde_rot=0

echo 3 > /proc/sys/vm/drop_caches
dmesg -n 4

exit $?
