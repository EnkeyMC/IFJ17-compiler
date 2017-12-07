# Project: IFJ17
# Parser documentation

TARGET=dokumentace

.PHONY: all clean

all: $(TARGET).pdf

$(TARGET).pdf: $(TARGET).tex
	latex $(TARGET).tex
	latex $(TARGET).tex
	dvips $(TARGET).dvi
	ps2pdf $(TARGET).ps

clean:
	rm -f *.aux *.dvi *.ps *.log
