# TODO
# 実行時には、一旦tmpディレクトリに実行して、
# binディレクトリは消さないようにすれば、
# 誤って実行してしまった時にもデータは残る。
CC = g++ -Wall -g -O0
MKDIR = mkdir -p
RM    = rm -rfv
PLOT = gnuplot

bin_dir = bin
EXE_NAME = CancerImmunoeditingModel.exe
TARGET = $(bin_dir)/$(EXE_NAME)

.PHONY: run all clean stat pack

$(TARGET): src/main.cpp
	@$(MKDIR) $(bin_dir)
	@$(CC) $^ -o $@ $(CPPFLAGS)

run:
	@cd $(bin_dir); ./$(EXE_NAME)

clean:
	@$(RM) $(bin_dir)

stat:
	@$(PLOT) animation.plt

pack:

all: $(TARGET) run stat
