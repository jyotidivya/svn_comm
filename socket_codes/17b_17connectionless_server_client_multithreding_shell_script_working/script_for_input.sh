#!/bin/bash

# script to read from file 
# function to read from file
lineNumber=151

Read_From_File()
{
	filename="$1"
	
	while [ $lineNumber -gt 0 ]
	do
		IFS='' read -r line 
		echo "$line"
		lineNumber=$(( $lineNumber - 1 )) 

	done < "$filename"	

}

# main body to call the function
sleep 3s
Read_From_File /home/divya/Desktop/svn_git_cmds.txt
# echo $lineNumber



  
