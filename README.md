# Tomo

![GitHub Release](https://img.shields.io/github/v/release/Dasdron15/Tomo?color=blue)
![GitHub License](https://img.shields.io/github/license/Dasdron15/Tomo?color=blue)

**Tomo** is a terminal-based code editor focused on simplicity and working out of the box without any configuration. Inspired by the lightweight nature of Helix and the intuitiveness of VS Code, it uses `ncurses` for terminal UI and `tree-sitter` for fast syntax highlighting.

Image of Tomo code editor in action:

![Screenshot](./assets/preview.png)

## Features

- Syntax highlighting (basic support for Python and C)  
- Smart indentation

## Shortcuts

- <kbd>Ctrl + C</kbd> - Copy selected text
- <kbd>Ctrl + V</kbd> - Paste selected text
- <kbd>Ctrl + X</kbd> - Cut selected text
- <kbd>Ctrl + G</kbd> - Go to line
- <kbd>Ctrl + Q</kbd> - Quit the editor
- <kbd>Ctrl + S</kbd> - Save the file
- <kbd>Ctrl + T</kbd> - Change theme
- <kbd>Ctrl + P</kbd> - Open command palette
- <kbd>Arrow keys</kbd> - Move cursor
- <kbd>Arrow keys + Shift</kbd> - Start text selection

## Installation

```sh
git clone https://github.com/Dasdron15/Tomo.git
cd <path/to/tomo>
make
sudo make install
```

`make` builds the editor and `sudo make install` installs it system-wide

## Usage 

```sh
tomo <path/to/file>
```

Currently, folder/project opening is not supported but planned for future releases.

## Planned features

- Undo/Redo
- Folder/project support
- Mouse support
- Syntax highlighting for more languages
- Auto-completion
- Vim mode

## Contribution

Contributions, bug reports and feature requests are welcome!
