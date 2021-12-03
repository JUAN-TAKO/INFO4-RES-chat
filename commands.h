#ifndef COMMANDS_H
#define COMMANDS_H

typedef enum{
    Q_NAME = 1,
    A_NAME = 2, 
    MSG_TO = 3,
    MSG_FROM = 4,
    Q_LIST = 5,
    A_LIST = 6,
    INFO = 10,
    ERROR = 11,
    BYE = 100
} commands_e;

#endif