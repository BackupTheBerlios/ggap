PYTHON = /usr/bin/python
CHEETAH = /usr/bin/cheetah

COMPILED = $(BUILD)/mooscript.py $(BUILD)/usermenu.py $(BUILD)/activestrings.py $(BUILD)/index.py

HTML_CHAPS = mooscript.html usermenu.html activestrings.html
HTML_CHAPS_COMPILED = $(BUILD)/mooscript_html.py $(BUILD)/usermenu_html.py $(BUILD)/activestrings_html.py
HTML_CHAPS_TEMPLATES = $(BUILD)/mooscript_html.tmpl $(BUILD)/usermenu_html.tmpl $(BUILD)/activestrings_html.tmpl

TEX_CHAPS = mooscript.tex usermenu.tex activestrings.tex
TEX_CHAPS_COMPILED = $(BUILD)/mooscript_tex.py $(BUILD)/usermenu_tex.py $(BUILD)/activestrings_tex.py
TEX_CHAPS_TEMPLATES = $(BUILD)/mooscript_tex.tmpl $(BUILD)/usermenu_tex.tmpl $(BUILD)/activestrings_tex.tmpl

BUILD = build

all: tex

html: $(HTML_CHAPS) index.html

$(COMPILED): $(BUILD)/%.py: %.tmpl manual.py
	mkdir -p $(BUILD)
	cp manual.py $(BUILD)/
	$(CHEETAH) c $< --stdout > $@

$(HTML_CHAPS_COMPILED): $(BUILD)/%.py: $(BUILD)/%.tmpl manual.py
	mkdir -p $(BUILD)
	cp manual.py $(BUILD)/
	$(CHEETAH) c $< --stdout > $@

index.html: $(BUILD)/index_html.py manual.py
	$(PYTHON) $(BUILD)/index_html.py > $(BUILD)/$@ && mv $(BUILD)/$@ $@

$(BUILD)/index_html.py: index_html.tmpl manual.py
	mkdir -p $(BUILD)
	cp manual.py $(BUILD)/
	$(CHEETAH) c $< --stdout > $@

$(HTML_CHAPS): %.html: $(BUILD)/%.py $(BUILD)/%_html.py manual.py
	$(PYTHON) $(BUILD)/$*_html.py > $(BUILD)/$@ && mv $(BUILD)/$@ $@

$(HTML_CHAPS_TEMPLATES): $(BUILD)/%_html.tmpl: html.tmpl gen_html_tmpl manual.py
	mkdir -p $(BUILD)
	cp manual.py $(BUILD)/
	$(PYTHON) gen_html_tmpl $* > $@

tex: $(TEX_CHAPS) ggap.tex

$(TEX_CHAPS_COMPILED): $(BUILD)/%.py: $(BUILD)/%.tmpl manual.py
	mkdir -p $(BUILD)
	cp manual.py $(BUILD)/
	$(CHEETAH) c $< --stdout > $@

ggap.tex: $(BUILD)/ggap_tex.py manual.py
	$(PYTHON) $(BUILD)/ggap_tex.py > $(BUILD)/$@ && mv $(BUILD)/$@ $@

$(BUILD)/ggap_tex.py: ggap_tex.tmpl manual.py
	mkdir -p $(BUILD)
	cp manual.py $(BUILD)/
	$(CHEETAH) c $< --stdout > $@

$(TEX_CHAPS): %.tex: $(BUILD)/%.py $(BUILD)/%_tex.py manual.py
	$(PYTHON) $(BUILD)/$*_tex.py > $(BUILD)/$@ && mv $(BUILD)/$@ $@

$(TEX_CHAPS_TEMPLATES): $(BUILD)/%_tex.tmpl: tex.tmpl gen_tex_tmpl manual.py
	mkdir -p $(BUILD)
	cp manual.py $(BUILD)/
	$(PYTHON) gen_tex_tmpl $* > $@

clean:
	rm -rf $(BUILD) *.pyc *~ *.html *.tex *.log *.aux *.out

.PHONY: all html tex clean
