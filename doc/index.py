#!/usr/bin/env python

import re
import sys
from getopt import getopt
import string

re_entry = re.compile(r'<dt>(.*)<a href="(.*)">(.*)</a>')
re_entry2 = re.compile(r'<dt>(.*)<a href="(.*)">(.*)</a>\s*<a href="(.*)">(.*)</a>')
re_href = re.compile(r'<a href=".*">')

def warn(s):
    sys.stderr.write("Warning: " + s + "\n")

def check(e):
    if re_href.search(e):
        raise "Ooops"

def parse(filename):
    try:
        f = open(filename)
    except:
        warn("file " + filename + " missing")
        return None
    lines = f.readlines()
    f.close()

    entries = []

    for l in lines:
        if l[-1] == '\n':
            l = l[:-1]
        if not l:
            continue

        m = re_entry2.match(l)

        if not m is None:
            entries.append([m.group(1), m.group(2), m.group(3)])
            entries.append([m.group(1), m.group(4), m.group(5)])
            check(m.group(3))
            check(m.group(5))
        else:
            m = re_entry.match(l)
            if not m is None:
                entries.append([m.group(1), m.group(2), m.group(3)])
                check(m.group(3))

    sys.stderr.write("File " + filename + " Found " + str(len(entries)) + " entries\n")

    return entries

def write(file, entries):
    for k in entries.keys():
        file.write(k + "\n")
        for e in entries[k]:
            file.write(e[1] + " " + e[0] + "\n")

if __name__ == '__main__':
    opts, args = getopt(sys.argv[1:], "f:")

    output = sys.stdout

    for o, a in opts:
        if o == "-f":
            output = open(a, "w")

    entries = {}
    files = {'_' : 'theindex.htm'}
    for l in string.ascii_uppercase:
        files[l] = 'indx' + l + '.htm'

    for k in files.keys():
        en = parse(files[k])
        if en:
            entries[k] = en
    write(output, entries)

    if output != sys.stdout:
        output.close()
