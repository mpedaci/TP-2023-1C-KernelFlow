PROCESS_NAME = memoria

TARGET = $(BIN_DIR)/$(PROCESS_NAME)

OBJ_DIR = ./obj
BIN_DIR = ./bin
INC_DIR = ./include
SRC_DIR = ./src

SHARED_INC_DIR = ../libs/include
SHARED_SRC_DIR = ../libs/src
SHARED_OBJ_DIR = ../libs/obj

LIBS = -lcommons -lpthread -lreadline -lrt
CFLAGS = -I$(INC_DIR) -I$(SHARED_INC_DIR) -g -Wall

SRCS = $(wildcard $(SRC_DIR)/*.c)
OBJS = $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRCS))

SHARED_SRC = $(wildcard $(SHARED_SRC_DIR)/*.c)
SHARED_OBJ = $(patsubst $(SHARED_SRC_DIR)/%.c,$(SHARED_OBJ_DIR)/%.o,$(SHARED_SRC))

$(TARGET) : $(OBJS) $(SHARED_OBJ)
	mkdir -p $(BIN_DIR)
	gcc $(OBJS) $(SHARED_OBJ) -o $(TARGET) $(CFLAGS) $(LIBS)

$(OBJ_DIR)/%.o : $(SRC_DIR)/%.c
	mkdir -p $(OBJ_DIR)
	gcc -c -MD $(CFLAGS) $< -o $@

$(SHARED_OBJ_DIR)/%.o: $(SHARED_SRC_DIR)/%.c
	mkdir -p $(SHARED_OBJ_DIR)
	gcc -c -MD $(CFLAGS) $< -o $@

-include $(OBJ_DIR)/*.d
-include $(SHARED_OBJ_DIR)/*.d

.PHONY : clean
clean :
	rm -r $(OBJ_DIR) $(BIN_DIR) $(SHARED_OBJ_DIR)