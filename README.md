1.迈德威视驱动安装：
	cd ./Mindvision/linuxSDK_V2.1.0.32
	sudo bash ./install.sh

 
 2.相机设备接口rule写入：
 	cd ./rules
 	sudo bash ./create_mvudev.sh
 重启电脑即可使用相机，如果不成功就进入"create_mvudev.sh"中逐条命令终端执行


 
 3."ArmorDetect_MindVision"项目基于迈德威视，为多线程架构
 已编译好可直接执行drone程序，该程序默认启动相机并录制视频，可以用它来检验你的迈德威视相机驱动是否安装成功


 
 4.基于迈德威视的Task3：
 多线程架构无需更改，相机参数需要你们自己改，
 TODO指明需要/可以修改的地方，process线程里面图像处理需要你们自己写，我给出的仅仅是一个imshow样例
 多写类，自己添加.h .cpp文件等，注意代码模块管理（anyway ，complete task in any way）
 
 
 
