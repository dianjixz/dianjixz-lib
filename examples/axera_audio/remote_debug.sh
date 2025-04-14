#!/bin/bash

# 配置变量
TARGET_IP="192.168.28.150"          # 目标设备IP
TARGET_PORT="1234"                 # GDB Server端口
TARGET_PATH="/root"           # 目标设备上的路径
PROGRAM="axera_audio"             # 要调试的程序
LOCAL_PATH="dist/axera_audio"        # 本地程序路径
GDB_TOOL=/opt/gcc-linaro-7.5.0-2019.12-x86_64_aarch64-linux-gnu/bin/aarch64-linux-gnu-gdb
# 1. 传输程序到目标设备
echo "正在将程序传输到目标设备..."
sshpass -p root scp $LOCAL_PATH root@$TARGET_IP:$TARGET_PATH/

# 2. 在目标设备上启动gdbserver
echo "正在目标设备上启动gdbserver..."
sshpass -p root ssh root@$TARGET_IP "cd $TARGET_PATH && gdbserver :$TARGET_PORT ./$PROGRAM -d 0" &
sleep 2  # 等待gdbserver启动

# 3. 在主机上启动gdb并连接到目标设备
echo "正在主机上启动GDB..."
$GDB_TOOL $LOCAL_PATH -ex "target remote $TARGET_IP:$TARGET_PORT"


# sshpass -p root ssh root@192.168.28.22 'start-stop-daemon --stop --quiet --signal INT --oknodo --exec /root/dist/test_drm0 ; sleep 1 ; start-stop-daemon --start --quiet --background --make-pidfile --pidfile /var/run/test_drm0.pid  --exec /bin/bash -- -c "/root/dist/test_drm0 > /dev/ttyS0 2>&1"'

