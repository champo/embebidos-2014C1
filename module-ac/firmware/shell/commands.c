#include "commands.h"

#include "string.h"

command_t command_list[COMMAND_MAX_CANT];
int commands_added=0;

void get_command_list(char* commands[]) {
    int i;
    for(i=0;i<commands_added;i++) {
        commands[i] = command_list[i].name;
    }
    commands[i] = NULL;
}

void add_command(char * name,start_f function){
	if(commands_added<COMMAND_MAX_CANT){
		command_list[commands_added].name=name;
		command_list[commands_added].start=function;
		commands_added++;
	}
}

start_f get_command(char * name){
	int i;
	for(i=0;i<commands_added;i++){
		if(!strcmp(command_list[i].name,name)){
			return command_list[i].start;
		}
	}
	return NULL;
}
