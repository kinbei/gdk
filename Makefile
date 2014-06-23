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

COMMON_SRC = io/bytesbuffer.cpp \
	net/acceptor.cpp net/tcpsocket.cpp \
	net/iocp/iocpnetiowrappers.cpp net/iocp/wsaextensionfunction.cpp \
	util/debug.cpp

TEST_SRC = main.cpp


all : \
	$(GDK_BUILD_PATH)/lib/libcommon.a	\
	$(GDK_BUILD_PATH)/target/test

$(GDK_BUILD_PATH)/lib/libcommon.a : $(foreach v, $(COMMON_SRC), src/common/$(v))
	g++ -fPIC --shared -o $@ $^ -Iinc -Iinc/common


$(GDK_BUILD_PATH)/target/test : $(foreach v, $(TEST_SRC), test/$(v))
	g++ -rdynamic -o $@ $^ -Iinc -Iinc/common -L./lib -lcommon


