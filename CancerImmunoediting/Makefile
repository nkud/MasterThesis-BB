CC = g++ -Wall -g -O0
MKDIR = mkdir -p

bin_dir = bin
TARGET = $(bin_dir)/main.exe

.PHONY: run all

$(TARGET): src/main.cpp
	@$(MKDIR) $(bin_dir)
	@$(CC) $^ -o $@ $(CPPFLAGS)

run:
	@./$(TARGET)

all: $(TARGET) run
