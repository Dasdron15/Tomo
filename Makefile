.PHONY: all build install uninstall clean

all: build

build:
	cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
	cmake --build build

install:
	sudo cmake --install build

uninstall:
	sudo rm -f /usr/local/bin/tomo
	sudo rm -rf /usr/local/lib/tomo-parsers
	sudo rm -rf ~/.config/tomo

clean:
	rm -rf build
