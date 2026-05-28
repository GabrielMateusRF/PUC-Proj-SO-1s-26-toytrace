#include "student_api.h"
#include "syscall_names.h"
#include <stdio.h>
#include <sys/syscall.h>
#include "trace_helpers.h"
#include <string.h>

void student_debug_raw_event(const struct syscall_event *ev,
                             char *buf,
                             size_t bufsz)
{
    /*
     * Suporte de depuracao para a Semana 4:
     *
     * Esta funcao existe para inspecionar eventos crus depois que o runtime
     * ja consegue parar em syscalls e preencher struct syscall_event.
     * Ela nao e a formatacao final do projeto.
     *
     * Experimento sugerido:
     * - imprima o nome da syscall;
     * - imprima se o evento e entrada ou saida;
     * - imprima o pid;
     * - em eventos de entrada, observe os argumentos;
     * - em eventos de saida, observe o valor de retorno.
     *
     * Depois compare a saida de:
     *
     *   ./toytrace trace --raw-events -- ./tests/targets/hello_write
     *
     * A pergunta importante da Semana 4 e:
     * por que a mesma syscall aparece duas vezes?
     // RESPOSTA: A mesma syscall aparece duas vezes porque ha um 
     // PTRACE_SYSCALL para o processo em dois momentos, uma vez na entrada da syscall
     // antes de ela executar, e outra vez na saída, depois que ela termina.
     */
    if (ev->entering) {
        snprintf(buf, bufsz, "pid=%d %s entrada",
                 ev->pid,
                 syscall_name(ev->syscall_no));
        return;
    }

    // O retorno so e valido na saida da syscall.
    // Melhoria no debuger para poder ver melhor o retorno das sysacalls
    snprintf(buf, bufsz, "pid=%d %s saida ret=%ld",
             ev->pid,
             syscall_name(ev->syscall_no),
             ev->ret);
}

void student_format_event(const struct syscall_event *ev,
                          char *buf,
                          size_t bufsz)
{
    /*
     * TODO Semana 5: Feito
     *
     * Primeiro, formate uma syscall completa em uma linha simples.
     *
     * Depois, adicione casos especiais para:
     *     read(fd, buf, count)
     *     write(fd, buf, count)
     *     openat(dirfd, "path", flags, mode)
     *     execve("path", ...)
     *     exit_group(status)
     *
     * Para caminhos do processo monitorado, use read_child_string().
     * Se a leitura falhar, imprima "<ilegivel>".
     */
    // read usa apenas fd, endereco do buffer e quantidade solicitada.
    if (ev->syscall_no == SYS_read) {
        snprintf(buf, bufsz, "read(%lu, %#lx, %lu) = %ld",
                 ev->args[0],
                 ev->args[1],
                 ev->args[2],
                 ev->ret);
        return;
    }

    // write tem a mesma assinatura basica de read.
    if (ev->syscall_no == SYS_write) {
        snprintf(buf, bufsz, "write(%lu, %#lx, %lu) = %ld",
                 ev->args[0],
                 ev->args[1],
                 ev->args[2],
                 ev->ret);
        return;
    }

    // Em openat, args[1] nao guarda o texto do caminho diretamente.
    // Ele guarda um endereco dentro da memoria do processo monitorado.
    if (ev->syscall_no == SYS_openat) {
        char path[256];

        // O tracer precisa ler esse endereco no filho para recuperar a string.
        if (read_child_string(ev->pid, ev->args[1], path, sizeof(path)) < 0) {
            strncpy(path, "<ilegivel>", sizeof(path));
        }

        // dirfd pode representar constantes negativas, como AT_FDCWD (-100)
        // Como ele chega armazenado em unsigned long, imprimimos como int para
        // recuperar o valor correto no formato da syscall
        // (feito o mesmo para SYS_exit_group, que tem um argumento de status que pode ser negativo)
        snprintf(buf, bufsz, "openat(%d, \"%s\", %#lx, %#lx) = %ld",
                 (int)ev->args[0],
                 path,
                 ev->args[2],
                 ev->args[3],
                 ev->ret);
        return;
    }

    // No execve, o path ja foi capturado pelo pairer na entrada da syscall.
    // Ler args[0] aqui pode falhar, porque apos execve a imagem do processo muda.
    if (ev->syscall_no == SYS_execve) {
        snprintf(buf, bufsz, "execve(\"%s\", ...) = %ld",
                 ev->has_path ? ev->path : "<ilegivel>",
                 ev->ret);
        return;
    }

    // exit_group encerra o processo e so precisa mostrar o status em args[0]. feito
    if(ev->syscall_no == SYS_exit_group) {
        snprintf(buf, bufsz, "exit_group(%d) = %ld",
                (int)ev->args[0],
                ev->ret);
        return;
    }
    //tentei deixar no padrao dos outros
    //exit group parece chatinho https://man7.org/linux/man-pages/man2/exit_group.2.html

    // Syscalls sem caso especial continuam usando os seis argumentos crus.
    snprintf(buf, bufsz, "%s(%#lx, %#lx, %#lx, %#lx, %#lx, %#lx) = %ld",
             syscall_name(ev->syscall_no),
             ev->args[0],
             ev->args[1],
             ev->args[2],
             ev->args[3],
             ev->args[4],
             ev->args[5],
             ev->ret);
}
