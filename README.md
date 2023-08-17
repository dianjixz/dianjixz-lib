# 简介
  用于程序设计原型的开发库。预计包含 linux、windows、soc、mcu，等平台的程序开发。

# 使用
快速食用,编译 hello world!

## 编译本机程序
``` bash
# 进入工作目录
cd examples/demo1
# 编译
make
# 测试运行程序
make run
```

## 交叉编译程序
``` bash
# 进入工作目录
cd examples/demo1
# 设置交叉编译工具链
make set_arm
# 编译
make
# 使用 scp 上传 
make push
# 使用 ssh 命令运行程序
# make push_run
```
## 相关命令
``` bash
# 清理编译文件
make clean
# 彻底清理编译文件
make distclean
# 编译优化版程序（可能出现异常）
make release
# 输出详细的编译过程
make verbose
# 打开或者关闭自带的库
make menuconfig
```

[库使用](./assets/README.md)