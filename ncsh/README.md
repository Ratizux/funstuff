# ncsh

Homework of C programming for the spring semester during my freshman year.

Compile with `gcc -o main main.c -lncurses`. You should install development dependencies of ncurses first

The following content is the original documentation attached to the homework.

#### Digital clock

This displays current data & time on the top of your terminal in the following style:

`[2022-12-18 14:27.15]`

It will always display local time only.

#### MOTD (Message of The Day)

This will try to read messages from `/etc/motd` and display it on the screen. It is able to remove duplicate empty lines in the end of the file.

Simply edit source code if you want to change the default MOTD file path.

#### Run program

Use this to run specified program. Note: The `stdio` of specified program will disappear immediately after exited, so you may want to use the `Command shell` option below.

#### Command shell

It launches the "real" default command shell installed in the system. Note: The `$SHELL` environment variable must be set, or our program will not able to know the path of default shell.

#### Conway's Game of Life

Conway's Game of Life is a famous zero-player game invented by the scientist John Conway in 1970. In ncsh, this is implemented for the users to familiar with keyboard and help users to "put the fun back in computing itself".

Before you start to play, it's better to search on the web about it to have a basic knowledge about its rules.

This provided a 40x30 map, so you may have to maximize your terminal emulator window to play it.

You can use arrow keys to move the cursor, press `Z/X` to change cells' status(dead/alive) and press `P`(or `N` if you want) to see the changes.

Have fun!

#### New mail indicator

In Unix-like system, users can send mails to each other. This function indicates about the new mail.
