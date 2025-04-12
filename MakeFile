cobalt.out: src/main.c src/editor.c src/fileio.c
	gcc src/main.c src/editor.c src/fileio.c -o cobalt.out -lcurses