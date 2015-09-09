#
# Cancer Immunoediting Model Makefile
#
# TODO
# - 実行時には、一旦tmpディレクトリに実行して、
# binディレクトリは消さないようにすれば、
# 誤って実行してしまった時にもデータは残る。
# - timestampを上手く機能させる。

# command
PRINT = echo
CC    = g++ -Wall -g -O0
PY    = python
MKDIR = mkdir -p
COPY  = cp -r
RM    = rm -rf
PLOT  = gnuplot

# dictionary
bin_dir = bin
stat_dir = stat
EXE_NAME = CancerImmunoeditingModel.exe
TARGET = $(bin_dir)/$(EXE_NAME)


master_dir         = master
now			= $(shell date +%y%m%d-%H%M%S)
R = notitle
result_dir		= result-$(now)-$(R)

# utility
timestamp	:= $(shell date '+< %y/%m/%d %H:%M:%S >')


.PHONY: run all clean stat pack open re script plot info

$(TARGET): src/main.cpp
	@$(COLORECHO)
	@$(PRINT) '==> Creating $(notdir $@)...'
	@$(CLRECHO)
	@$(MKDIR) $(bin_dir)
	@$(CC) $^ -o $@ $(CPPFLAGS)
	@$(COLORECHO)
	@$(PRINT) '==> Done'
	@$(CLRECHO)

run:
	@$(COLORECHO)
	@$(PRINT) '==> Run $(EXE_NAME)'
	@$(CLRECHO)
	@cd $(bin_dir); ./$(EXE_NAME)
	@$(COLORECHO)
	@$(PRINT) '==> End $(timestamp) $(now)'
	@$(CLRECHO)

clean:
	@$(COLORECHO)
	@$(PRINT) '==> Cleanning $(bin_clean_files)'
	@$(CLRECHO)
	@$(RM) $(bin_dir) $(stat_dir)
	@$(COLORECHO)
	@$(PRINT) '==> Done'
	@$(CLRECHO)

plot:
	@$(COLORECHO)
	@$(PRINT) '==> Plotting...'
	@$(CLRECHO)
	@$(MKDIR) $(stat_dir)
	-@cd $(stat_dir); $(PLOT) auto.plt 1>/dev/null
	-@cd $(stat_dir); $(PLOT) cell-animation.plt 1>/dev/null
	-@cd $(stat_dir); $(PLOT) last-cell-animation.plt 1>/dev/null
	-@cd $(stat_dir); $(PLOT) normalcell-animation.plt 1>/dev/null
	-@cd $(stat_dir); $(PLOT) last-normalcell-animation.plt 1>/dev/null
	-@cd $(stat_dir); $(PLOT) cancercell-animation.plt 1>/dev/null
	-@cd $(stat_dir); $(PLOT) last-cancercell-animation.plt 1>/dev/null
	-@cd $(stat_dir); $(PLOT) tcell-animation.plt 1>/dev/null
	-@cd $(stat_dir); $(PLOT) last-tcell-animation.plt 1>/dev/null
	-@cd $(stat_dir); $(PLOT) glucose-animation.plt 1>/dev/null
	-@cd $(stat_dir); $(PLOT) oxygen-animation.plt 1>/dev/null
	-@cd $(stat_dir); $(PLOT) last-oxygen-animation.plt 1>/dev/null
	-@cd $(stat_dir); $(PLOT) last-glucose-animation.plt 1>/dev/null
	@$(COLORECHO)
	@$(PRINT) '==> Done'
	@$(CLRECHO)

script:
	@$(COLORECHO)
	@$(PRINT) '==> Scripting...'
	@$(CLRECHO)
	@$(MKDIR) $(stat_dir)
	@cd $(stat_dir); $(PY) ../script/stat.py
	@$(COLORECHO)
	@$(PRINT) '==> Done'
	@$(CLRECHO)

stat: script plot

pack:
	@$(COLORECHO)
	@$(PRINT) '==> Packing... >> $(result_dir)'
	@$(CLRECHO).
	@$(MKDIR) $(master_dir)
	@$(MKDIR) $(master_dir)/$(result_dir)
	@$(MKDIR) $(master_dir)/$(result_dir)/$(program_dir)
	@$(PRINT) '==> mkdir done.'
	@$(COPY) src script $(master_dir)/$(result_dir)/$(program_dir)/
	@$(PRINT) '==> cp src done.'
	@$(COPY) bin stat $(master_dir)/$(result_dir)/
	@$(PRINT) '==> cp stat done.'
	@$(PRINT) '$(master_dir)/$(result_dir)'
	@ls -al '$(master_dir)/$(result_dir)'
	@$(COLORECHO)
	@$(PRINT) '==> Done'
	@$(CLRECHO)

info:
	@$(COLORECHO)
	@$(PRINT) '==> Information'
	@$(CLRECHO)
	@$(COLORECHO)
	@$(PY) script/config.py
	# @$(PRINT) '^^^^^^^^^^^^^^^'
	@$(CLRECHO)

open:
	@$(COLORECHO)
	@$(PRINT) '==> Open'
	@$(CLRECHO)
	@open stat/index.html
	@$(COLORECHO)
	@$(PRINT) '==> Done'
	@$(CLRECHO)

re: clean $(TARGET)

all: clean $(TARGET) info run stat open

-include colors.mk
