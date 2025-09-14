# Tomo

![GitHub Release](https://img.shields.io/github/v/release/Dasdron15/Tomo?color=blue)
![GitHub License](https://img.shields.io/github/license/Dasdron15/Tomo?color=blue)

**Tomo** is a terminal-based code editor focused on simplicity and working out of the box without any configuration. Inspired by the lightweight nature of Helix and the intuitiveness of VS Code, it uses `ncurses` for terminal UI and `tree-sitter` for fast syntax highlighting.

Image of Tomo code editor in action:

![Screenshot](./assets/preview.png)

## Features

- Syntax highlighting
- Smart indentation
- Custom themes
- Command palette

## Themes

**Tomo** supports custom themes via `.ini` files. Place your theme files in `~/.config/tomo/themes` directory.

### Theme file format

Each theme is a simple `.ini` file with the following format:

```ini
default = "#d0d0d0"
keyword = "#ff5555"
type = "#87cefa"
custom_type = "#ffa500"
string = "#98fb98"
number = "#f0e68c"
char = "#98fb98"
function = "#dda0dd"
identifier = "#b0c4de"
preprocessor = "#7fffd4"
comment = "#808080"
unactive = "#555555"
status_bar_bg = "#202020"
status_bar_text = "#d0d0d0"
background = "#121212"
select = "#333333"
```

- Colors should be in hexadecimal format (`#RRGGBB`).
- `default` - text default color  
- `keyword` - programming language keywords  
- `type` - built-in types (like `int`, `float`)  
- `custom_type` - user-defined types (like structs, classes)  
- `string` - string literals  
- `number` - numeric literals  
- `char` - character literals  
- `function` - function names  
- `identifier` - variable and parameter names  
- `preprocessor` - preprocessor directives (C/C++)  
- `comment` - Comments
- `unactive` - Unactive line numbers 
- `status_bar_bg` - status bar background  
- `status_bar_text` - status bar text color  
- `background` - editor background  
- `select` - selected text background

## Shortcuts

- <kbd>Ctrl + C</kbd> - Copy selected text
- <kbd>Ctrl + V</kbd> - Paste selected text
- <kbd>Ctrl + X</kbd> - Cut selected text
- <kbd>Ctrl + Z</kbd> - Undo changes
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

- Folder/project support
- config file support
- Mouse support
- Syntax highlighting for more languages
- Auto-completion
- Vim mode

## Contribution

Contributions, bug reports and feature requests are welcome!
