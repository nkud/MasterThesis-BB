import os

print '...',

ls = os.listdir('.')

out = open('cancercell-size-master.txt', 'w')
for l in ls:
    n = 0
    if not 'result' in l: continue
    cancerfilepass = '%s/bin/cancercell-size.txt' % l
    file = open(cancerfilepass, 'r')
    for line in file:
        n += 1
        line = line.split()
        out.write('%s,' % line[1])
        #if n > 300: break
    out.write('\n')

raw_input('done')
