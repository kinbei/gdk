GDK(Game Develop Kit)
===

## Summary
```
1. the return type of all the error code are int32, success shall return 0. Otherwise, failed
2. //TODO means todo list
3. //!NOTE means sth need attention
```

## Build
```
git init
export GIT_SSL_NO_VERIFY=true
git clone https://github.com/sleepwom/gdk.git dev
cd gdk
make linux
```

## Run
```
./target/test ip port number
```
