#! /usr/bin/python
# -*- coding: utf-8 -*-

SOURCE_FNAME = 'src/main.cpp'

source_file = open(SOURCE_FNAME, 'r')

# ソースファイルから、定数パラメータを抜き出す
for line in source_file:
	if '//:' in line:
		line = line.split()
		print '%s = %s' % (line[2], line[4])
	else:
		continue