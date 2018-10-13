OBJDIR := ../obj
SRCDIR := ../

SRC_FILES := $(wildcard $(SRCDIR)connection/*.cpp)
SRC_FILES += $(wildcard $(SRCDIR)server/*.cpp)
SRC_FILES += $(wildcard $(SRCDIR)proto_cpp/*.cc)
SRC_FILES += $(wildcard $(SRCDIR)service/*.cpp)
SRC_FILES += $(wildcard $(SRCDIR)util/*.cpp)
SRC_FILES += $(wildcard $(SRCDIR)util/*.c)
SRC_FILES += $(wildcard $(SRCDIR)http/*.cpp)

OBJ_TMP_CPP_FILES := $(SRC_FILES:.cpp=.o)
OBJ_TMP_CPP_CC_FILES := $(OBJ_TMP_CPP_FILES:.cc=.o)
OBJ_FILES := $(OBJ_TMP_CPP_CC_FILES:.c=.o)
ALL_BUILD_OBJ = $(subst ..,$(OBJDIR),$(OBJ_FILES))
ALL_BUILD_DEP = $(ALL_BUILD_OBJ:.o=.d)

OPTION := -std=c++11 -Wall -Werror -I/usr/local/boost/include -L/usr/local/boost/lib -lboost_system -pthread \
			-lboost_log -lboost_thread -lboost_log_setup -DBOOST_LOG_DYN_LINK \
			-I/usr/local/protobuf/include -L/usr/local/protobuf/lib -lprotobuf -g -D DEBUG -rdynamic

server:	$(ALL_BUILD_OBJ)
	g++ -o server $(ALL_BUILD_OBJ) $(OPTION)

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	astyle $<
	$(shell test -d $(dir $@) || mkdir -p $(dir $@))
	g++ -c $< $(OPTION) -o $@
$(OBJDIR)/%.o: $(SRCDIR)/%.cc
	$(shell test -d $(dir $@) || mkdir -p $(dir $@))
	g++ -c $< $(OPTION) -o $@
$(OBJDIR)/%.o: $(SRCDIR)/%.c
	$(shell test -d $(dir $@) || mkdir -p $(dir $@))
	g++ -c $< $(OPTION) -o $@

$(OBJDIR)/%.d: $(SRCDIR)/%.cpp
	$(shell test -d $(dir $@) || mkdir -p $(dir $@)) 
	@set -e; rm -f $@; g++ -MM $(OPTION) $< > $@.$$$$; \
	sed 's,\(^.*\)\.o[ :]*,$@ $(@:.d=.o) : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$
$(OBJDIR)/%.d: $(SRCDIR)/%.cc
	$(shell test -d $(dir $@) || mkdir -p $(dir $@)) 
	@set -e; rm -f $@; g++ -MM $(OPTION) $< > $@.$$$$; \
	sed 's,\(^.*\)\.o[ :]*,$@ $(@:.d=.o) : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$
$(OBJDIR)/%.d: $(SRCDIR)/%.c
	$(shell test -d $(dir $@) || mkdir -p $(dir $@))
	@set -e; rm -f $@; g++ -MM $(OPTION) $< > $@.$$$$; \
	sed 's,\(^.*\)\.o[ :]*,$@ $(@:.d=.o) : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$

-include $(ALL_BUILD_DEP)

clean:
	find ${OBJDIR} | xargs rm -fr
	rm -fr server

test:
	@echo SRC_FILES: $(SRC_FILES)
	@echo OBJ_TMP_CPP_FILES: $(OBJ_TMP_CPP_FILES)
	@echo OBJ_TMP_CPP_CC_FILES: $(OBJ_TMP_CPP_CC_FILES)
	@echo OBJ_FILES: $(OBJ_FILES)
	@echo ALL_BUILD_OBJ: $(ALL_BUILD_OBJ)
	@echo ALL_BUILD_DEP: $(ALL_BUILD_DEP)
