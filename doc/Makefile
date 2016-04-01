OBJ=main
SRC=$(OBJ).md
PDF=$(OBJ).pdf
TEX=$(OBJ).tex

all:
	pandoc -o $(PDF) $(SRC)

tex:
	pandoc --standalone -o $(TEX) $(SRC)

preview:
	qlmanage -p $(PDF) &> /dev/null

clean:
	rm -f $(OBJ).aux
	rm -f $(OBJ).log
	rm -f $(OBJ).out
	rm -f $(OBJ).pdf
