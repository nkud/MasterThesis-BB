#! /usr/bin/python
# -*- coding: utf-8 -*-

import os

# 自動化する処理を作成する。
def auto(n):
    match = 'const ENERGY CELL_DIVISION_THRESHOLD_ENERGY ='
    distr = '%s %f; //: 細胞分裂エネルギー閾値' % (match, n)
    filepass = 'src/main.cpp'
    command='sed -i -e "/%s/c %s" %s' % (match, distr, filepass)
    os.system(command)
    os.system('make all')
    os.system('make pack')
 
def autorun(function, n):
    for i in n:
        function(i)

data = []
for i in range(200):
    data.append((i+1)*0.5)

# Main routine
if __name__ == '__main__':
    print '---> has started AutoRun system'
    autorun(auto, data)

