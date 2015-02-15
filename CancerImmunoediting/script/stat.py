#! /usr/bin/python
# -*- coding: utf-8 -*-

# constant value ################################

# 最大ステップ数
MAX_STEP = 100

SOURCE_FNAME = '../src/main.cpp'

# 設定パラメータを表す文字列
CONST_STRING = '//:'

#################################################

#
# 設定パラメータを取得する。
#

source_file = open(SOURCE_FNAME, 'r')
config_line = []
# ソースファイルから、定数パラメータを抜き出す
for line in source_file:
    if CONST_STRING in line:
        line = line.split()

        # パラメータの変数説明の有無で表示する箇所を変える
        if len(line) > 6:
            paramline = '%s = %s' % (line[6], line[4])
        else:
            paramline = '%s = %s' % (line[2], line[4])
        print paramline
        config_line.append(paramline)

#
# 通常のグラフスクリプトを生成する。
#

# auto.plt
auto_plot_file = open('auto.plt', 'w')

auto_plot_line = []
# -----------------------------------------------
auto_plot_line += 'set terminal png size 500,200;'

# 平均細胞エネルギー
auto_plot_line += 'plot "../bin/cell-energy-average.txt" w l;'
auto_plot_line += 'set output "cell-energy-average.png";'
auto_plot_line += 'replot;set output;'

# 総細胞数
auto_plot_line += 'plot "../bin/cell-size.txt" w l;'
auto_plot_line += 'set output "cell-size.png";'
auto_plot_line += 'replot;set output;'
# -----------------------------------------------

for line in auto_plot_line:
    auto_plot_file.write(line)

#
# アニメーション用のプロットスクリプトを生成する。
#

# animation.plt
animation_plot_file = open('glucose-animation.plt', 'w')

animation_plot_line = []
# -----------------------------------------------
animation_plot_line += 'set terminal gif animate optimize size 200,200 delay 5;'
animation_plot_line += 'set output "glucose-animation.gif";'
animation_plot_line += 'set style line 1 lw 2;'
animation_plot_line += 'set key below right;'
animation_plot_line += 'set key textcolor lt 0;'
animation_plot_line += 'n=1;'
animation_plot_line += 'load "glucose-frame.plt";'
# -----------------------------------------------

for line in animation_plot_line:
    animation_plot_file.write(line)

# frame.plt
frame_plot_file = open('glucose-frame.plt', 'w')

frame_plot_line = []
# -----------------------------------------------
frame_plot_line += 'title(n)=sprintf("t = %d", n);'
frame_plot_line += 'file(n)=sprintf("../bin/%d-glucose.txt", n);'
frame_plot_line += 'set title title(n);'
frame_plot_line += 'set view map;'
frame_plot_line += 'set cbrange[0:10];'
frame_plot_line += 'splot file(n) w pm3d;'
frame_plot_line += 'if(n<%d) n=n+1; reread;' % MAX_STEP
# -----------------------------------------------

for line in frame_plot_line:
    frame_plot_file.write(line)


# animation.plt
animation_plot_file = open('animation.plt', 'w')

animation_plot_line = []
# -----------------------------------------------
animation_plot_line += 'set terminal gif animate optimize size 200,200 delay 5;'
animation_plot_line += 'set output "animation.gif";'
animation_plot_line += 'set style line 1 lw 2;'
animation_plot_line += 'set key below right;'
animation_plot_line += 'set key textcolor lt 0;'
animation_plot_line += 'n=1;'
animation_plot_line += 'load "frame.plt";'
# -----------------------------------------------

for line in animation_plot_line:
    animation_plot_file.write(line)

# frame.plt
frame_plot_file = open('frame.plt', 'w')

frame_plot_line = []
# -----------------------------------------------
frame_plot_line += 'title(n)=sprintf("t = %d", n);'
frame_plot_line += 'file(n)=sprintf("../bin/%d-cell.txt", n);'
frame_plot_line += 'set title title(n);'
frame_plot_line += 'set view map;'
frame_plot_line += 'set cbrange[0:1];'
frame_plot_line += 'splot file(n) w pm3d;'
frame_plot_line += 'if(n<%d) n=n+1; reread;' % MAX_STEP
# -----------------------------------------------

for line in frame_plot_line:
    frame_plot_file.write(line)

#
# index.html
#
# 結果出力用のHTMLファイルを生成する。
#
def image_set_line(imagefname):
    """ 画像を配置する文字列を返す """
    line = ''
    line += '<!-- IMAGE -->'
    line += '<table class="graph"'
    line += '\t<tr><td><img src="%s" /></td></tr>' % imagefname
    line += '</table>'
    return line

def image_with_title_set_line(imagefname, title):
    """ 画像とタイトルを配置する文字列を返す """
    line = ''
    line += '<h2>%s</h2>\n' % title
    line += image_set_line(imagefname)
    return line


html_file = open('index.html', 'w')

html_line = []
# -----------------------------------------------
html_line += '<html>'
html_line += '<title>result</title>'
html_line += '<body>'
html_line += '<h1>result</h1>'

html_line += '<h2>configure</h2>'
for line in config_line:
    html_line += '%s<br />' % line
html_line += '<hr />'

html_line += image_set_line('animation.gif')
html_line += '<hr />'
html_line += image_set_line('glucose-animation.gif')
html_line += '<hr />'
html_line += image_with_title_set_line('cell-energy-average.png', '平均細胞エネルギー')
html_line += '<hr />'
html_line += image_with_title_set_line('cell-size.png', '総細胞数')
html_line += '</body></html>'
# -----------------------------------------------

for line in html_line:
    html_file.write(line)
