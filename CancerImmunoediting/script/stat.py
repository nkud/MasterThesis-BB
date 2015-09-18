#! /usr/bin/python
# -*- coding: utf-8 -*-

import datetime

# constant value ################################

# 最大ステップ数
ANIM_MAX_STEP = 100
MAX_STEP = 0

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
            paramline = '%s = %s' % (line[6], line[4][:-1])
        else:
            paramline = '%s = %s' % (line[2], line[4][:-1])
        print paramline
        if line[2] == 'MAX_STEP': MAX_STEP = line[4][:-1]
        config_line.append(paramline)

###############################################################################
#
# 通常のグラフスクリプトを生成する。
#

# auto.plt
auto_plot_file = open('auto.plt', 'w')

auto_plot_line = []
# -----------------------------------------------
auto_plot_line += 'set terminal png size 800,150;'

def graph_lines(fname, xlabel, ylabel):
    """ グラフ用の文字列を返す """
    lines = []
    lines += 'set xlabel "%s";' % xlabel
    lines += 'set ylabel "%s";' % ylabel
    lines += 'plot "../bin/%s.txt" w l;' % fname
    lines += 'set output "%s.png";' % fname
    lines += 'replot;set output;'
    return lines

# 平均細胞エネルギー
auto_plot_line += graph_lines("cell-energy-average", "STEP", "ENERGY AVERAGE")
auto_plot_line += graph_lines("normal-energy-average", "STEP", "NORMAL ENERGY AVE")
auto_plot_line += graph_lines("cancer-energy-average", "STEP", "CANCER ENERGY AVE")
auto_plot_line += graph_lines("genevalue-ave", "STEP", "GENE VALUE AVE")
# auto_plot_line += 'set xlabel "STEP";'
# auto_plot_line += 'set ylabel "ENERGY AVERAGE";'
# auto_plot_line += 'plot "../bin/cell-energy-average.txt" w l;'
# auto_plot_line += 'set output "cell-energy-average.png";'
# auto_plot_line += 'replot;set output;'

# 総細胞数
auto_plot_line += graph_lines("normalcell-size", "STEP", "NORMAL CELL SIZE")

auto_plot_line += graph_lines("mutantcancer-size", "STEP", "MUTANT CANCER SIZE")
auto_plot_line += graph_lines("standardcancer-size", "STEP", "STANDARD CANCER SIZE")
auto_plot_line += 'set ylabel "MUTANT & STANDARD";'
auto_plot_line += 'plot "../bin/standardcancer-size.txt" w l;'
auto_plot_line += 'replot "../bin/mutantcancer-size.txt" w l;'
auto_plot_line += 'set output "mutant-standard-cancer-size.png";'
auto_plot_line += 'replot;set output;'

# auto_plot_line += 'set ylabel "NORMAL CELL SIZE";'
# auto_plot_line += 'plot "../bin/normalcell-size.txt" w l;'
# auto_plot_line += 'set output "normalcell-size.png";'
# auto_plot_line += 'replot;set output;'

# 正常細胞＋がん細胞数
auto_plot_line += 'set ylabel "NORMAL & CANCER";'
auto_plot_line += 'plot "../bin/normalcell-size.txt" w l;'
auto_plot_line += 'replot "../bin/cancercell-size.txt" w l;'
auto_plot_line += 'set output "cell-size.png";'
auto_plot_line += 'replot;set output;'

# がん細胞数
auto_plot_line += 'set ylabel "CANCER CELL SIZE";'
auto_plot_line += 'plot "../bin/cancercell-size.txt" w l;'
auto_plot_line += 'set output "cancercell-size.png";'
auto_plot_line += 'replot;set output;'

# 排除された細胞数
auto_plot_line += 'set ylabel "DELETED CELL SIZE";'
auto_plot_line += 'plot "../bin/deleted-cell-size.txt" w l;'
auto_plot_line += 'set output "deleted-cell-size.png";'
auto_plot_line += 'replot;set output;'

# T細胞数
auto_plot_line += 'set ylabel "TCELL SIZE";'
auto_plot_line += 'plot "../bin/tcell-size.txt" w l;'
auto_plot_line += 'set output "tcell-size.png";'
auto_plot_line += 'replot;set output;'

