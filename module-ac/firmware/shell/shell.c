#include "shell.h"

#include "string.h"
#include "commands.h"
#include "stdio.h"

#include "../drivers/buffered_uart.h"
#include "../drivers/timer.h"

#include <avr/io.h>

#include <stdlib.h>

#define COMAND_LINE_MAX 100
#define HISTORY_MAX 10

char* history[HISTORY_MAX][COMAND_LINE_MAX];
int history_current = 0;
int history_count = 0;

char * name="user";
char * pcname="thispc";

char * strnormalise(char * str){
        int j, i;
        // cambia enters por espacios
        for(j=0;str[j]!='\0';j++){
                if(str[j]=='\n' || str[j] == '\t'){
                        str[j]=' ';
                }
        }
        // elimina espacios del principio
        while(str[0]==' '){
                str=str+1;
        }
        //elimina espacios del final
        for(i=strlen(str)-1;i>0 && str[i]==' ';i--){
                str[i]='\0';
        }
        //elimina espacios repetidos en el medio
        for(j=0;str[j]!='\0';j++){
                if(str[j]==' ' && str[j+1]==' '){
                        strcpy(str + j, str + j + 1);
                        j--;
                }
        }
        return str;
}

void printuser(){
	char buffer[100];
	sprintf(buffer, "\x1B[32m%s@%s:\x1B[0m",name,pcname);
	uart_write(buffer, strlen(buffer));
}

int execute(char* comand,int argcant,char * argvec[]){
	if(comand[0]=='\0'){
		return 0;
	}
	start_f start=get_command(comand);
	if(start==NULL){
		char buffer[100];
		sprintf(buffer,"invalid comand: %s\n\r",comand);
		uart_write(buffer, strlen(buffer));
		return -1;
	}
	return start(argcant,argvec);
}

int parseline(){
	char c;
	int i=0;
	char comand_line[COMAND_LINE_MAX];
	uart_read(&c,1);
	while(c!='\r' && i<COMAND_LINE_MAX-3){
		comand_line[i]=c;
		uart_write(&c,1);
		i++;
		uart_read(&c,1);
	}
	comand_line[i]='\0';
	uart_write("\n\r", strlen("\n\r"));
	char* command=strnormalise(comand_line);
	int argcant=0;
	char * argvec[50];
        int in_quotes = 0;
	for(i=0;command[i]!='\0';i++){
		if(command[i]==' ' && !in_quotes){
			command[i]='\0';
			argvec[argcant]=&command[i+1];
			argcant++;
                } else if (command[i]=='"') {
                    in_quotes = !in_quotes;
                }
	}
	return execute(command,argcant,argvec)==-15;
}

int exit_shell(int argc,char* argv[]){
	  return -15;
}

int echo_shell(int argc,char* argv[]){
	int i;
	for(i=0;i<argc;i++){
		char buffer[100];
		sprintf(buffer,"%s\n\r",argv[i]);
		uart_write(buffer, strlen(buffer));
	}
	return 0;
}


int clear_shell(){
	char buffer[100];
	sprintf(buffer,"\x1B[2J");
	uart_write(buffer, strlen(buffer));
	return 0;
}

int help_shell(){
    uart_write("These are the commands available: \n\r",strlen("These are the commands available: \n\r"));
    char* commands[COMMAND_MAX_CANT];
    get_command_list(commands);
    int i = 0;
    while(commands[i]) {
    	char buffer[100];
        sprintf(buffer,"\x1B[4m%s\x1B[0m\t\n\r", commands[i++]);
		uart_write(buffer, strlen(buffer));
    }
    return 0;
}


void toggle_led(){
	//toggle led
    PORTD ^= (0x80);
}


int toggle_shell(int argc,char* argv[]){
	if(argc>0){
		add_listener(toggle_led,atoi(argv[0]));
		char c;
		do{
			uart_read(&c,1);
		}while(c!='q');
		remove_listener(toggle_led);
	}
	return 0;
}

void shell_start(){
	int exit=0;
	add_command("echo", echo_shell);	
	add_command("exit", exit_shell);
	add_command("clear", clear_shell);
	add_command("help", help_shell);
	add_command("toggle", toggle_shell);
	while(!exit)
	{
		printuser();
		exit=parseline();
	}
}



