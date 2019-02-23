
all:
	  clang unibmr.c -o unibmr 


slides:
	  clang unibmr.c -o unibmr 
	  ./unibmr file.bmr
	   pdflatex file.tex
	   mupdf file.pdf


