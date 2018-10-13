OBJDIR = ./build
INCDIR = ./include ./include/util ../proto/include
SRCDIR = ./src
TARGET = libservercore.a
LIBPATH = ../proto/lib
LIBNAME = serverproto

SRCFILES := $(wildcard $(SRCDIR)/connection/*.cpp)
SRCFILES += $(wildcard $(SRCDIR)/server/*.cpp)
SRCFILES += $(wildcard $(SRCDIR)/service/*.cpp)
SRCFILES += $(wildcard $(SRCDIR)/http/*.cpp)
SRCFILES += $(wildcard $(SRCDIR)/util/*.cpp)
SRCFILES += $(wildcard $(SRCDIR)/util/*.c)

BUILD_TMP_OBJ := $(SRCFILES:.cpp=.o)
BUILD_TMP2_OBJ := $(BUILD_TMP_OBJ:.c=.o)
BUILD_OBJ = $(subst ./src/,./build/,$(BUILD_TMP2_OBJ))
BUILD_DEP = $(BUILD_OBJ:.o=.d)

C_OPTION := -std=c++11 -Wall -g -D BOOST_LOG_DYN_LINK \
			-I/usr/local/boost/include/ -I/usr/local/include
C_OPTION += $(addprefix -I,$(INCDIR))

L_OPTION := -std=c++11 -Wall -g -D BOOST_LOG_DYN_LINK \
			-I/usr/local/include \
			-L/usr/local/boost/lib/ -L/usr/local/lib \
			-lboost_log_setup -lboost_log -lboost_filesystem -lboost_thread -lboost_system -pthread \
			-lprotobuf -g -rdynamic -D BOOST_LOG_DYN_LINK
L_OPTION += $(addprefix -I,$(INCDIR))
L_OPTION += $(addprefix -L,$(LIBPATH))
L_OPTION += $(addprefix -l,$(LIBNAME))

all: $(BUILD_OBJ)
	ar rcs $(TARGET) $(BUILD_OBJ)
	mv $(TARGET) ./lib

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	$(shell test -d $(dir $@) || mkdir -p $(dir $@))
	g++ -c $< $(C_OPTION) -o $@

$(OBJDIR)/%.o: $(SRCDIR)/%.c
	$(shell test -d $(dir $@) || mkdir -p $(dir $@))
	g++ -c $< $(C_OPTION) -o $@

$(OBJDIR)/%.d: $(SRCDIR)/%.cpp
	$(shell test -d $(dir $@) || mkdir -p $(dir $@))
	@set -e; rm -f $@; g++ -MM $(C_OPTION) $< > $@.$$$$; \
 	sed 's,\(^.*\)\.o[ :]*,$@ $(@:.d=.o) : ,g' < $@.$$$$ > $@; \
 	rm -f $@.$$$$

$(OBJDIR)/%.d: $(SRCDIR)/%.c
	$(shell test -d $(dir $@) || mkdir -p $(dir $@))
	@set -e; rm -f $@; g++ -MM $(C_OPTION) $< > $@.$$$$; \
 	sed 's,\(^.*\)\.o[ :]*,$@ $(@:.d=.o) : ,g' < $@.$$$$ > $@; \
 	rm -f $@.$$$$

-include $(BUILD_DEP)

clean:
	@rm $(BUILD_OBJ) $(BUILD_DEP) ./lib/$(TARGET)
	
test:
	@echo "SRCFILES: " $(SRCFILES)
	@echo "BUILD_OBJ: " $(BUILD_OBJ)
	@echo "BUILD_DEP: " $(BUILD_DEP)
