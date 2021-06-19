
# 利用python与单片机串口通信， 通过pip下载插件pyserial。
# 参考教程见 https://blog.csdn.net/Chorcier 12月的文章

# coding:utf-8


import serial.tools.list_ports
import time
plist = list(serial.tools.list_ports.comports())

if len(plist) <= 0:
		print("没有发现端口!")
else:
		plist_0 = list(plist[1])   # 原代码这里plist[0]默认的是COM5，但是我电脑的U单片机口是COM7,刚好是plist[1]
		serialName = plist_0[0]
		serialFd = serial.Serial(serialName, 9600, timeout=60)
		print("可用端口名>>>", serialFd.name)
		while 1:
				serialFd.write("o".encode())
				time.sleep(1)
				serialFd.write("c".encode())
				time.sleep(1)
