.PHONY: all build install uninstall clean

all: build

build:
	cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
	cmake --build build

install:
	sudo cmake --install build

uninstall:
	sudo rm -f /usr/local/bin/tomo
	sudo rm -rf /usr/local/lib/tomo-parsers

clean:
	rm -rf build
