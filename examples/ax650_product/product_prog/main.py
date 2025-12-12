#!/usr/bin/env python3
import sys, os
import threading
import time
from PyQt5 import QtWidgets, uic
from PyQt5.QtWidgets import (
    QApplication,  # PyQt5应用程序基础类
    QMainWindow,  # 主窗口类
    QWidget,  # 基础窗口部件类
    QVBoxLayout,  # 垂直布局管理器
    QPushButton,  # 按钮控件
    QLabel,  # 标签控件
    QProgressBar,  # 进度条控件
    QHeaderView,  # 表头视图
    QTableWidgetItem,  # 表格项
    QMessageBox,  # 消息框
    QFileDialog,  # 文件对话框
)
from PyQt5.QtCore import QThread, pyqtSignal, QObject  # 线程相关类
from PyQt5.QtCore import QTimer, QDateTime  # 定时器和时间相关类
from PyQt5.QtGui import QFont, QColor  # 字体和颜色类
from datetime import datetime  # 时间处理模块
import socket  # 网络通信模块
import psutil  # 系统信息获取模块
import threading  # 多线程模块
from matplotlib.backends.backend_qt5agg import FigureCanvasQTAgg as FigureCanvas
from matplotlib.figure import Figure
import fnmatch
import subprocess
import logging
from anytree import Node, RenderTree, findall
from parse import compile
import threading
import pty
import json
import numpy as np
import zmq
import serial
import soundfile as sf
from datetime import timedelta
import re




# 配置日志记录器
logging.basicConfig(
    level=logging.INFO, format="%(asctime)s - %(name)s - %(levelname)s - %(message)s"
)


def resource_path(relative_path):
    """获取资源文件的绝对路径"""
    base_path = getattr(sys, "_MEIPASS", os.path.dirname(os.path.abspath(__file__)))
    return os.path.join(base_path, relative_path)


class WorkerSignals(QObject):
    """定义工作线程的信号"""

    # 进度更新信号，传递整数类型的进度值
    progress_updated = pyqtSignal(int)
    # 状态更新信号，传递字符串类型的状态信息
    status_updated = pyqtSignal(str)
    # 任务完成信号，不传递参数
    finished = pyqtSignal()





from PyQt5.QtCore import (
    Qt, pyqtProperty, QPropertyAnimation, QSize, QObject, QEvent
)
from PyQt5.QtWidgets import (
    QApplication, QWidget, QVBoxLayout, QLabel
)
from PyQt5.QtGui import QColor, QFont, QPainter


class KeyStatusWidget(QWidget):
    ec_event_str = ''
    button_event = pyqtSignal(bool)
    def __init__(self, parent=None):
        super().__init__(parent)
        self._pressed = False  # 按键状态
        self._color_pos = 0    # 颜色过渡属性
        self.setFixedSize(100, 100)
        self._label = QLabel(self)
        self._label.setAlignment(Qt.AlignCenter)
        self._label.setFont(QFont("Arial", 32, QFont.Bold))
        self._label.resize(self.size())

        self._normal_color = QColor(120, 120, 120)
        self._pressed_color = QColor(60, 170, 255)
        self._animation = QPropertyAnimation(self, b"colorPos", self)
        self._animation.setDuration(300)

        self.update_ui()
        self.button_event.connect(self._button_event)
        # # 安装事件过滤器捕获键盘事件
        # QApplication.instance().installEventFilter(self)
        threading.Thread(target=self.start_button_sub, daemon=True).start()
    def set_ec_event_str(self, event_str):
        self.ec_event_str = event_str
    def update_ui(self):
        # 设置当前颜色和文字
        c = self.interpolate_color(self._normal_color, self._pressed_color, self._color_pos)
        self._label.setText(f"{'按下' if self._pressed else '松开'}")
        pal = self.palette()
        pal.setColor(self.backgroundRole(), c)
        self.setPalette(pal)
        self.setAutoFillBackground(True)
        self.update()

    def interpolate_color(self, c1, c2, t):
        # 线性插值颜色
        r = c1.red() + (c2.red() - c1.red()) * t
        g = c1.green() + (c2.green() - c1.green()) * t
        b = c1.blue() + (c2.blue() - c1.blue()) * t
        return QColor(int(r), int(g), int(b))

    # 动画属性
    def getColorPos(self):
        return self._color_pos

    def setColorPos(self, value):
        self._color_pos = value
        self.update_ui()

    colorPos = pyqtProperty(float, getColorPos, setColorPos)

    def _button_event(self, pressed):
        if pressed:
            if not self._pressed:
                self._pressed = True
                self.start_animation(True)
        else:
            if self._pressed:
                self._pressed = False
                self.start_animation(False)


    def eventFilter(self, obj, event):
        if event.type() == QEvent.KeyPress and event.key() == Qt.Key_T:
            if not self._pressed:
                self._pressed = True
                self.start_animation(True)
        if event.type() == QEvent.KeyRelease and event.key() == Qt.Key_T:
            if self._pressed:
                self._pressed = False
                self.start_animation(False)
        return super().eventFilter(obj, event)

    def start_animation(self, to_pressed):
        # 启动颜色动画
        self._animation.stop()
        self._animation.setStartValue(self._color_pos)
        self._animation.setEndValue(1.0 if to_pressed else 0.0)
        self._animation.start()

    def start_button_sub(self):
        import zmq
        context = zmq.Context()
        socket = context.socket(zmq.SUB)
        socket.connect("ipc:///tmp/llm/ec_prox.event.socket")
        socket.setsockopt_string(zmq.SUBSCRIBE, "")
        while True:
            data = socket.recv().decode('utf-8')
            if self.ec_event_str in data:
                if '"code":0,' in data:
                    self.button_event.emit(True)
                elif '"code":1,' in data:
                    self.button_event.emit(False)







class NetnsIfaceManager:
    def __init__(self):
        self.exec_head = 'ip netns exec eth1_namespace'
    def __enter__(self):
        os.system('ip netns add eth0_namespace')
        os.system('ip netns add eth1_namespace')
        os.system('ip link set eth0 netns eth0_namespace')
        os.system('ip link set eth1 netns eth1_namespace')
        os.system('ip netns exec eth0_namespace ip addr add 192.168.30.1/24 dev eth0')
        os.system('ip netns exec eth0_namespace ip link set eth0 up')
        os.system('ip netns exec eth1_namespace ip addr add 192.168.30.2/24 dev eth1')
        os.system('ip netns exec eth1_namespace ip link set eth1 up')
        os.system('ip netns exec eth0_namespace iperf3 -s &')
        return self

    def __exit__(self, exc_type, exc_val, exc_tb):
        # 接口 down
        os.system('killall iperf3')
        os.system('ip netns exec eth0_namespace ip link set eth0 down')
        os.system('ip netns exec eth1_namespace ip link set eth1 down')
        os.system('ip netns exec eth0_namespace ip link set eth0 netns 1')
        os.system('ip netns exec eth1_namespace ip link set eth1 netns 1')
        os.system('ip link set eth0 up')
        os.system('ip link set eth1 up')
        os.system('ip netns delete eth0_namespace')
        os.system('ip netns delete eth1_namespace')



class PtyPope:
    def __init__(self, cmd, work_dir = None):
        self._cmd = cmd
        self.proc = None
        self.fdopenFD = None
        self.master_fd = None
        self.slave_fd = None
        self.work_dir = work_dir

    def __enter__(self):
        self.master_fd, self.slave_fd = pty.openpty()
        self.proc = subprocess.Popen(
            self._cmd,
            stdout=self.slave_fd,
            stderr=self.slave_fd,
            bufsize=1,
            universal_newlines=True,
            close_fds=True,
            cwd=self.work_dir
        )
        os.close(self.slave_fd)
        self.fdopenFD = os.fdopen(self.master_fd, 'r')
        return self.fdopenFD

    def __exit__(self, exc_type, exc_val, exc_tb):
        if self.fdopenFD:
            self.fdopenFD.close()
        # if self.proc:
        #     self.proc.wait()
        































