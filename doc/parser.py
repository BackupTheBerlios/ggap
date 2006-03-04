#!/usr/bin/env python

import sys
import re

class Para(object):
    def __init__(self):
        object.__init__(self)

        self.lines = []

    def add(self, line):
        self.lines.append(line)

class Text(Para): pass
class Symbol(Para): pass
class Example(Para): pass
class Items(Para): pass


class Section(object):
    def __init__(self, line):
        object.__init__(self)

        self.line = line
        self.paragraphs = []

    def new_paragraph(self, typ, line):
        para = typ()
        para.add(line)
        self.paragraphs.append(para)
        return para

class Chapter(object):
    def __init__(self, line):
        object.__init__(self)

        self.sections = []
        self.line = line

    def new_section(self, line):
        self.sections.append(Section(line))
        return self.sections[-1]


class Doc(object):
    def __init__(self):
        object.__init__(self)

        self.file = None
        self.chapters = []
        self.file_info = []
        self.copyright = []

    def add_file_info(self, line):
        self.file_info.append(line)
    def add_copyright(self, line):
        self.copyright.append(line)

    def new_chapter(self, line):
        self.chapters.append(Chapter(line))
        return self.chapters[-1]

re_file_info = re.compile(r'^%A  .*')
re_copyright = re.compile(r'^%Y  .*')
re_sectsep   = re.compile(r'^%%%%%%%%%%%%%%%%%%%%%%%%%%[%]+')
re_comment   = re.compile(r'^%.*')

re_chapter   = re.compile(r'^\\Chapter{.*}')
re_section   = re.compile(r'^\\Section{.*}')


class Parser(object):
    def __init__(self):
        object.__init__(self)
        self.reset()

    def reset(self):
        self.tags = {}
        self.state = self.start
        self.doc = Doc()

    def parse(self, file):
        f = open(file)
        lines = f.readlines()
        f.close()

        for i in xrange(len(lines)):
            l = lines[i]
            if l[-1:] == '\n':
                lines[i] = l[:-1]

        self.reset()
        self.doc.file = file
        self.lines = lines
        self.ptr = 0

        self.chap = None
        self.sect = None
        self.para = None

        while self.ptr < len(lines):
            self.state()
            self.ptr += 1

    def start(self):
        line = self.lines[self.ptr]
        m = re_file_info.match(line)
        if m:
            self.doc.add_file_info(line)
            return
        m = re_copyright.match(line)
        if m:
            self.doc.add_copyright(line)
            return
        m = re_comment.match(line)
        if m:
            return
        m = re_chapter.match(line)
        if m:
            self.chap = self.doc.new_chapter(line)
            self.state = self.chapter
            return
        raise RuntimeError("wrong line in the state 'start'\n" + line)

    def try_section(self):
        line = self.lines[self.ptr]

        m = re_sectsep.match(line)

        if m:
            self.ptr += 1
            line = self.lines[self.ptr]
            m = re_section.match(line)
            if m:
                self.sect = self.chap.new_section(line)
                self.state = self.section
                return True
            raise RuntimeError("wrong line %d after %%%% line in the state 'chapter'\n%s\n" % \
                                                                    (self.ptr + 1, line))
        return False

    def chapter(self):
        line = self.lines[self.ptr]

        if not len(line):
            return

        if self.try_section():
            return

        raise RuntimeError("wrong line %d in the state 'chapter'\n%s\n" % (self.ptr + 1, line))

    def section(self):
        line = self.lines[self.ptr]

        if not line:
            self.para = None
            return

        if line.startswith("\\beginitems"):
            self.para = self.sect.new_paragraph(Items, line)
            return

        if line.startswith("\\>"):
            self.para = self.sect.new_paragraph(Symbol, line)
            return

        if line.startswith("\\beginexample"):
            self.para = self.sect.new_paragraph(Example, line)
            return

        if not line.startswith("\\"):
            self.para = self.sect.new_paragraph(Text, line)
            return

        if self.para:
            self.para.add(line)
            return

        if self.try_section():
            return

        if line.startswith("\%"):
            return

        raise RuntimeError("unknown paragraph type on line %d\n%s\n" % (self.ptr + 1, line))

if __name__ == '__main__':
    file = '/home/muntyan/Desktop/gap/gap4r4/doc/ref/matint.tex'
    if sys.argv[1:]:
        file = sys.argv[1]
    Parser().parse(file)
