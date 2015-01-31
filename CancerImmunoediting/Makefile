CC = g++ -Wall -g -O0
MKDIR = mkdir -p
RM    = rm -rfv

bin_dir = bin
EXE_NAME = main.exe
TARGET = $(bin_dir)/$(EXE_NAME)

.PHONY: run all

$(TARGET): src/main.cpp
	@$(MKDIR) $(bin_dir)
	@$(CC) $^ -o $@ $(CPPFLAGS)

run:
	@cd $(bin_dir); ./$(EXE_NAME)

all: $(TARGET) run

clean:
	@$(RM) $(bin_dir)
