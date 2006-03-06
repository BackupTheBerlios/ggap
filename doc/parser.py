#!/usr/bin/env python

import sys
import re


re_file_info = re.compile(r'^%A  .*')
re_copyright = re.compile(r'^%Y  .*')
re_sectsep   = re.compile(r'^%%%%%%%%%%%%%%%%%%%%%%%%%%[%]+')
re_comment   = re.compile(r'^%.*')
re_chapter   = re.compile(r'^\\(Chapter|PreliminaryChapter){.*}')
re_section   = re.compile(r'^\\Section{.*}')
re_index     = re.compile(r'^\\(index|atindex).*')


class Node(object):
    def __init__(self):
        object.__init__(self)

        self.lines = []
        self.indices = []

    def add(self, line):
        self.lines.append(line)

    def parse(self):
        raise NotImplementedError("%s.parse() not implemented" % (type(self),))

    def add_index(self, list):
        self.indices += list


class Para(Node): pass

class Text(Para):
    def parse(self):
        pass

class Example(Para):
    def parse(self):
        pass

class Items(Para):
    def parse(self):
        pass

class List(Para):
    def parse(self):
        pass

class Symbol(Para):
    def __init__(self, line):
        Para.__init__(self)
        self.add(line)

    def parse(self):
        pass

class Special(Para):
    def __init__(self, line):
        Para.__init__(self)
        self.add(line)

    def parse(self):
        pass


class Section(Node):
    def __init__(self):
        Node.__init__(self)

    def parse(self):
        if not self.lines:
            raise RuntimeError("empty section")

        if re_section.match(self.lines[0]):
                ptr = 1
        else:
                ptr = 0

        para = None
        self.paragraphs = []
        self.next_para_indices = []

        blocks = [
            ["\\beginexample", "\\endexample", Example],
            ["\\begintt", "\\endtt", Example],
            ["\\beginitems", "\\enditems", Items],
            ["\\beginlist", "\\endlist", List],
        ]

        while ptr < len(self.lines):
            line = self.lines[ptr]
            ptr += 1

            if not line:
                if para:
                    para = None
                continue

            if line.startswith("\\>"):
                self.add_para(Symbol(line))
                para = None
                continue

            found_block = False
            for b in blocks:
                if line.startswith(b[0]):
                    last = self.skip_block(ptr - 1, b[1])
                    self.add_block(b[2], ptr - 1, last)
                    ptr = last + 1
                    para = None
                    found_block = True
                    break
            if found_block:
                continue

            if re_index.match(line):
                first = ptr - 1
                last = first

                if first == 2:
                    self.add_index([line])
                    continue

                while re_index.match(self.lines[last + 1]):
                    last += 1

                self.add_index_next_para(first, last)
                ptr = last + 1
                continue

            if line.startswith("\\)"):
                para = None
                self.add_para(Special(line))
                continue

            if line.startswith("%"):
                continue

            # XXX
            if not line.startswith("\\") or \
                   line.startswith("\\ldots") or \
                   line.startswith("\\matrix") or \
                   line.startswith("\\medskip") or \
                   line.startswith("\\choose") or \
                   line.startswith("\\beta") or \
                   line.startswith("\\URL") or \
                   line.startswith("\\sum") or \
                   line.startswith("\\chi") or \
                   line.startswith("\\sigma") or \
                   line.startswith("\\{") or \
                   line.startswith("\\frac") or \
                   line.startswith("\\right") or \
                   line.startswith("\\\"") or \
                   line.startswith("\\cite"):
                if para:
                    assert type(para) == Text
                    para.add(line)
                para = Text()
                para.add(line)
                self.add_para(para)
                continue

            raise RuntimeError("bad paragraph start '%s'" % (line,))

        print "%d paragraphs" % (len(self.paragraphs),)

        for para in self.paragraphs:
            para.parse()

    def skip_block(self, first, end):
        last = first
        while last < len(self.lines) and not self.lines[last].startswith(end):
            last += 1
        return last

    def add_block(self, typ, first, last):
        para = typ()
        self.add_para(para)
        for i in range(first, last+1):
            para.add(self.lines[i])

    def add_index_next_para(self, first, last):
        self.next_para_indices += self.lines[first:last+1]

    def add_para(self, para):
        self.paragraphs.append(para)
        if self.next_para_indices:
            para.add_index(self.next_para_indices)
            self.next_para_indices = []


class Chapter(Node):
    def __init__(self):
        Node.__init__(self)

        self.sections = []

    def new_section(self):
        self.sections.append(Section())
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

    def chapter(self):
        self.chapters.append(Chapter())
        return self.chapters[-1]

class Parser(object):
    def __init__(self):
        object.__init__(self)
        self.reset()

    def reset(self):
        self.tags = {}
        self.doc = Doc()
        self.ptr = 0
        self.chap = None
        self.sect = None

    def parse(self, file):
        f = open(file)
        lines = f.readlines()
        f.close()

        self.lines = []

        for i in xrange(len(lines)):
            l = lines[i]
            if l[-1:] == '\n':
                l = l[:-1]
            if l[-1:] == '\r':
                l = l[:-1]
            self.lines.append(l)

        self.reset()
        self.doc.file = file

        self.get_sections()

        for s in parser.doc.chapters[-1].sections:
            s.parse()

    def get_sections(self):
        while self.ptr < len(self.lines):
            line = self.lines[self.ptr]
            self.ptr += 1

            if re_file_info.match(line):
                self.doc.add_file_info(line)
                continue

            if re_copyright.match(line):
                self.doc.add_copyright(line)
                continue

            if re_chapter.match(line):
                if self.chap:
                    raise RuntimeError("duplicated \\Chapter at line %d" % (self.ptr,))
                self.chap = self.doc.chapter()
                self.chap.add(line)
                continue

            if not self.chap and re_comment.match(line):
                continue

            if re_sectsep.match(line):
                if self.lines[self.ptr] == "%%" and \
                   self.lines[self.ptr+1].startswith("%E"):
                    break

                while not self.lines[self.ptr]:
                    self.ptr += 1
                line = self.lines[self.ptr]
                if not re_section.match(line):
                    raise RuntimeError("wrong line %d after %%%%" % (self.ptr+1,))

                self.sect = self.chap.new_section()
                self.sect.add(self.lines[self.ptr])

                self.ptr += 1
                continue

            if re_comment.match(line):
                continue

            if not self.sect and not line:
                continue

            if not self.sect:
                self.sect = self.chap.new_section()

            self.sect.add(line)


if __name__ == '__main__':
    files = ['/home/muntyan/Desktop/gap/gap4r4/doc/ref/matint.tex']
    files = ['/home/muntyan/Desktop/gap/gap4r4/doc/ref/rws.tex']
    files = ['/home/muntyan/Desktop/gap/gap4r4/doc/ref/about.tex']

    if sys.argv[1:]:
        files = sys.argv[1:]

    for f in files:
        if f.endswith("extcover.tex") or \
           f.endswith("ficover.tex"):
            continue
        print "file " + f
        parser = Parser()
        parser.parse(f)
        print "in file %s: %d sections" % (f, len(parser.doc.chapters[-1].sections),)
