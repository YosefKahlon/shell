# shell


ID: 209011840 ID: 206795841


This project is a shell that is designed to function like a Linux terminal, 
allowing users to execute command-line instructions on their computer. 
The shell provides a command prompt, 
where users can enter commands and receive output from the system.

**Here is the instruction of how to use:**

- Before you start

<div dir='ltr'>

    make all


</div>

* How to run
<div dir='ltr'>

    ./myshell


</div>

## Features
The shell includes the following features, which are designed to emulate the behavior of a Linux terminal:

* Redirecting output: Users can redirect the output of a command to a file using >, For example, ls -l > output.txt will redirect the output of the ls command to a file named output.txt.

* Error redirection: Users can redirect the error output of a command to a file using 2>. For example, ls nofile 2> errors.txt will redirect the error output of the ls command to a file named errors.txt.

* Appending to files: Users can append to an existing file using >>. For example, ls -l >> output.txt will append the output of the ls command to a file named output.txt.
* Command to change prompt: Users can change the prompt using the prompt command. For example, 'prompt = hi' will set the prompt to 'hi'.
* echo command to print arguments.
* echo $? will print the status of the last command, where 0 indicates success.
* Change the working directory with the cd command. For example, cd mydir will change the working directory to 'mydir'.
* Repeat the last command using the !! command.
* Exit from the shell with the quit command.
* Option to chain several commands in a pipe.
* Adding variables to the shell: Users can add variables to the shell using the following syntax: 'variable=value'. For example, 'person=David' will create a variable named 'person' with the value 'David'.
* Navigate to the last 20 command with the up and down arrow.
* Conditional statements: Users can use the 'if' command to create conditional statements. For example, 'if date | grep Fri; then echo "Shabat Shalom"; else echo "Hard way to go"; fi' will print 'Shabat Shalom' if it's a Friday, and 'Hard way to go' otherwise.

### End

<div dir='ltr'>

    ./make clean

</div>
