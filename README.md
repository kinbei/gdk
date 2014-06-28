GDK(Game Develop Kit)
===

## Summary
```
1. the return type of all the error code are int32, success shall return 0. Otherwise, failed
2. //TODO means todo list
3. //!NOTE means sth need attention
```


## Compatibility
```
GDK is cross-platform. Some platform/compiler combinations which have been tested are shown as follows.
* Visual C++ 2008 on Windows (64-bit)
* g++ (GCC) 4.1.2 20070115 (prerelease) (SUSE Linux)
```


## Build
```
export GIT_SSL_NO_VERIFY=true
git clone https://github.com/sleepwom/gdk.git --branch dev
cd gdk
mkdir lib
mkdir target
make linux
```


## Run
```
# run server
./target/testserver ip port client-bytes-check-number

# run client
./target/testclient ip port client-bytes-check-number client-connection-number

```
