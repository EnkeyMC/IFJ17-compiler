# Project: IFJ17
# Parser documentation

LL_GRAMMAR=ll_grammar
EXPR_GRAMMAR=expr_grammar

.PHONY: all clean

all: $(LL_GRAMMAR).pdf $(EXPR_GRAMMAR).pdf

$(LL_GRAMMAR).pdf: $(LL_GRAMMAR).tex
	latex $(LL_GRAMMAR).tex
	dvips $(LL_GRAMMAR).dvi
	ps2pdf $(LL_GRAMMAR).ps

$(EXPR_GRAMMAR).pdf: $(EXPR_GRAMMAR).tex
	latex $(EXPR_GRAMMAR).tex
	dvips $(EXPR_GRAMMAR).dvi
	ps2pdf $(EXPR_GRAMMAR).ps

clean:
	rm -f *.aux *.dvi *.ps *.log
