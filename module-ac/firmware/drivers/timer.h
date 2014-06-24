#ifndef timer_h
#define timer_h

typedef void (*function_t)();

void add_listener(function_t function,unsigned int count);

void remove_listener(function_t function);

void init_timer_listener();

#endif
