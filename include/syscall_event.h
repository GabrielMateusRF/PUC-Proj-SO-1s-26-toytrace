#ifndef SYSCALL_EVENT_H
#define SYSCALL_EVENT_H

#include <sys/types.h>

struct syscall_event {
    pid_t pid;
    int entering;              /* 1 na entrada da syscall, 0 na saida */
    long syscall_no;
    long ret;                  /* valido apenas em eventos de saida */
    unsigned long args[6];     /* argumentos capturados na entrada */
    int has_path;              /* 1 se path contem uma string valida */
    char path[256];            /* caminho capturado antes de execve trocar a imagem */
};

#endif
