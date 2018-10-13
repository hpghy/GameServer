OBJDIR = ./build
INCDIR = ./include ../server_core/include ../proto/include
SRCDIR = ./src
TARGET = gateserver
LIBPATH = ../server_core/lib ../proto/lib
LIBNAME = servercore serverproto

SRCFILES := $(wildcard $(SRCDIR)/*.cpp)

BUILD_TMP_OBJ := $(SRCFILES:.cpp=.o)
BUILD_OBJ = $(subst ./src/,./build/,$(BUILD_TMP_OBJ))
BUILD_DEP = $(BUILD_OBJ:.o=.d)

C_OPTION := -std=c++11 -Wall -g -D BOOST_LOG_DYN_LINK \
			-I/usr/local/boost/include/ -I/usr/local/include
C_OPTION += $(addprefix -I,$(INCDIR))

L_OPTION := -std=c++11 -Wall -g -D BOOST_LOG_DYN_LINK \
			-I/usr/local/boost/include/ -I/usr/local/include \
			-L/usr/local/boost/lib/ -L/usr/local/lib \
			-lboost_log_setup -lboost_log -lboost_filesystem -lboost_thread -lboost_system -pthread \
			-lprotobuf -g -rdynamic -D BOOST_LOG_DYN_LINK
L_OPTION += $(addprefix -I,$(INCDIR))
L_OPTION += $(addprefix -L,$(LIBPATH))
L_OPTION += $(addprefix -l,$(LIBNAME))

all: $(BUILD_OBJ)
	g++ -o $(TARGET) $(BUILD_OBJ) $(L_OPTION)

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	$(shell test -d $(dir $@) || mkdir -p $(dir $@))
	g++ -c $< $(C_OPTION) -o $@

$(OBJDIR)/%.d: $(SRCDIR)/%.cpp
	$(shell test -d $(dir $@) || mkdir -p $(dir $@))
	@set -e; rm -f $@; g++ -MM $(C_OPTION) $< > $@.$$$$; \
 	sed 's,\(^.*\)\.o[ :]*,$@ $(@:.d=.o) : ,g' < $@.$$$$ > $@; \
 	rm -f $@.$$$$

-include $(BUILD_DEP)

clean:
	-rm $(TARGET) $(BUILD_DEP) $(BUILD_OBJ)

test:
	@echo "SRCFILES: " $(SRCFILES)
	@echo "BUILD_OBJ: " $(BUILD_OBJ)
	@echo "BUILD_DEP: " $(BUILD_DEP)
