#ifndef COMMANDS_H
#define COMMANDS_H

typedef int (*start_f)(int argc,char * argv[]);


struct command_struct {
    char * name;
    start_f start;
};

typedef struct command_struct command_t;

void add_command(char * name,start_f function);
start_f get_command(char * name);
void get_command_list(char* commands[]);

char * autocomplete(char * name);


#define COMMAND_MAX_CANT 20

#endif //COMMANDS_H
