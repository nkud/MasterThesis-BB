#! /usr/bin/python
# -*- coding: utf-8 -*-

import os

HTML_FNAME = "index.html"
print '==> generating...'

html_file = open(HTML_FNAME, 'w')

dirs = os.listdir('./master')

lines = []
lines += '<html>'
lines += '<meta http-equiv="Content-Type" content="text/html; charset=utf-8"> '
lines += '<body>'

lines += '<h1># INDEX</h1>'
lines += '<table border="5px">'
for d in reversed(dirs):
    if not 'result' in d: continue
    lines += '<tr><td><a href="master/%s/stat/index.html">%s</a></td><td>none</td></tr>\n' % (d, d)

for line in lines:
    html_file.write(line)
lines += '</table>'

lines += '</body>'
lines += '</html>'

print '==> done'
