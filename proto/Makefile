TARGET = libserverproto.a
PROTO_DIR := ./proto
PYTHON_DIR := ./proto_python
INC_DIR := ./include
SRC_DIR := ./src
LIB_DIR := ./lib
BUILD_DIR := ./build
TMP_DIR := ./tmp

PROTO_CPP_INC := $(INC_DIR)/$(PROTO_DIR)
PROTO_CPP_SRC := $(SRC_DIR)/$(PROTO_DIR)

PROTO_FILES := $(wildcard $(PROTO_DIR)/*.proto)
SRC_FILES := $(wildcard $(PROTO_CPP_SRC)/*.cc)
TMP_OBJ := $(addprefix $(BUILD_DIR)/,$(SRC_FILES))
OBJ_FILES := $(TMP_OBJ:.cc=.o)
DEP_FILES := $(TMP_OBJ:.cc=.d)

all: proto $(OBJ_FILES)
	ar rcs $(TARGET) $(OBJ_FILES)
	mv $(TARGET) $(LIB_DIR)

C_OPTION := -std=c++11 -Wall -g -I$(INC_DIR) -I/usr/local/include
$(BUILD_DIR)/%.o: %.cc
	$(shell test -d $(dir $@) || mkdir -p $(dir $@))
	g++ -c $< $(C_OPTION) -o $@

$(BUILD_DIR)/%.d: %.cc
	$(shell test -d $(dir $@) || mkdir -p $(dir $@))
	@set -e; rm -f $@; g++ -MM $(C_OPTION) $< > $@.$$$$; \
	sed 's,\(^.*\)\.o[ :]*,$@ $(@:.d=.o) : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$

-include $(DEP_FILES)

.PHONY: proto clean test
proto:
	@echo "protoc *.proto"
	$(shell test -d $(TMP_DIR) || mkdir -p $(TMP_DIR))
	$(shell test -d $(PYTHON_DIR) || mkdir -p $(PYTHON_DIR))
	$(shell test -d $(PROTO_CPP_INC) || mkdir -p $(PROTO_CPP_INC))
	$(shell test -d $(PROTO_CPP_SRC) || mkdir -p $(PROTO_CPP_SRC))
	@protoc $(PROTO_FILES) --cpp_out=$(TMP_DIR) --python_out=$(PYTHON_DIR)
	@mv $(TMP_DIR)/$(PROTO_DIR)/*.cc $(PROTO_CPP_SRC)
	@mv $(TMP_DIR)/$(PROTO_DIR)/*.h $(PROTO_CPP_INC)
	@rm -fr $(TMP_DIR)

test:
	@echo "PROTO_FILES: " $(PROTO_FILES)
	@echo "SRC_FILES: " $(SRC_FILES)
	@echo "OBJ_FILES" $(OBJ_FILES)

clean:
	@rm -fr $(TMP_DIR)
	@rm -fr $(PROTO_CPP_INC)/*.h
	@rm -fr $(PROTO_CPP_SRC)/*.cc
	@rm -fr $(BUILD_DIR)/*
