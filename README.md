# linux

Use Ubuntu on raspberry pi for build:
Linux raspberrypi 6.11.0-1004-raspi #4-Ubuntu SMP PREEMPT_DYNAMIC Fri Sep 27 22:25:34 UTC 2024 aarch64 aarch64 aarch64 GNU/Linux


making the probe function get called:
-------------------------------------
sudo bash -c 'echo mpu6050 0x68 > /sys/class/i2c-adapter/i2c-1/new_device'

or we can use device tree chnages to make the probe function get called:

compiling and loading overlay
-----------------------------
dtc -@ -I dts -O dtb -o my-mpu6050.dtbo my-mpu6050-overlay.dts
sudo dtoverlay my-mpu6050.dtbo

NOTE: Dont use standard names like "mpu6050" for dtbo files. they wont load.
      Try to do native compilation as the build linux source version and target ubuntu
      kernel versions dont match.
