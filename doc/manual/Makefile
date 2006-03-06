PYTHON = /usr/bin/python
CHEETAH = /usr/bin/cheetah

COMMON = base.py
HTML = mooscript.html usermenu.html activestrings.html
HTML_COMPILED = mooscript_html.py usermenu_html.py activestrings_html.py index_html.py
HTML_TEMPLATES = mooscript_html.tmpl usermenu_html.tmpl activestrings_html.tmpl
COMPILED = base.py mooscript.py usermenu.py activestrings.py index.py $(HTML_COMPILED)

all: html

html: $(HTML) index.html

$(COMPILED): %.py: %.tmpl
	$(CHEETAH) c $<

index.html: index_html.py
	$(PYTHON) index_html.py > $@.tmp && mv $@.tmp $@

$(HTML): %.html: %.py %_html.py $(COMMON)
	$(PYTHON) $*_html.py > $@.tmp && mv $@.tmp $@

$(HTML_TEMPLATES): %_html.tmpl: html.tmpl gen_html_tmpl
	$(PYTHON) gen_html_tmpl $* > $@

clean:
	rm -f *.html $(COMPILED) $(HTML_TEMPLATES) *~ *.bak *.pyc *.tmp

.PHONY: all html clean
