PYTHON = /usr/bin/python
CHEETAH = /usr/bin/cheetah

COMMON = $(BUILD)/base.py
HTML_CHAPS = mooscript.html usermenu.html activestrings.html
HTML_CHAPS_COMPILED = $(BUILD)/mooscript_html.py $(BUILD)/usermenu_html.py $(BUILD)/activestrings_html.py
HTML_CHAPS_TEMPLATES = $(BUILD)/mooscript_html.tmpl $(BUILD)/usermenu_html.tmpl $(BUILD)/activestrings_html.tmpl
COMPILED = $(BUILD)/base.py $(BUILD)/mooscript.py $(BUILD)/usermenu.py $(BUILD)/activestrings.py $(BUILD)/index.py

BUILD = build

all: html

html: $(HTML_CHAPS) index.html

$(COMPILED): $(BUILD)/%.py: %.tmpl
	mkdir -p $(BUILD)
	cp manual.py $(BUILD)/
	$(CHEETAH) c $< --stdout > $@

$(HTML_CHAPS_COMPILED): $(BUILD)/%.py: $(BUILD)/%.tmpl
	mkdir -p $(BUILD)
	cp manual.py $(BUILD)/
	$(CHEETAH) c $< --stdout > $@

index.html: $(BUILD)/index_html.py manual.py
	$(PYTHON) $(BUILD)/index_html.py > $(BUILD)/$@ && mv $(BUILD)/$@ $@

$(BUILD)/index_html.py: index_html.tmpl
	mkdir -p $(BUILD)
	cp manual.py $(BUILD)/
	$(CHEETAH) c $< --stdout > $@

$(HTML_CHAPS): %.html: $(BUILD)/%.py $(BUILD)/%_html.py $(COMMON)
	$(PYTHON) $(BUILD)/$*_html.py > $(BUILD)/$@ && mv $(BUILD)/$@ $@

$(HTML_CHAPS_TEMPLATES): $(BUILD)/%_html.tmpl: html.tmpl gen_html_tmpl manual.py
	mkdir -p $(BUILD)
	cp manual.py $(BUILD)/
	$(PYTHON) gen_html_tmpl $* > $@

clean:
	rm -rf $(BUILD) *.pyc *~ *.html

.PHONY: all html clean
