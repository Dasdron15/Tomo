cobalt.out: src/main.c src/editor.c src/utils/fileio.c src/utils/common.c
	gcc src/main.c src/editor.c src/utils/fileio.c src/utils/common.c -o cobalt.out -lcurses