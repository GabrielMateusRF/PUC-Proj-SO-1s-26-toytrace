#include "student_api.h"

int student_pair_syscall(struct syscall_pairer *pairer,
                         const struct syscall_event *ev,
                         struct syscall_event *out)
{
    /*
     * TODO Semana 2: feito
     *
     * O runtime chama esta funcao duas vezes para cada syscall:
     *
     *   1. uma vez antes da syscall executar
     *   2. uma vez depois da syscall terminar
     *
     * Na primeira parada, os argumentos estao disponiveis.
     * Na segunda parada, o retorno esta disponivel.
     *
     * Seu trabalho e produzir um evento completo apenas quando ja existirem
     * as duas metades da syscall.
     *
     * Dicas:
     * - ev->entering == 1 indica entrada de syscall.
     * - ev->entering == 0 indica saida de syscall.
     * - para comecar, assuma apenas um processo monitorado.
     *
     * Retorne:
     *   1 se out contem uma syscall completa
     *   0 se ainda nao ha syscall completa
     *  -1 se a sequencia de eventos parece invalida
     */
    // Valida as variaveis recebidas
    if (pairer == NULL || ev == NULL || out == NULL) {
        fprintf(stderr, "erro: argumentos invalidos para student_pair_syscall()\n");
        return -1;
    }

    // Evento de entrada: guarda a syscall, porque aqui estao os argumentos.
    if (ev->entering == 1) {
        pairer->entry = *ev;
        pairer->has_entry = 1;
        return 0;
    }

    // Evento de saida: precisa existir uma entrada salva antes.
    if (ev->entering == 0) {
        if (pairer->has_entry == 0) {
            fprintf(stderr, "erro: sequencia de eventos invalida em student_pair_syscall()\n");
            return -1;
        }

        // A saida deve ser da mesma syscall e do mesmo processo da entrada.
        if (ev->pid != pairer->entry.pid || ev->syscall_no != pairer->entry.syscall_no) {
            fprintf(stderr, "erro: sequencia de eventos invalida em student_pair_syscall()\n");
            pairer->has_entry = 0;
            return -1;
        }

        // O evento completo usa os argumentos da entrada e o retorno da saida.
        *out = pairer->entry;
        out->entering = 0;
        out->ret = ev->ret;
        pairer->has_entry = 0;
        return 1;
    }

    fprintf(stderr, "erro: tipo de evento invalido em student_pair_syscall()\n");
    return -1;
}
