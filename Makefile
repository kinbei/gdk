PLAT ?= none
PLATS = linux 

CC ?= g++

.PHONY : none $(PLATS) clean all cleanall


#ifneq ( $(PLAT), none )
.PHONY : default
default :
	$(MAKE) $(PLAT)
#endif

none :
	@echo "Please do 'make PLATFORM' where PLATFORM is one of these:"
	@echo "   $(PLATS)"

linux :
	$(MAKE) all PLAT=$@

GDK_BUILD_PATH ?= .

COMMON_CPP = io/bytesbuffer.cpp \
	net/acceptor.cpp net/tcpsocket.cpp \
	net/iocp/iocpnetiowrappers.cpp net/iocp/wsaextensionfunction.cpp \
	util/debug.cpp

TEST_CPP = main.cpp

all : \
	$(GDK_BUILD_PATH)/lib/libcommon.a	\
	$(GDK_BUILD_PATH)/target/testserver \
	$(GDK_BUILD_PATH)/target/testclient

$(GDK_BUILD_PATH)/lib/libcommon.a : $(foreach v, $(COMMON_CPP), src/common/$(v))
	g++ -g -fPIC --shared -o $@ $^ -Iinc -Iinc/common

$(GDK_BUILD_PATH)/target/testserver : $(foreach v, $(TEST_CPP), test/testserver/$(v))
	g++ -g -rdynamic -o $@ $^ -Iinc -Iinc/common -L./lib -lcommon

$(GDK_BUILD_PATH)/target/testclient : $(foreach v, $(TEST_CPP), test/testclient/$(v))
	g++ -g -rdynamic -o $@ $^ -Iinc -Iinc/common -L./lib -lcommon