# 初期化されたT細胞数
auto_plot_line += 'set ylabel "INIT TCELL SIZE";'
auto_plot_line += 'plot "../bin/init-tcell-size.txt" w l;'
auto_plot_line += 'set output "init-tcell-size.png";'
auto_plot_line += 'replot;set output;'

# 突然変異回数
auto_plot_line += 'set ylabel "MUTATION COUNT";'
auto_plot_line += 'plot "../bin/mutation-count.txt" w l;'
auto_plot_line += 'set output "mutation-count.png";'
auto_plot_line += 'replot;set output;'

auto_plot_line += 'set ylabel "NORMAL DIVISION COUNT";'
auto_plot_line += 'plot "../bin/normal-division-count.txt" w l;'
auto_plot_line += 'set output "normal-division-count.png";'
auto_plot_line += 'replot;set output;'
auto_plot_line += 'set ylabel "CANCER DIVISION COUNT";'
auto_plot_line += 'plot "../bin/cancer-division-count.txt" w l;'
auto_plot_line += 'set output "cancer-division-count.png";'
auto_plot_line += 'replot;set output;'
# -----------------------------------------------

for line in auto_plot_line:
    auto_plot_file.write(line)

###############################################################################
#
# アニメーション用のプロットスクリプトを生成する。
#

def animation( title, anim_title ):
    """ gifアニメーションを作成する """
    ANIM_TITLE = '%s-animation' % title
    FRAME_TITLE = '%s-frame' % title
    # animation.plt
    animation_plot_file = open('%s.plt' % ANIM_TITLE, 'w')
    animation_plot_line = []
    # -----------------------------------------------
    animation_plot_line += 'set terminal gif animate optimize size 200,200 delay 5;'
    animation_plot_line += 'set output "%s.gif";' % ANIM_TITLE
    # animation_plot_line += 'set style line 1 lw 2;'
    # animation_plot_line += 'set key below right;'
    # animation_plot_line += 'set key textcolor lt 0;'
    animation_plot_line += 'n=1;'
    animation_plot_line += 'load "%s.plt";' % FRAME_TITLE
    # -----------------------------------------------
    for line in animation_plot_line:
        animation_plot_file.write(line)
    # frame.plt
    frame_plot_file = open('%s.plt' % FRAME_TITLE, 'w')
    frame_plot_line = []
    # -----------------------------------------------
    frame_plot_line += 'title(n)=sprintf("t = %d", n);'
    frame_plot_line += 'file(n)=sprintf("../bin/%d-'+title+'.txt", n);'
    frame_plot_line += 'set title title(n);'
    frame_plot_line += 'set view map;'
    frame_plot_line += 'set cbrange[0:10];'
    frame_plot_line += 'set xlabel "%s";' % anim_title
    frame_plot_line += 'splot file(n) w pm3d;'
    frame_plot_line += 'if(n<%d) n=n+1; reread;' % ANIM_MAX_STEP
    # -----------------------------------------------
    for line in frame_plot_line:
        frame_plot_file.write(line)
    ######################
    # last-animation.plt
    animation_plot_file = open('last-%s.plt' % ANIM_TITLE, 'w')
    animation_plot_line = []
    # -----------------------------------------------
    animation_plot_line += 'set terminal gif animate optimize size 200,200 delay 5;'
    animation_plot_line += 'set output "last-%s.gif";' % ANIM_TITLE
    # animation_plot_line += 'set style line 1 lw 2;'
    # animation_plot_line += 'set key below right;'
    # animation_plot_line += 'set key textcolor lt 0;'
    animation_plot_line += 'n=%d;' % (int(MAX_STEP) - ANIM_MAX_STEP)
    animation_plot_line += 'load "last-%s.plt";' % FRAME_TITLE
    # -----------------------------------------------
    for line in animation_plot_line:
        animation_plot_file.write(line)
    # last-frame.plt
    frame_plot_file = open('last-%s.plt' % FRAME_TITLE, 'w')
    frame_plot_line = []
    # -----------------------------------------------
    frame_plot_line += 'title(n)=sprintf("t = %d", n);'
    frame_plot_line += 'file(n)=sprintf("../bin/%d-'+title+'.txt", n);'
    frame_plot_line += 'set title title(n);'
    frame_plot_line += 'set view map;'
    frame_plot_line += 'set cbrange[0:10];'
    frame_plot_line += 'set xlabel "LAST %s";' % anim_title
    frame_plot_line += 'splot file(n) w pm3d;'
    frame_plot_line += 'if(n<%d) n=n+1; reread;' % int(MAX_STEP)
    # -----------------------------------------------
    for line in frame_plot_line:
        frame_plot_file.write(line)


