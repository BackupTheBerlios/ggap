#!/usr/bin/env python

import sys
import re


re_sym_type = re.compile(r'[A-Z]+$')
re_func = re.compile(r'^([A-Za-z0-9_]+)\s*\(.*\)')


def warn(s):
    print "Warning: ", s

def sort(filename):
    f = open(filename)
    lines = f.readlines()
    f.close()

    funcs = {}
    unknown = []

    for l in lines:
        if l[-1] == '\n':
            l = l[:-1]
        if not l:
            warn("empty line in input file")
            continue

        m = re_func.search(l)

        if m is None:
            #warn("no symbol type on line: " + l)
            unknown.append(l)
        else:
            name = m.group(1)
            if funcs.has_key(name):
                funcs[name].append(l)
            else:
                funcs[name] = [l]

    print "Functions: ", len(funcs)
    print "Uknown: ", len(unknown)

    keys = funcs.keys()
    keys.sort()

    for name in keys:
        funcs[name].sort()
        for f in funcs[name]:
            print name + ": " + f

if __name__ == '__main__':
    if not sys.argv[1:]:
        raise
    sort(sys.argv[1])