class MainWindow(QMainWindow):
    # 设备开关事件信号，传递布尔值表示设备连接状态
    open_close_event = pyqtSignal(bool)
    # 主窗口执行信号，用于在线程中执行UI更新操作
    MainWindowExec = pyqtSignal(str, dict)

    def __init__(self):
        super().__init__()
        # 加载 .ui 文件
        uic.loadUi(resource_path("main.ui"), self)
        # 按键ui
        status_widget = KeyStatusWidget()
        status_widget.set_ec_event_str(',"vale":204}')
        self.but_status.addWidget(status_widget)

        status_widget1 = KeyStatusWidget()
        status_widget1.set_ec_event_str(',"vale":0}')
        self.but_status_4.addWidget(status_widget1)

        status_widget2 = KeyStatusWidget()
        status_widget2.set_ec_event_str(',"vale":1}')
        self.but_status_5.addWidget(status_widget2)

        # eth 交叉测速
        self.eth_fig = Figure(figsize=(4, 3))
        self.eth_canvas = FigureCanvas(self.eth_fig)
        self.usb_tab_3.addWidget(self.eth_canvas)
        self.eth_ax = self.eth_fig.add_subplot(111)

        # 记录住线对象，设置颜色和标签
        self.eth_line_read, = self.eth_ax.plot(
            [], [], 'r-', label='RX (Mb/s)'
        )
        self.eth_line_write, = self.eth_ax.plot(
            [], [], 'b-', label='TX (Mb/s)'
        )

        self.eth_ax.set_xlabel('Time')
        self.eth_ax.set_ylabel('Rate (Mb/s)')
        self.eth_ax.legend()


        # eth0 测速
        self.eth0_fig = Figure(figsize=(4, 3))
        self.eth0_canvas = FigureCanvas(self.eth0_fig)
        self.usb_tab_4.addWidget(self.eth0_canvas)
        self.eth0_ax = self.eth0_fig.add_subplot(111)


        # 记录住线对象，设置颜色和标签
        self.eth0_line_read, = self.eth0_ax.plot(
            [], [], 'r-', label='RX (Mb/s)'
        )
        self.eth0_line_write, = self.eth0_ax.plot(
            [], [], 'b-', label='TX (Mb/s)'
        )

        self.eth0_ax.set_xlabel('Time')
        self.eth0_ax.set_ylabel('Rate (Mb/s)')
        self.eth0_ax.legend()

        # eth1 测速
        self.eth1_fig = Figure(figsize=(4, 3))
        self.eth1_canvas = FigureCanvas(self.eth1_fig)
        self.usb_tab_5.addWidget(self.eth1_canvas)
        self.eth1_ax = self.eth1_fig.add_subplot(111)

        # 记录住线对象，设置颜色和标签
        self.eth1_line_read, = self.eth1_ax.plot(
            [], [], 'r-', label='RX (Mb/s)'
        )
        self.eth1_line_write, = self.eth1_ax.plot(
            [], [], 'b-', label='TX (Mb/s)'
        )

        self.eth1_ax.set_xlabel('Time')
        self.eth1_ax.set_ylabel('Rate (Mb/s)')
        self.eth1_ax.legend()


        # disk 测速
        self.disk_fig = Figure(figsize=(4, 3))
        self.disk_canvas = FigureCanvas(self.disk_fig)
        self.usb_tab_2.addWidget(self.disk_canvas)
        self.disk_ax = self.disk_fig.add_subplot(111)
        # 记录住线对象，设置颜色和标签
        self.disk_line_read, = self.disk_ax.plot(
            [], [], 'r-', label='Read (Mb/s)'
        )
        self.disk_line_write, = self.disk_ax.plot(
            [], [], 'b-', label='Write (Mb/s)'
        )

        self.disk_ax.set_xlabel('Time')
        self.disk_ax.set_ylabel('Rate (Mb/s)')
        self.disk_ax.legend()


        # 音频
        self.audio_fig = Figure(figsize=(4, 8))  # 加大纵向空间
        self.audio_canvas = FigureCanvas(self.audio_fig)
        self.verticalLayout.addWidget(self.audio_canvas)
        self.audio_axes = []    # 保存四个ax
        self.audio_lines = []   # 保存四个line
        channels = ['Channel 1', 'Channel 2', 'Channel 3', 'Channel 4']
        colors = ['r', 'g', 'b', 'k']
        for i in range(4):
            ax = self.audio_fig.add_subplot(4, 1, i+1)  # 4行 1列 第i+1个
            line, = ax.plot([], [], color=colors[i], label=channels[i])
            ax.set_ylabel('Amp')
            ax.set_title(channels[i])
            if i == 3:
                ax.set_xlabel('Time (s)')
            self.audio_axes.append(ax)
            self.audio_lines.append(line)
        self.audio_fig.tight_layout()
        # # 测试
        # fig = Figure(figsize=(4, 3))
        # canvas = FigureCanvas(fig)
        # ax = fig.add_subplot(111)
        # ax.plot([1, 2, 3], [4, 5, 6], 'r-')
        # self.usb_tab_5.addWidget(canvas)





    #     # 隐藏标签页栏
    #     self.cli.tabBar().hide()
    #     # self.checkBox_10.setEnabled(False)

    #     # 连接UI控件的信号与槽函数
    #     self.canven_ui_connect()

    #     # 设置以太网监控
        self.eth_setup_monitor()
        # 设置USB监控
        self.usb_setup_monitor()
        # 扫描 pci 设备
        self.pci_setup_update()
        self.system_status_monitor()

        self.ui_connect()

    def canven_log_message(self, textEdit, log):
        """在控制台显示日志消息"""
        # 获取控制台的垂直滚动条并滚动到底部
        scrollbar = textEdit.verticalScrollBar()
        scrollbar.setValue(scrollbar.maximum())
        # 添加日志条目到控制台
        textEdit.append(log)
        # 再次滚动到底部确保最新消息可见
        scrollbar = textEdit.verticalScrollBar()
        scrollbar.setValue(scrollbar.maximum())

    def MainWindowExec_fun(self, fun_str, fun_obj):
        """反射函数"""
        exec(fun_str, fun_obj | {"self": self})

    def ui_connect(self):
        """连接UI控件的信号与槽函数"""
        # 主窗口反射函数
        self.MainWindowExec.connect(self.MainWindowExec_fun)
        self.pushButton_11.clicked.connect(self.eth0_iperf3_test_task)
        self.pushButton_12.clicked.connect(self.eth1_iperf3_test_task)
        self.pushButton.clicked.connect(self.rgb_mode_get)
        self.pushButton_2.clicked.connect(self.rgb_mode_set)
        self.pushButton_3.clicked.connect(self.rgb_size_get)
        self.pushButton_4.clicked.connect(self.rgb_size_set)
        self.pushButton_5.clicked.connect(self.rgb_color_get)
        self.pushButton_6.clicked.connect(self.rgb_color_set)
        self.pushButton_8.clicked.connect(self.grove_i2c_scan)
        self.pushButton_7.clicked.connect(self.uart_test_task)
        self.pushButton_14.clicked.connect(self.audio_capture_task)
        self.pushButton_15.clicked.connect(lambda : self.audio_play_task(1))
        self.pushButton_16.clicked.connect(lambda : self.audio_play_task(2))
        self.pushButton_18.clicked.connect(lambda : self.audio_play_task(3))
        self.pushButton_19.clicked.connect(lambda : self.audio_play_task(4))
        self.pushButton_13.clicked.connect(self.systemd_load_task)
        self.pushButton_17.clicked.connect(self.disk_write_read_test_task)
        # self.pushButton_9.clicked.connect(self.disk_write_read_test_open_dir)
        self.comboBox_2.currentIndexChanged.connect(self.disk_write_read_test_set_dir)
        self.pushButton_10.clicked.connect(self.eth_cross_jiao_iperf3_test_task)
        self.tabWidget.currentChanged.connect(self.on_tab_changed)


        self.checkBox.stateChanged.connect(lambda x: self.zmq_rpc_exec('pcie0_set_switch',f'{{"data":{1 if x else 0}}}'))
        self.checkBox_2.stateChanged.connect(lambda x: self.zmq_rpc_exec('pcie1_set_switch',f'{{"data":{1 if x else 0}}}'))
        # self.checkBox_3.stateChanged.connect(lambda x: self.zmq_rpc_exec('pcie1_set_switch',f'{{"data":{1 if x else 0}}}'))
        self.checkBox_4.stateChanged.connect(lambda x: self.zmq_rpc_exec('ext_power',f'{{"data":{1 if x else 0}}}'))
        self.checkBox_5.stateChanged.connect(lambda x: self.zmq_rpc_exec('usbds1_set_big_power',f'{{"data":{1 if x else 0}}}'))
        self.checkBox_6.stateChanged.connect(lambda x: self.zmq_rpc_exec('usbds2_set_big_power',f'{{"data":{1 if x else 0}}}'))
        self.checkBox_7.stateChanged.connect(lambda x: self.zmq_rpc_exec('usbds1_set_switch',f'{{"data":{1 if x else 0}}}'))
        self.checkBox_8.stateChanged.connect(lambda x: self.zmq_rpc_exec('usbds2_set_switch',f'{{"data":{1 if x else 0}}}'))
        self.checkBox_9.stateChanged.connect(lambda x: self.zmq_rpc_exec('usbds3_set_switch',f'{{"data":{1 if x else 0}}}'))
        self.checkBox_10.stateChanged.connect(lambda x: self.zmq_rpc_exec('grove_iic_set_switch',f'{{"data":{1 if x else 0}}}'))
        self.checkBox_11.stateChanged.connect(lambda x: self.zmq_rpc_exec('grove_uart_set_switch',f'{{"data":{1 if x else 0}}}'))

        self.pushButton_66.clicked.connect(lambda : self.lineEdit_30.setText(f'''{json.loads(self.zmq_rpc_exec('poweron_get_time',''))['data']}'''))
        self.pushButton_67.clicked.connect(lambda : self.zmq_rpc_exec('poweron_set_time',f'{{"data":{self.lineEdit_30.text()}}}'))

        self.pushButton_69.clicked.connect(lambda : self.zmq_rpc_exec('poweroff','{"data":1}'))
        self.pushButton_64.clicked.connect(lambda : self.lineEdit_29.setText(f'''{json.loads(self.zmq_rpc_exec('fan_get_pwm',''))['data']}'''))
        self.pushButton_65.clicked.connect(lambda : self.zmq_rpc_exec('fan_set_pwm',f'''{{"data":{self.lineEdit_29.text()}}}'''))

        self.pushButton_58.clicked.connect(self.cmm_test_task)
        self.pushButton_70.clicked.connect(self.fan_pwm_test_task)
        self.pushButton_71.clicked.connect(self.time_power_start)
        self.pushButton_59.clicked.connect(self.audio_play_logo)


    def audio_play_logo(self):
        def _play_logo(MainWindowExec):
            try:
                os.system('tinyplay -D0 -d0 /usr/local/m5stack/logo.wav')
            except Exception as e:
                pass
        threading.Thread(target=_play_logo, daemon=True, args=(self.MainWindowExec,)).start()

    def time_power_start(self):
        self.zmq_rpc_exec('time_power_start',f'''{{"data":{self.lineEdit_30.text()}}}''')
        self.zmq_rpc_exec('poweroff','{"data":1}')


    def fan_pwm_test_task(self):
        def _fan_pwm_test_task(MainWindowExec):
            pwm = json.loads(self.zmq_rpc_exec('fan_get_speed',f'''{{}}'''))['data']
            MainWindowExec.emit(f'''self.canven_log_message(self.textEdit_26, "PWM set 20% ")''', {})
            self.zmq_rpc_exec('fan_set_pwm',f'''{{"data":20}}''')
            time.sleep(3)
            if json.loads(self.zmq_rpc_exec('fan_get_speed',f'''{{}}'''))['data'] < 300:
                MainWindowExec.emit(f'''self.canven_log_message(self.textEdit_26, "20% PWM测试失败")''', {})
            MainWindowExec.emit(f'''self.canven_log_message(self.textEdit_26, "PWM set 50% ")''', {})
            self.zmq_rpc_exec('fan_set_pwm',f'''{{"data":50}}''')
            time.sleep(3)
            if json.loads(self.zmq_rpc_exec('fan_get_speed',f'''{{}}'''))['data'] < 300:
                MainWindowExec.emit(f'''self.canven_log_message(self.textEdit_26, "50% PWM测试失败")''', {})
            MainWindowExec.emit(f'''self.canven_log_message(self.textEdit_26, "PWM set 80% ")''', {})
            self.zmq_rpc_exec('fan_set_pwm',f'''{{"data":80}}''')
            time.sleep(3)
            if json.loads(self.zmq_rpc_exec('fan_get_speed',f'''{{}}'''))['data'] < 300:
                MainWindowExec.emit(f'''self.canven_log_message(self.textEdit_26, "80% PWM测试失败")''', {})
            MainWindowExec.emit(f'''self.canven_log_message(self.textEdit_26, "PWM set 100% ")''', {})
            self.zmq_rpc_exec('fan_set_pwm',f'''{{"data":100}}''')
            if json.loads(self.zmq_rpc_exec('fan_get_speed',f'''{{}}'''))['data'] < 300:
                MainWindowExec.emit(f'''self.canven_log_message(self.textEdit_26, "100% PWM测试失败")''', {})
            self.zmq_rpc_exec('fan_set_pwm',f'''{{"data":{pwm}}}''')
            MainWindowExec.emit(f'''self.pushButton_70.setText("测试")''', {})
            MainWindowExec.emit(f'''self.pushButton_70.setEnabled(True)''', {})
        self.pushButton_70.setText("测试中")
        self.pushButton_70.setEnabled(False)
        self.canven_log_message(self.textEdit_26, "开始风扇转速测试")
        threading.Thread(target=_fan_pwm_test_task, daemon=True, args=(self.MainWindowExec,)).start()

    def cmm_test_task(self):
        def _cmm_test_task(MainWindowExec):
            MainWindowExec.emit(f'''self.canven_log_message(self.textEdit_25, "执行CMM测试：/usr/local/m5stack/bin/ax650_sample_cmm")''', {})
            try:
                with PtyPope(['/usr/local/m5stack/bin/ax650_sample_cmm']) as sout:
                    for line in sout:
                        if line == '':
                            break
                        MainWindowExec.emit(f'''self.canven_log_message(self.textEdit_25, line)''', {"line": line.strip()})
            except (OSError, ValueError) as e:
                pass
            except Exception as e:
                MainWindowExec.emit(f'''self.canven_log_message(self.textEdit_25, f"{e}")''', {"e":e})
            MainWindowExec.emit(f'''self.pushButton_58.setText("开始测试")''', {})
            MainWindowExec.emit(f'''self.pushButton_58.setEnabled(True)''', {})
        self.pushButton_58.setText("正在测试中")
        self.pushButton_58.setEnabled(False)
        threading.Thread(target=_cmm_test_task, daemon=True, args=(self.MainWindowExec,)).start()



    ec_control_panel_status = False
    def on_tab_changed(self, index):
        tab_tabtext = self.tabWidget.tabText(index)
        if tab_tabtext == "EC控制面板":
            self.start_ec_control_panel()
        else:
            self.stop_ec_control_panel()

    def refresh_control_panel(self):
        data = self.zmq_rpc_exec("board_get_power_info", '''{}''')
        power_info = json.loads(data)
        for key in power_info["data"]:
            self_object = getattr(self, key)
            self_object.setText(str(power_info["data"][key]))
        data = self.zmq_rpc_exec("fan_get_speed", '''{}''')
        fun_info = json.loads(data)
        self.label_139.setText(str(fun_info["data"]))



    def start_ec_control_panel(self):
        self.ec_control_panel_status = True
        # 创建系统状态监控定时器
        self.control_panel_timer = QTimer()
        # 连接定时器超时信号到刷新系统状态函数
        self.control_panel_timer.timeout.connect(self.refresh_control_panel)
        # 设置定时器间隔为5000毫秒（5秒）
        self.control_panel_timer.setInterval(1000)  # 5秒刷新一次
        # 启动定时器
        self.control_panel_timer.start()
        self.zmq_rpc_exec('ec_button_set_lcd_event','{"data":1}')
        self.zmq_rpc_exec('ec_button_set_head_event','{"data":1}')

    def stop_ec_control_panel(self):
        if self.ec_control_panel_status:
            self.ec_control_panel_status = False
            self.control_panel_timer.stop()


    def disk_write_read_test_set_dir(self):
        # 设置磁盘读写测试目录
        dir_option = self.comboBox_2.currentText()
        if dir_option == "SD 卡":
            self.lineEdit_7.setText("/mnt/sdcard")
        elif dir_option == "pcie0 ssd":
            self.lineEdit_7.setText("/mnt/pcie0_ssd")
        elif dir_option == "pcie1 ssd":
            self.lineEdit_7.setText("/mnt/pcie1_ssd")
        elif dir_option == "emmc 存储":
            self.lineEdit_7.setText("/mnt/emmc")



    def disk_write_read_test_open_dir(self):
        # 打开目录选择对话框
        # directory = QFileDialog.getExistingDirectory(
        #     self,
        #     "选择文件夹",
        #     "",  # 起始目录，空字符串表示当前目录
        #     QFileDialog.ShowDirsOnly | QFileDialog.DontResolveSymlinks
        # )
        
        # if directory:
        #     self.lineEdit_7.setText(directory)
        #     print(f'选中的目录: {directory}')
        # fileName, _ = QFileDialog.getSaveFileName(
        #     self, "保存文件", "", "文本文件 (*.txt);;所有文件 (*)"
        # )
        pass



    def disk_write_read_test_task(self):
        work_dir = self.lineEdit_7.text()
        if work_dir == "":
            QMessageBox.warning(self, "测试目录设置失败", "请先设置测试目录", QMessageBox.Ok, QMessageBox.Ok)
            return
        file_block_size = 1

        if 'emmc' in work_dir:
            file_block_size = 4
        else:
            if 'sdcard' in work_dir:
                file_block_size = 1
                if not os.path.exists('/dev/mmcblk1'):
                    QMessageBox.warning(self, "SD 卡不存在", "请先插入 SD 卡", QMessageBox.Ok, QMessageBox.Ok)
                    return
                os.system(f'mount /dev/mmcblk1p1 {work_dir} ; sleep 1; mount -v > /tmp/test_mount.log')

            if 'pcie0' in work_dir:
                file_block_size = 4
                if not os.path.exists('/dev/nvme0n1'):
                    QMessageBox.warning(self, "PCIE0 SSD 不存在", "请先插入 PCIE0 SSD", QMessageBox.Ok, QMessageBox.Ok)
                    return
                os.system(f'mount /dev/nvme0n1p1 {work_dir} ; sleep 1; mount -v > /tmp/test_mount.log')
            if 'pcie1' in work_dir:
                file_block_size = 4
                if not os.path.exists('/dev/nvme1n1'):
                    QMessageBox.warning(self, "PCIE1 SSD 不存在", "请先插入 PCIE1 SSD", QMessageBox.Ok, QMessageBox.Ok)
                    return
                os.system(f'mount /dev/nvme1n1p1 {work_dir} ; sleep 1; mount -v > /tmp/test_mount.log')

            try:
                with open('/tmp/test_mount.log') as f:
                    if f'{work_dir}' not in f.read():
                        raise "error"
            except:
                QMessageBox.warning(self, f"设备挂载失败", "请检查硬件", QMessageBox.Ok, QMessageBox.Ok)
                return


        
        
        def _disk_sysbench_test(MainWindowExec):
            
            # 准备测试文件
            def run_prepare():
                cmd = '/usr/bin/sysbench fileio --file-total-size=512M prepare'.split()
                with PtyPope(cmd, work_dir) as sout:
                    for line in sout:
                        MainWindowExec.emit(f'''self.canven_log_message(self.textEdit_4, line)''', {"line": line.strip()})
                        if 'written' in line:
                            break

            
            disk_line_write = []
            disk_line_read = []
            disk_write_times = []
            disk_read_times = []
            
            # 测试顺序读取
            def run_test(test_mode, is_write_test=False):
                cmd = f'/usr/bin/sysbench fileio --file-total-size=512M --file-test-mode={test_mode} --report-interval=1 --max-time=11 --file-block-size={file_block_size}M --file-extra-flags=direct --file-fsync-all=on --num-threads=4 run'.split()
                pattern_tx = compile("{} reads: {read_speed} MiB/s writes: {write_speed} MiB/s {}")
                with PtyPope(cmd, work_dir) as sout:
                    for line in sout:
                        MainWindowExec.emit(f'''self.canven_log_message(self.textEdit_4, line)''', {"line": line.strip()})
                        if 'Threads fairness' in line:
                            break
                        if len(disk_line_write) >= 11:
                            continue
                        result_tx = pattern_tx.parse(line)
                        if result_tx is not None:
                            if is_write_test:
                                # 写入测试时关注 writes
                                disk_line_write.append(float(result_tx['write_speed']))
                                disk_write_times.append(len(disk_line_write))
                            else:
                                # 读取测试时关注 reads
                                disk_line_read.append(float(result_tx['read_speed']))
                                disk_read_times.append(len(disk_line_read))
                            
                            # 更新图表数据
                            MainWindowExec.emit(f'''self.disk_line_write.set_data(disk_write_times, disk_line_write)''', 
                                            {"disk_write_times": disk_write_times, "disk_line_write": disk_line_write})
                            MainWindowExec.emit(f'''self.disk_line_read.set_data(disk_read_times, disk_line_read)''', 
                                            {"disk_read_times": disk_read_times, "disk_line_read": disk_line_read})
                            MainWindowExec.emit(f'''self.disk_ax.relim()''', {})
                            MainWindowExec.emit(f'''self.disk_ax.autoscale_view()''', {})
                            MainWindowExec.emit(f'''self.disk_canvas.draw()''', {})
            
            try:
                run_prepare()
                # 顺序读取测试
                MainWindowExec.emit(f'''self.canven_log_message(self.textEdit_4, "开始顺序读取测试...")''', 
                                {"line": "开始顺序读取测试..."})
                run_test('seqrd', is_write_test=False)
                
                # 顺序写入测试
                MainWindowExec.emit(f'''self.canven_log_message(self.textEdit_4, "开始顺序写入测试...")''', 
                                {"line": "开始顺序写入测试..."})
                run_test('seqwr', is_write_test=True)
                
            finally:
                # 清理测试文件
                MainWindowExec.emit(f'''self.canven_log_message(self.textEdit_4, "清理测试文件...")''', 
                                {"line": "清理测试文件..."})
                # os.system(f'cd {work_dir} ; sysbench fileio --file-total-size=512M cleanup')
                subprocess.run('sysbench fileio --file-total-size=512M cleanup', shell=True, cwd=work_dir)
                MainWindowExec.emit(f'''self.canven_log_message(self.textEdit_4, "磁盘测试完成")''', 
                                {"line": "磁盘测试完成"})
                if work_dir != "/mnt/emmc":
                    os.system(f'umount {work_dir}')
            print("------------------disk test finish------------------")
        
        threading.Thread(target=_disk_sysbench_test, daemon=True, args=(self.MainWindowExec,)).start()





    systemd_load_status = False
    def systemd_load_task(self):
        if self.systemd_load_status:
            self.systemd_load_status = False
            self.pushButton_13.setText("启动压力测试")
            self.canven_log_message(self.textEdit_8, "停止 CPU 压力测试 程序")
            os.system("start-stop-daemon --stop --pidfile /tmp/sysbench_cpu.pid")
            self.canven_log_message(self.textEdit_8, "停止 NPU 压力测试 程序")
            os.system("start-stop-daemon --stop --pidfile /tmp/sample_npu_yolov5s_npu.pid")
        else:
            self.systemd_load_status = True
            self.pushButton_13.setText("停止压力测试")
            self.canven_log_message(self.textEdit_8, "启动 CPU 压力测试 程序")
            os.system("start-stop-daemon --start --quiet --background --make-pidfile --pidfile /tmp/sysbench_cpu.pid --chdir /tmp --exec /usr/bin/sysbench -- cpu --time=2592000 --threads=9 run")
            self.canven_log_message(self.textEdit_8, "启动 NPU 压力测试 程序")
            os.system("start-stop-daemon --start --quiet --background --make-pidfile --pidfile /tmp/sample_npu_yolov5s_npu.pid --chdir /tmp --exec /opt/bin/sample_npu_yolov5s -- -m /opt/data/npu/models/yolov5s.axmodel -i /opt/data/npu/images/dog.jpg -r 10000000")




    def audio_play_task(self, channel):
        os.system(f'tinyplay -D 0 -d 0 /tmp/ch{channel}_32k.wav &')

    audio_capture_task_status = False

    def zmq_rpc_exec(self,cmd,data):
        context = zmq.Context()
        socket = context.socket(zmq.REQ)
        socket.setsockopt(zmq.SNDTIMEO, 1000)
        socket.setsockopt(zmq.RCVTIMEO, 1000)
        socket.connect("ipc:///tmp/rpc.ec_prox")
        socket.send_string(cmd, zmq.SNDMORE)
        socket.send_string(data) 
        try:
            data = socket.recv().decode('utf-8')
            return data
        except:
            print("接收超时，未获取到数据")
            return '{"data":"error"}'


    def audio_capture_task(self):
        if self.audio_capture_task_status:
            self.zmq_rpc_exec("fan_set_pwm", '''{"data":200}''')
            os.system("start-stop-daemon --stop --pidfile /tmp/tinycap_test.pid --signal INT; sleep 0.5")
            self.audio_capture_task_status = False
            self.audio_timer.stop()
            self.pushButton_14.setText("开始录音")
            data, samplerate = sf.read('/tmp/audio_test.wav')

            num_samples = len(data)
            duration = num_samples / samplerate

            # 如果数据点太多，进行降采样显示（只影响显示，不影响保存的文件）
            max_display_points = 10000
            if num_samples > max_display_points:
                step = num_samples // max_display_points
                data_display = data[::step]
                t = np.linspace(0, duration, len(data_display))
            else:
                data_display = data
                t = np.linspace(0, duration, num_samples)

            for i in range(4):
                # 保存完整数据
                ch_full = data[:, i]

                sf.write(f'/tmp/ch{i+1}.wav', ch_full, samplerate)
                volume_factor = int(self.lineEdit_8.text())
                os.system(f'ffmpeg -y -i /tmp/ch{i+1}.wav -ar 32000 -ac 2 -af volume={volume_factor} /tmp/ch{i+1}_32k.wav > /dev/null 2>&1 &')
                
                # 显示降采样后的数据
                ch_display = data_display[:, i]
                self.audio_lines[i].set_data(t, ch_display)
                
                # 设置坐标轴范围（基于完整数据）
                self.audio_axes[i].set_xlim(0, duration)
                y_min, y_max = ch_full.min(), ch_full.max()
                y_margin = (y_max - y_min) * 0.1 if y_max > y_min else 0.1
                self.audio_axes[i].set_ylim(y_min - y_margin, y_max + y_margin)

            self.audio_fig.tight_layout()
            self.audio_canvas.draw()

        else:
            print(self.zmq_rpc_exec("fan_set_pwm", '''{"data":20}'''))
            self.audio_capture_task_status = True
            self.pushButton_14.setText("停止录音")
            os.system("start-stop-daemon --start --quiet --background --make-pidfile --pidfile /tmp/tinycap_test.pid --chdir /tmp --exec /usr/local/bin/tinycap -- audio_test.wav -D 0 -d 1 -c 4  -b 16 -r 96000")
            self.time_count = 0
            def _refresh_audio():
                self.label_15.setText(str(timedelta(seconds=self.time_count)))
                self.time_count += 1
            self.audio_timer = QTimer()
            self.audio_timer.timeout.connect(_refresh_audio)
            _refresh_audio()
            self.audio_timer.start(1000)  # 每2秒更新一次显示
















    uart_work_status = False
    def uart_test_task(self):
        def _uart_read(MainWindowExec, ser):
            while ser.is_open:
                if ser.in_waiting:
                    data = ser.readline().decode('utf-8').strip()
                    MainWindowExec.emit(
                        f"""self.canven_log_message(self.textEdit_2, "接收:{data}")""",
                        {},
                    )
                time.sleep(0.1)
        def _uart_write(MainWindowExec, ser):
            count = 0
            while ser.is_open:
                ser.write(f'''Hello UART {count}\n'''.encode('utf-8'))
                MainWindowExec.emit(
                    f"""self.canven_log_message(self.textEdit, "发送:Hello UART {count}")""",
                    {},
                )
                count += 1
                time.sleep(1)

        if self.uart_work_status:
            self.ser.close()
            self.uart_work_status = False
            self.pushButton_7.setText("GROVE UART 测试")
        else:
            self.ser = serial.Serial('/dev/ttyS1', 115200, timeout=1)
            self.uart_work_status = True
            self.pushButton_7.setText("关闭UART测试")
            threading.Thread(target=_uart_read, args=(self.MainWindowExec, self.ser,), daemon=True).start()
            threading.Thread(target=_uart_write, args=(self.MainWindowExec, self.ser,), daemon=True).start()



    def grove_i2c_scan(self):
        # self.textEdit_3.setText("")
        # result = subprocess.run(['i2cdetect', '-y', '-r', '3'], capture_output=True, text=True)
        # self.textEdit_3.setText(result.stdout)
        with PtyPope(['i2cdetect', '-y', '-r', '3']) as sout:
            for line in sout:
                self.canven_log_message(self.textEdit_3, line.rstrip('\r\n'))

        # for lin in result.stdout.splitlines():
        #     self.textEdit_3.append(lin)


    def rgb_mode_get(self):
        context = zmq.Context()
        socket = context.socket(zmq.REQ)
        socket.setsockopt(zmq.SNDTIMEO, 1000)
        socket.setsockopt(zmq.RCVTIMEO, 1000)
        socket.connect("ipc:///tmp/rpc.ec_prox")
        socket.send_string("rgb_get_mode", zmq.SNDMORE)
        socket.send_string("") 
        try:
            data = socket.recv().decode('utf-8')
            params = json.loads(data)
            self.lineEdit.setText(f'''{params["data"]}''')
        except zmq.Again:
            print("接收超时，未获取到数据")
        
    def rgb_mode_set(self):
        if self.lineEdit.text() == "":
            QMessageBox.warning(self, "RGB模式获取失败", "请在输入框里输入RGB模式", QMessageBox.Ok, QMessageBox.Ok)
            return
        context = zmq.Context()
        socket = context.socket(zmq.REQ)
        socket.setsockopt(zmq.SNDTIMEO, 1000)
        socket.setsockopt(zmq.RCVTIMEO, 1000)
        socket.connect("ipc:///tmp/rpc.ec_prox")
        socket.send_string("rgb_set_mode", zmq.SNDMORE)
        socket.send_string(f'''{{"data":{self.lineEdit.text()}}}''')
        try:
            data = socket.recv().decode('utf-8')
            print(data)
        except zmq.Again:
            print("接收超时，未获取到数据")
        

    def rgb_size_get(self):
        context = zmq.Context()
        socket = context.socket(zmq.REQ)
        socket.setsockopt(zmq.SNDTIMEO, 1000)
        socket.setsockopt(zmq.RCVTIMEO, 1000)
        socket.connect("ipc:///tmp/rpc.ec_prox")
        socket.send_string("rgb_get_size", zmq.SNDMORE)
        socket.send_string("") 
        try:
            data = socket.recv().decode('utf-8')
            params = json.loads(data)
            self.lineEdit_2.setText(f'''{params["data"]}''')
        except zmq.Again:
            print("接收超时，未获取到数据")
    def rgb_size_set(self):
        if self.lineEdit_2.text() == "":
            QMessageBox.warning(self, "RGB灯数量获取失败", "请在输入框里输入RGB灯数量", QMessageBox.Ok, QMessageBox.Ok)
            return
        context = zmq.Context()
        socket = context.socket(zmq.REQ)
        socket.setsockopt(zmq.SNDTIMEO, 1000)
        socket.setsockopt(zmq.RCVTIMEO, 1000)
        socket.connect("ipc:///tmp/rpc.ec_prox")
        socket.send_string("rgb_set_size", zmq.SNDMORE)
        socket.send_string(f'''{{"data":{self.lineEdit_2.text()}}}''')
        try:
            data = socket.recv().decode('utf-8')
            print(data)
        except zmq.Again:
            print("接收超时，未获取到数据")



    def rgb_color_get(self):
        if self.lineEdit_3.text() == "":
            QMessageBox.warning(self, "RGB索引获取失败", "请在输入框里输入RGB索引", QMessageBox.Ok, QMessageBox.Ok)
            return
        context = zmq.Context()
        socket = context.socket(zmq.REQ)
        socket.setsockopt(zmq.SNDTIMEO, 1000)
        socket.setsockopt(zmq.RCVTIMEO, 1000)
        socket.connect("ipc:///tmp/rpc.ec_prox")
        socket.send_string("rgb_get_color", zmq.SNDMORE)
        socket.send_string(f'''{{"data":{self.lineEdit_3.text()}}}''') 
        try:
            data = socket.recv().decode('utf-8')
            params = json.loads(data)
            self.lineEdit_4.setText(f'''{params["data"]}''')
        except zmq.Again:
            print("接收超时，未获取到数据")

    def to_decimal_string(self, val):
        # 如果已经是int类型
        if isinstance(val, int):
            return str(val)
        # 如果是float，也可以按整数处理
        if isinstance(val, float):
            return str(int(val))

        # 如果是字符串
        if isinstance(val, str):
            s = val.strip().lower()
            if s.startswith('0b'):
                # 二进制字符串
                return str(int(s, 2))
            elif s.startswith('0x'):
                # 十六进制字符串
                return str(int(s, 16))
            elif s.startswith('0o'):
                # 八进制字符串
                return str(int(s, 8))
            else:
                # 默认按十进制处理
                return str(int(s))
        # 其他类型处理（比如 bytes）
        raise ValueError(f"不支持的类型: {type(val)}")

    def rgb_color_set(self):
        if self.lineEdit.text() != "0":
            self.lineEdit.setText("0")
            self.rgb_mode_set()

        if self.lineEdit_4.text() == "":
            QMessageBox.warning(self, "RGB颜色获取失败", "请在输入框里输入RGB颜色", QMessageBox.Ok, QMessageBox.Ok)
            return
        if self.comboBox.currentText() == "单个":
            if self.lineEdit_3.text() == "":
                QMessageBox.warning(self, "RGB索引获取失败", "请在输入框里输入RGB索引", QMessageBox.Ok, QMessageBox.Ok)
                return
            self.zmq_rpc_exec('rgb_set_color', f'''{{"data":{{"rgb_index":{self.lineEdit_3.text()},"rgb_color":{self.to_decimal_string(self.lineEdit_4.text())}}}}}''')
            try:
                data = socket.recv().decode('utf-8')
                print(data)
            except zmq.Again:
                print("接收超时，未获取到数据")
        else:
            if self.lineEdit_2.text() == "":
                self.rgb_size_get()
            self.zmq_rpc_exec('rgb_set_color', f'''{{"data":{{"rgb_index":-1,"rgb_color":{self.to_decimal_string(self.lineEdit_4.text())}}}}}''')





    def _eth_iperf3_test(self, MainWindowExec, cmd, canven_log, eth_line_write, eth_line_read, eth_ax, eth_canvas , end_fun = None):
        eth_times = []
        eth_write_speeds = []
        eth_read_speeds = []
        eth_count = 0
        MainWindowExec.emit(f'''self.canven_log_message({canven_log}, "start iperf3 -c ....")''', {})
        MainWindowExec.emit(f'''self.canven_log_message({canven_log}, cmd)''', {"cmd":str(cmd)})
        pattern_tx = compile("{}[TX-C]{}sec{}Bytes{} {speed:g} {}bits/sec{}")
        pattern_rx = compile("{}[RX-C]{}sec{}Bytes{} {speed:g} {}bits/sec{}")
        try:
            with PtyPope(cmd) as sout:
                for line in sout:
                    MainWindowExec.emit(f'''self.canven_log_message({canven_log}, line)''', {"line":line.strip()})
                    result_tx = pattern_tx.parse(line)
                    result_rx = pattern_rx.parse(line)
                    if result_tx is not None:
                        eth_write_speeds.append(int(result_tx['speed']))
                        eth_count += 1
                    elif result_rx is not None:
                        eth_read_speeds.append(int(result_rx['speed']))
                        eth_count += 1
                    else:
                        continue
                    if eth_count == 2:
                        eth_count = 0
                        eth_times.append(len(eth_times) + 1)
                        # 更新图表数据
                        print(eth_times, eth_write_speeds, eth_read_speeds)
                        MainWindowExec.emit(f'''self.{eth_line_write}.set_data(eth_times, eth_write_speeds)''', {"eth_times":eth_times, "eth_write_speeds":eth_write_speeds})
                        MainWindowExec.emit(f'''self.{eth_line_read}.set_data(eth_times, eth_read_speeds)''', {"eth_times":eth_times, "eth_read_speeds":eth_read_speeds})
                        MainWindowExec.emit(f'''self.{eth_ax}.relim()''', {})
                        MainWindowExec.emit(f'''self.{eth_ax}.autoscale_view()''', {})
                        MainWindowExec.emit(f'''self.{eth_canvas}.draw()''', {})
        except (OSError, ValueError) as e:
            MainWindowExec.emit(f'''self.canven_log_message({canven_log}, "iperf3 over")''', {})
        except Exception as e:
            MainWindowExec.emit(f'''self.canven_log_message({canven_log}, e)''', {"e":str(e)})
        if end_fun is not None:
            end_fun()






    def eth_cross_jiao_iperf3_test_task(self):
        """以太网性能测试任务"""
        row_count = self.interface_table.rowCount()
        ip_eth0_speed = None
        ip_eth1_speed = None
        for row in range(row_count):
            item = self.interface_table.item(row, 0)  # 第0列是接口名
            if item and item.text() == "eth0":
                ip_item = self.interface_table.item(row, 2)  # 第2列是速度
                if ip_item:
                    ip_eth0_speed = ip_item.text()
                break  # 找到就退出循环
        for row in range(row_count):
            item = self.interface_table.item(row, 0)  # 第0列是接口名
            if item and item.text() == "eth1":
                ip_item = self.interface_table.item(row, 2)  # 第2列是速度
                if ip_item:
                    ip_eth1_speed = ip_item.text()
                break  # 找到就退出循环
        print(ip_eth0_speed, ip_eth1_speed)
        if ip_eth0_speed != "1000 M" or ip_eth1_speed != "1000 M":
            QMessageBox.warning(self, "未确认以太网交叉连接", "请交叉插入以太网", QMessageBox.Ok, QMessageBox.Ok)
            return


        self.pushButton_10.setText("正在进行网络交叉测试，请勿关闭 ...")
        self.pushButton_10.setEnabled(False)
        def _end_fun():
            self.MainWindowExec.emit(f'''self.pushButton_10.setText("启动测试")''', {})
            self.MainWindowExec.emit(f'''self.pushButton_10.setEnabled(True)''', {})
        def _start_fun():
            self.MainWindowExec.emit(f'''self.canven_log_message(self.textEdit_5, "start eth namespace ...")''', {})
            with NetnsIfaceManager() as netns_mgr:
                cmd = netns_mgr.exec_head.split() + ['timeout', '30s' ,'iperf3', '-c', '192.168.30.1', '--bidir']
                self._eth_iperf3_test(self.MainWindowExec, cmd, 'self.textEdit_5','eth_line_write','eth_line_read','eth_ax','eth_canvas',_end_fun)
        threading.Thread(target=_start_fun, daemon=True, ).start()






    def eth0_iperf3_test_task(self):
        """以太网性能测试任务"""
        remoter_server = self.lineEdit_5.text()
        # local_ip = self.interface_table.item(0, 1).text()
        # for item in self.interface_table.selectedItems():
        #     print(item.row(), item.column(), item.text())
        
        row_count = self.interface_table.rowCount()
        ip_eth0 = None
        for row in range(row_count):
            item = self.interface_table.item(row, 0)  # 第0列是接口名
            if item and item.text() == "eth0":
                ip_item = self.interface_table.item(row, 1)  # 第1列是IP地址
                if ip_item:
                    ip_eth0 = ip_item.text()
                break  # 找到就退出循环

        if ip_eth0 == "无IP地址":
            QMessageBox.warning(self, "以太网IP获取失败", "未找到 eth0 或其IP地址", QMessageBox.Ok, QMessageBox.Ok)
            return
        self.pushButton_11.setText("正在测试 eth0 ...")
        self.pushButton_11.setEnabled(False)
        def _end_fun():
            self.MainWindowExec.emit(f'''self.pushButton_11.setText("启动测试")''', {})
            self.MainWindowExec.emit(f'''self.pushButton_11.setEnabled(True)''', {})
        cmd = ['timeout', '30s' ,'iperf3', '-c', remoter_server, '--bidir', '-B', ip_eth0]
        threading.Thread(target=self._eth_iperf3_test, daemon=True, args=(self.MainWindowExec, cmd, 'self.textEdit_6','eth0_line_write','eth0_line_read','eth0_ax','eth0_canvas',_end_fun)).start()





    def eth1_iperf3_test_task(self):
        """以太网性能测试任务"""
        remoter_server = self.lineEdit_5.text()        
        row_count = self.interface_table.rowCount()
        ip_eth1 = None
        for row in range(row_count):
            item = self.interface_table.item(row, 0)  # 第0列是接口名
            if item and item.text() == "eth1":
                ip_item = self.interface_table.item(row, 1)  # 第1列是IP地址
                if ip_item:
                    ip_eth1 = ip_item.text()
                break  # 找到就退出循环

        if ip_eth1 == "无IP地址":
            QMessageBox.warning(self, "以太网IP获取失败", "未找到 eth1 或其IP地址", QMessageBox.Ok, QMessageBox.Ok)
            return
        self.pushButton_12.setText("正在测试 eth1 ...")
        self.pushButton_12.setEnabled(False)
        def _end_fun():
            self.MainWindowExec.emit(f'''self.pushButton_12.setText("启动测试")''', {})
            self.MainWindowExec.emit(f'''self.pushButton_12.setEnabled(True)''', {})
        cmd = ['timeout', '30s' ,'iperf3', '-c', remoter_server, '--bidir', '-B', ip_eth1]
        threading.Thread(target=self._eth_iperf3_test, daemon=True, args=(self.MainWindowExec, cmd, 'self.textEdit_7','eth1_line_write','eth1_line_read','eth1_ax','eth1_canvas',_end_fun)).start()
        

    def pci_setup_update(self):
        """扫描 PCI 设备"""
        pcie_interfaces = [
            ["pcie0 左","无"],
            ["pcie1 右","无"]
        ]
        # 设置接口表格列数
        self.pcie_interface_table.setColumnCount(2)
        # 设置表头标签
        self.pcie_interface_table.setHorizontalHeaderLabels(["端口", "设备描述"])
        # 设置表头列宽度自动拉伸
        # self.pcie_interface_table.horizontalHeader().setSectionResizeMode(
        #     QHeaderView.Stretch
        # )
        # 第一列（端口）设置为定宽（比如100）
        self.pcie_interface_table.horizontalHeader().setSectionResizeMode(0, QHeaderView.Interactive)
        self.pcie_interface_table.setColumnWidth(0, 70)

        # 第二列（设备描述）设置为拉伸：自动填充剩余空间
        self.pcie_interface_table.horizontalHeader().setSectionResizeMode(1, QHeaderView.Stretch)

        # 设置接口表格行数
        self.pcie_interface_table.setRowCount(2)

        # pci_devices = []
        pstdout = subprocess.run(['lspci'], capture_output=True, text=True)
        pattern = compile("{id} {}")
        for lin in pstdout.stdout.splitlines():
            result = pattern.parse(lin)
            if result is not None:
                
                if result['id'] != "0000:00:00.0" and result['id'] != "0001:80:00.0":
                    if "0000:" in result['id']:
                        pcie_interfaces[0][1] = lin
                    elif "0001:" in result['id']:
                        pcie_interfaces[1][1] = lin
        for x,y in [(x, y) for x in range(2) for y in range(2)]:
            self.pcie_interface_table.setItem(x, y, QTableWidgetItem(pcie_interfaces[x][y]))







    def usb_refresh_usb_info(self):
        """刷新USB信息"""
        usb_current_interfaces = [
            ["port1 左", "无","空闲"],
            ["port2 右", "无","空闲"],
            ["port3 外", "无","空闲"],
            ["port4 内", "无","空闲"]
        ]
        result = subprocess.run(['lsusb', '-t'], capture_output=True, text=True)
        usbroot = Node("root")
        usbroot.num_spaces = -1
        now_node = usbroot
        for lin in result.stdout.splitlines():
            num_spaces = len(lin) - len(lin.lstrip(' '))
            if num_spaces == 0:
                now_node = Node(lin, parent=usbroot)
                now_node.num_spaces = num_spaces
            elif num_spaces > now_node.num_spaces:
                now_node = Node(lin, parent=now_node)
                now_node.num_spaces = num_spaces
            else:
                while now_node.num_spaces >= num_spaces:
                    now_node = now_node.parent
                now_node = Node(lin, parent=now_node)
                now_node.num_spaces = num_spaces
        # print(result.stdout)
        # for pre, fill, node in RenderTree(usbroot):
        #     print("%s%s" % (pre, node.name))
        try:
            usb3_0 = list(findall(usbroot, filter_=lambda n: "Bus 02.Port 1: Dev 1, Class=root_hub, Driver=xhci-hcd/1p, 5000M" in n.name))[0]
            pattern = compile("{}Port {port:d}: Dev {dev}, If {if}, Class={class}, {drive}, {speed}M")
            for usb3_0_dev in usb3_0.children[0].children:
                result = pattern.parse(usb3_0_dev.name)
                if result is None:
                    continue
                usb_current_interfaces[result['port'] - 1][1] = result['class']
                usb_current_interfaces[result['port'] - 1][2] = result['speed']
        except:
            pass
        # usb3_0_port =list(usb3_0.children)[0]  
        # for child in usb3_0_port:
        #     print(child.name)
        # except :
        #     pass
        try:
            usb2_0 = list(findall(usbroot, filter_=lambda n: "Bus 01.Port 1: Dev 1, Class=root_hub, Driver=xhci-hcd/1p, 480M" in n.name))[0]
            for usb2_0_dev in usb2_0.children[0].children:
                result = pattern.parse(usb2_0_dev.name)
                if result is None:
                    continue
                usb_current_interfaces[result['port'] - 1][1] = result['class']
                usb_current_interfaces[result['port'] - 1][2] = result['speed']
        except:
            pass
        # print(usb_current_interfaces)
        

        for x,y in [(x, y) for x in range(4) for y in range(3)]:
            self.usb_interface_table.setItem(x, y, QTableWidgetItem(usb_current_interfaces[x][y]))




        #     for child in usb2_0.children:
        #         print(child.name)
        # except :
        #     pass









    def usb_setup_monitor(self):
        # 设置接口表格列数
        self.usb_interface_table.setColumnCount(3)
        # 设置表头标签
        self.usb_interface_table.setHorizontalHeaderLabels(["端口", "设备类型", "速度"])
        # 设置表头列宽度自动拉伸
        self.usb_interface_table.horizontalHeader().setSectionResizeMode(
            QHeaderView.Stretch
        )
        self.usb_interface_table.setRowCount(4)

        # 刷新网络信息
        self.usb_refresh_usb_info()
        # 创建USB监控定时器
        self.usb_monitor_timer = QTimer()
        # 连接定时器超时信号到刷新USB信息函数
        self.usb_monitor_timer.timeout.connect(self.usb_refresh_usb_info)
        # 设置定时器间隔为3000毫秒（3秒）
        self.usb_monitor_timer.setInterval(1000)  # 1秒刷新一次
        # 启动定时器
        self.usb_monitor_timer.start()



    def eth_get_network_interfaces(self):
        """获取网络接口信息"""
        interfaces = {}

        try:
            for interface_name, interface_addresses in psutil.net_if_addrs().items():
                interface_info = {
                    "name": interface_name,
                    "status": "未知",
                    "ipv4": [],
                    "ipv6": [],
                    "speed": "-1"
                }

                stats = psutil.net_if_stats()
                if interface_name in stats:
                    interface_info["status"] = (
                        "UP" if stats[interface_name].isup else "DOWN"
                    )

                for address in interface_addresses:
                    if address.family == socket.AF_INET:  # IPv4
                        interface_info["ipv4"].append(address.address)
                    elif address.family == socket.AF_INET6:  # IPv6
                        interface_info["ipv6"].append(address.address)
                
                def is_ignored(interface):
                    ignore_list = ['lo', 'docker*', 'bond*']
                    for pattern in ignore_list:
                        if fnmatch.fnmatch(interface, pattern):
                            return True
                    return False
                try:
                    with open(f"/sys/class/net/{interface_name}/speed", "r") as f:
                        interface_info["speed"] = f.read().strip() + " M"
                except:
                    pass
                if not is_ignored(interface_name):
                    interfaces[interface_name] = interface_info

        except Exception as e:
            # self.canven_log_message(f"获取网络接口信息错误: {str(e)}")
            pass

        return interfaces

    def eth_update_interface_table(self, interfaces):
        """更新接口表格"""
        self.interface_table.setRowCount(len(interfaces))

        for row, (interface_name, info) in enumerate(interfaces.items()):
            self.interface_table.setItem(row, 0, QTableWidgetItem(interface_name))
            ip_text = "\n".join(info["ipv4"]) if info["ipv4"] else "无IP地址"
            self.interface_table.setItem(row, 1, QTableWidgetItem(ip_text))
            self.interface_table.setItem(row, 2, QTableWidgetItem(info["speed"]))

    def eth_refresh_network_info(self):
        """刷新网络信息"""
        eth_current_interfaces = self.eth_get_network_interfaces()
        self.eth_update_interface_table(eth_current_interfaces)

    def eth_setup_monitor(self):
        """设置以太网监控"""
        # 设置接口表格列数
        self.interface_table.setColumnCount(3)
        # 设置表头标签
        self.interface_table.setHorizontalHeaderLabels(["接口名称", "IP地址", "速度"])
        # 设置表头列宽度自动拉伸
        self.interface_table.horizontalHeader().setSectionResizeMode(
            QHeaderView.Stretch
        )

        # 刷新网络信息
        self.eth_refresh_network_info()
        # 创建以太网监控定时器
        self.eth_monitor_timer = QTimer()
        # 连接定时器超时信号到刷新网络信息函数
        self.eth_monitor_timer.timeout.connect(self.eth_refresh_network_info)
        # 设置定时器间隔为3000毫秒（3秒）
        self.eth_monitor_timer.setInterval(1000)  # 1秒刷新一次
        # 启动定时器
        self.eth_monitor_timer.start()

    # @profile
    def refresh_system_status(self):
        def _refresh_system_status(MainWindowExec):
            # 获取内存使用情况
            memory = psutil.virtual_memory()
            memory_str = f"{memory.used // (1024 * 1024)} / {memory.total // (1024 * 1024)} MB"
            MainWindowExec.emit(f'''self.label_23.setText(f"{memory_str}")''', {"memory_str": memory_str})
            # 获取CPU 温度
            try:
                with open("/sys/class/thermal/thermal_zone0/temp", "r") as f:
                    temp_milli = int(f.read().strip())
                    temp_celsius = temp_milli / 1000.0
                    MainWindowExec.emit(f'''self.label_25.setText(f"{temp_celsius:.1f} °C")''', {"temp_celsius": temp_celsius})
            except:
                pass
            # 获取cmm内存使用情况
            try:
                with open('/proc/ax_proc/mem_cmm_info', 'r') as f:
                    content = f.read()
                    
                # 使用正则表达式提取total size和used的KB值
                total_match = re.search(r'total size=(\d+)KB', content)
                used_match = re.search(r'used=(\d+)KB', content)
                
                if total_match and used_match:
                    total_kb = int(total_match.group(1))
                    used_kb = int(used_match.group(1))
                    
                    # 转换为MB
                    total_mb = total_kb // 1024
                    used_mb = used_kb // 1024
                    
                    # 格式化输出
                    result = f"{used_mb} / {total_mb} MB"
                    MainWindowExec.emit(f'''self.label_24.setText(result)''', {"result": result})
            except :
                pass
            # 获取NPU使用率
            try:
                with open("/proc/ax_proc/npu/top", "r") as f:
                    content = f.read()
                    total_match = re.search(r'utilization:(\d+)%', content)
                    if total_match:
                        utilization = total_match.group(1)
                        MainWindowExec.emit(f'''self.label_22.setText(f"{utilization}%")''', {"utilization": utilization})
                    else:
                        MainWindowExec.emit(f'''self.label_22.setText("no task")''', {})
            except:
                pass
            # 获取CPU使用率
            cpu_usage = psutil.cpu_percent(interval=0.5)
            try:
                MainWindowExec.emit(f'''self.label_21.setText(f"{cpu_usage}%")''', {"cpu_usage":cpu_usage})
            except:
                pass
        threading.Thread(target=_refresh_system_status, daemon=True, args=(self.MainWindowExec,)).start()



    def system_status_monitor(self):
        if os.path.exists("/proc/ax_proc/npu/enable"):
            os.system("echo 1 > /proc/ax_proc/npu/enable")
        # 创建系统状态监控定时器
        self.system_status_timer = QTimer()
        # 连接定时器超时信号到刷新系统状态函数
        self.system_status_timer.timeout.connect(self.refresh_system_status)
        # 设置定时器间隔为5000毫秒（5秒）
        self.system_status_timer.setInterval(1000)  # 5秒刷新一次
        # 启动定时器
        self.system_status_timer.start()

if __name__ == "__main__":
    app = QApplication(sys.argv)
    window = MainWindow()
    window.show()
    sys.exit(app.exec_())