# 酸素マップアニメーション ################################################################
animation( 'oxygen', 'OXYGEN MAP' )

# グルコースマップアニメーション #############################################################
animation( 'glucose', 'GLUCOSE MAP' )

# 細胞マップアニメーション ################################################################
animation( 'cell', 'NORMAL & CANCER MAP' )
animation( 'normalcell', 'NORMAL CELL MAP' )
animation( 'cancercell', 'CANCER CELL MAP' )

# T細胞マップアニメーション ###############################################################
animation( 'tcell', 'TCELL MAP' )

###############################################################################
#
# index.html
#
# 結果出力用のHTMLファイルを生成する。
#
def image_set_line(*images):
    """ 画像を配置する文字列を返す """
    line = ''
    line += '<!-- IMAGE -->'
    line += '<table class="graph"><tr>'
    for image in images:
        line += '\t<td><img src="%s" /></td>' % image
    line += '</tr></table>'
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
html_line += '<meta http-equiv="Content-Type" content="text/html; charset=utf-8"> '

# CSS
html_line += '<style type="text/css">'
# html_line += 'h1{color:white; background:black;}'
html_line += 'body{font-family:"verdana", "consolas", "courier";}'
html_line += '</style>'

html_line += '<body>'
html_line += '<h1># result-%s</h1>' % datetime.datetime.now()

html_line += '<h2>parameter</h2>'

# パラメータ表
html_line += '<table>'
for line in config_line:
    line = line.split()
    html_line += '<tr><td>%s</td><td>%s</td></tr>' % ( line[0], line[2] )
html_line += '</table>'


html_line += '<hr />'
html_line += '<h2>%s</h2>\n' % '開始直後マップ'
html_line += image_set_line('cell-animation.gif', 'normalcell-animation.gif', 'cancercell-animation.gif')
html_line += image_set_line('last-cell-animation.gif', 'last-normalcell-animation.gif', 'last-cancercell-animation.gif')
html_line += image_set_line('tcell-animation.gif', 'glucose-animation.gif', 'oxygen-animation.gif')
html_line += image_set_line('last-tcell-animation.gif', 'last-glucose-animation.gif', 'last-oxygen-animation.gif')

html_line += '<hr />'
html_line += '<h2>%s</h2>\n' % '平均細胞エネルギー'
html_line += image_set_line('cell-energy-average.png')
html_line += image_set_line('normal-energy-average.png')
html_line += image_set_line('cancer-energy-average.png')
html_line += image_set_line('genevalue-ave.png')

html_line += '<hr />'
html_line += '<h2>%s</h2>\n' % '正常細胞・がん細胞'
html_line += image_set_line('normalcell-size.png')
html_line += image_set_line('cancercell-size.png')
html_line += image_set_line('cell-size.png')
html_line += image_set_line('deleted-cell-size.png')

html_line += '<hr />'
html_line += image_set_line('mutantcancer-size.png')
html_line += image_set_line('standardcancer-size.png')
html_line += image_set_line('mutant-standard-cancer-size.png')

html_line += '<hr />'
html_line += '<h2>%s</h2>\n' % 'T細胞'
html_line += image_set_line('tcell-size.png')
html_line += image_set_line('init-tcell-size.png')

html_line += '<hr />'
html_line += image_set_line('mutation-count.png')
html_line += image_set_line('normal-division-count.png')
html_line += image_set_line('cancer-division-count.png')


html_line += '</body></html>'
# -----------------------------------------------

for line in html_line:
    html_file.write(line)
