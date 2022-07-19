# tiny-shell
## Run
```
gcc shell.c -o shell
shell
```
## Usage
#### Process command
```
pc -list
pc -{child, kill, stop, resume} <pid>
```
Press `Esc` to stop foreground process.
#### Execute .bat file
```
exc <dir>
```
#### Path command
```
path -all
path -add <name> <value>
```
#### Other command
`date`, `time`, `help`, `exit`, `cd`
