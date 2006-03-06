PYTHON = /usr/bin/python
CHEETAH = /usr/bin/cheetah

COMMON = base.py
HTML = mooscript.html usermenu.html
HTML_COMPILED = mooscript_html.py usermenu_html.py
HTML_TEMPLATES = mooscript_html.tmpl usermenu_html.tmpl
COMPILED = base.py mooscript.py usermenu.py $(HTML_COMPILED)

all: html

html: $(HTML)

$(COMPILED): %.py: %.tmpl
	$(CHEETAH) c $<

$(HTML): %.html: %.py %_html.py $(COMMON)
	$(PYTHON) $*_html.py > $@.tmp && mv $@.tmp $@

$(HTML_TEMPLATES): %_html.tmpl: html.tmpl
	$(PYTHON) gen_html_tmpl $* > $@

clean:
	rm -f *.html *.py *~ *.bak *.pyc *.tmp *_html.*

.PHONY: all html clean
