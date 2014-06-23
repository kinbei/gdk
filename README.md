gdk
===

Game Develop Kit

Note: 目前不支持自动构建, 只实现了 **bytesbuffer**, **网络相关** 类

1. 所有错误码的返回类型为 int32, 0表示成功, 非0表示失败
2. //TODO 作为todo list 的统一标识
3. //!NOTE  表示注意事项


## Build
```
git init
export GIT_SSL_NO_VERIFY=true
git clone https://github.com/sleepwom/gdk.git dev
cd gdk
make linux
```
