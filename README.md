# Custom Shell Creation
This shell will help one learn about processes and basic process management in a shell.

How To Run?  
-----------  
1. compile the customshell.c file using GCC  
   ```sh
   gcc -o customshell customshell.c  
   ```
2. Run the object file created.  
   ```sh
   ./customshell  
   ```


Usage of Shell  
--------------
This shell (say, `banu>`) will be doing all the following -

1. Execute commands with multiple arguments. For example:  
	```sh
	banu> Command arg1 arg2 arg3  
	(Output of Command shown here. Your shell waits for Command to finish)  
	banu>  
	```
	
2. Execute commands in either foreground or background mode. In foreground mode, the shell just waits for the command to complete before displaying the shell prompt again (as in the above example). In background mode, a command is executed with an ampersand & suffix. The shell prompt appears immediately after typing a command name (say Command1) and shell becomes ready to accept and execute the next command (say Command2), even as Command1 continues executing in the background. For example:  
	```sh
	banu> Command1 &  
	banu> Command2  
	(Output of Command1 and Command2 may interleave here in arbitrary order.
	Shell waits for Command 2 to finish.)  
	banu>  
	```
		
3. Maintain multiple processes running in background mode simultaneously. For example:  
	```sh
	banu> Command1 &  
	banu> Command2 &  
	banu> Command3 &  
	banu>   
	(Output of Command1, Command2, and Command3 may interleave here in arbitrary order.
	Shell does not wait for any of the commands to finish.)  
	```  

		
4. List all currently running background jobs using "listjobs" command.  
	```sh   
	banu> Command1 &  
	banu> Command2 &  
	banu> Command3 &  
	banu> listjobs  
	List of backgrounded processes:  
	Command 1 with PID 1000 Status:RUNNING  
	Command 2 with PID 1005 Status:RUNNING  
	Command 3 with PID 1007 Status:FINISHED  
	banu>  
	```   
		
5. Bring a background process to foreground using the fg command with process ID as argument. For instance, continuing from the previous example:  
	```sh   
	banu> fg 1005  
	(Shell waits for Command2 to finish.)  
	banu>  
	```   

6. I/O Redirection:  
	a. Redirect the input of a command from a file. For example:  
	```sh  
	banu> Command < input_file  
	```  
		
	b. Redirect the output of a command to a file. For example:  
	```sh  
	banu> Command > output_file
	```  