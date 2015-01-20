import cPickle
import sys
import os
try:
    inputfile_name, outfile_name = sys.argv[1:]
except:
    sys.exit('Expecting two arguments the input and output filenames\n')
if not os.path.exists(inputfile_name):
    sys.exit('input file "{}" does not exist.\n'.format(inputfile_name))
if os.path.exists(outfile_name):
    sys.exit('output file "{}" does not exist.\n'.format(outfile_name))
a = {}
for line in open(inputfile_name, 'r'):
    ls = line.strip().split('\t')
    if len(ls) == 4:
        try:
            acc, gi, tax = ls[0], int(ls[1]), int(ls[3])
            acc = acc.split('.')[0]
            a[acc] = tax
            a[gi] = tax
        except:
            sys.exit('Unparseable line with 4 tabs:\n{}\n'.format(ls))
cPickle.dump(a, open(outfile_name, 'wb'))

