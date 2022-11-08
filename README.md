# Terminal-Based-file-explorer
A terminal based file explorer for Linux based systems. Implemented two modes for the application- normal mode and command mode.


							AOS ASSIGNMENT 1

		CREATEFILE COMMANDS
1)	create_file test1.txt .
2)	create_file test2.txt ./
3)	create_file test3.txt ./sample
4)	create_file test4.txt ./sample/
5)	create_file test5.txt ~
6)	create_file test6.txt ~/
7)	create_file test7.txt ~/sample
8)	create_file test8.txt ~/sample/
9)	create_file test9.txt /home/nikhil
10)	create_file test10.txt /home/nikhil/
11)	create_file test11.txt /


		CREATEDIR COMMANDS
1)	create_dir sample1 .
2)	create_dir sample2 ./
3)	create_dir sample3 ./sample
4)	create_dir sample4 ./sample/
5)	create_dir sample5 ~
6)	create_dir sample6 ~/
7)	create_dir sample7 ~/sample
8)	create_dir sample8 ~/sample/
9)	create_dir sample9 /home/nikhil
10)	create_dir sample10 /home/nikhil/
11)	create_dir sample11 /

		COPY COMMANDS
1)	copy test1.txt test2.txt test3.txt ~/sample
2)	copy sample1 sample2 sample3	~/sample
3)	copy test1.txt test2.txt test3.txt ./sample
4)	copy sample1 sample2 sample3	./sample
5)	copy test1.txt test2.txt test3.txt /home/nikhil/sample
6)	copy sample1 sample2 sample3	/home/nikhil/sample

		MOVE COMMANDS
1)	move test1.txt test2.txt test3.txt ~/sample
2)	move sample1 sample2 sample3	~/sample
3)	move test1.txt test2.txt test3.txt ./sample
4)	move sample1 sample2 sample3	./sample
5)	move test1.txt test2.txt test3.txt /home/nikhil/sample
6)	move sample1 sample2 sample3	/home/nikhil/sample

		RENAME COMMANDS
1)	rename test1.txt test2.txt
2)	rename sample1 sample2

		GOTO COMMANDS
1) 	goto /home/nikhil
2) 	goto /home/nikhil/AOS/nikhil
3)	goto /home
	
		SEARCH COMMANDS
1)	search AOS
2)	search nikhil
3)	search test7.txt
	
		DELETE COMMANDS
1)	delete_dir ~/sample
2)	delete_dir ./sample
3)	delete_dir /home/nikhil/sample
4)	delete_file ~/test1.txt
5)	delete_file ./test2.txt
6)	delete_file /home/nikhil/test3.txt



# Linux based File Explorer
A file explorer for linux systems that runs on terminal
## Prerequisites

**1. Platform:** Linux 

**2. Software Requirement:**
* G++ compiler
   * **To install G++ :** ```sudo apt-get install g++```

## Steps to run the project
- Open your terminal with present working directory as the project folder. Then run the **g++ nikhil.cpp** command to execute the file.
- Then finally run the project as shown below, 
    **./a.out**
## Assumptions
* In Command mode, to edit the command use backspace only.
* Preferable to open terminal in full size. if one record spans over more than one line then navigation won't work correctly.
* if **Enter** is pressed on some file and if there is no default application found then there will be no movement.
