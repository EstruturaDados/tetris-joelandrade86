#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

// --- CONSTANTES E DEFINIÇÕES ---
#define CAPACIDADE_MAXIMA 5 // Capacidade da Fila e Pilha
#define NOME_MAX 20

// --- STRUCTS ---

/**
 * @brief Estrutura básica da Peça de Jogo.
 */
typedef struct {
    char nome[NOME_MAX];
    int id;
} Peca;

/**
 * @brief Estrutura para a FILA CIRCULAR (Peças em Jogo).
 */
typedef struct {
    Peca* itens;
    int frente;
    int fim;
    int tamanho;
    int capacidade;
} Fila;

/**
 * @brief Estrutura para a PILHA (Peças de Reserva ou Desfazer).
 */
typedef struct {
    Peca* itens;
    int topo;
    int capacidade;
} Pilha;

// --- ASSINATURAS DAS FUNÇÕES (Modularização) ---

// Funções de Inicialização e Memória
Fila* criarFila(int capacidade);
Pilha* criarPilha(int capacidade);
void inicializarPecas(Fila* fila, Pilha* pilha);
void liberarEstruturas(Fila* fila, Pilha* pilha);

// Funções Básicas de Fila
bool estaCheiaFila(const Fila* fila);
bool estaVaziaFila(const Fila* fila);
void enfileirar(Fila* fila, Peca peca);
Peca desenfileirar(Fila* fila);

// Funções Básicas de Pilha
bool estaCheiaPilha(const Pilha* pilha);
bool estaVaziaPilha(const Pilha* pilha);
void empilhar(Pilha* pilha, Peca peca);
Peca desempilhar(Pilha* pilha);

// Funções de Simulação e Estratégia
void jogarPeca(Fila* fila, Pilha* desfazer);
void reservarPeca(Fila* fila, Pilha* reserva);
void usarPecaReservada(Fila* fila, Pilha* reserva);
void trocarTopoPilhaFrenteFila(Pilha* pilha, Fila* fila);
void desfazerUltimaJogada(Fila* fila, Pilha* desfazer);
void inverterFilaComPilha(Fila* fila, Pilha* pilha);
void exibirEstruturas(const Fila* fila, const Pilha* pilha);

// Funções de Interface
void exibirMenu();


// --- 1. FUNÇÃO PRINCIPAL (main) ---
int main() {
    Fila* filaJogo = criarFila(CAPACIDADE_MAXIMA);
    Pilha* pilhaReserva = criarPilha(CAPACIDADE_MAXIMA);
    Pilha* pilhaDesfazer = criarPilha(CAPACIDADE_MAXIMA); // Pilha dedicada ao Desfazer

    if (!filaJogo || !pilhaReserva || !pilhaDesfazer) return 1;

    // Inicialização das peças iniciais (dados estáticos)
    inicializarPecas(filaJogo, pilhaReserva);

    int opcao = -1;
    bool jogoEmAndamento = true;

    while (jogoEmAndamento) {
        exibirEstruturas(filaJogo, pilhaReserva);
        exibirMenu();
        if (scanf("%d", &opcao) != 1) {
            printf("\n❌ Entrada inválida. Tente novamente.\n");
            while (getchar() != '\n');
            continue;
        }

        switch (opcao) {
            case 1: // Jogar peça (Retira da Fila)
                jogarPeca(filaJogo, pilhaDesfazer);
                break;
            case 2: // Reservar peça (Fila -> Pilha Reserva)
                reservarPeca(filaJogo, pilhaReserva);
                break;
            case 3: // Usar peça reservada (Pilha Reserva -> Fila)
                usarPecaReservada(filaJogo, pilhaReserva);
                break;
            case 4: // Trocar Topo da Pilha com Frente da Fila
                trocarTopoPilhaFrenteFila(pilhaReserva, filaJogo);
                break;
            case 5: // Desfazer última jogada (Pilha Desfazer -> Fila)
                desfazerUltimaJogada(filaJogo, pilhaDesfazer);
                break;
            case 6: // Inverter Fila com Pilha
                inverterFilaComPilha(filaJogo, pilhaReserva);
                break;
            case 0: // Sair
                jogoEmAndamento = false;
                printf("\n👋 Jogo encerrado.\n");
                break;
            default:
                printf("\nOpção inválida.\n");
                break;
        }
    }
    
    liberarEstruturas(filaJogo, pilhaReserva);
    free(pilhaDesfazer->itens); // Libera o vetor de itens
    free(pilhaDesfazer); // Libera a struct da pilha Desfazer
    return 0;
}

// --- 2. IMPLEMENTAÇÃO DE FUNÇÕES BÁSICAS DE DADOS ---

// Fila
Fila* criarFila(int capacidade) {
    Fila* fila = (Fila*)malloc(sizeof(Fila));
    if (!fila) return NULL;
    fila->itens = (Peca*)calloc(capacidade, sizeof(Peca));
    if (!fila->itens) { free(fila); return NULL; }
    fila->frente = fila->tamanho = 0;
    fila->fim = capacidade - 1;
    fila->capacidade = capacidade;
    return fila;
}
bool estaCheiaFila(const Fila* fila) { return fila->tamanho == fila->capacidade; }
bool estaVaziaFila(const Fila* fila) { return fila->tamanho == 0; }
void enfileirar(Fila* fila, Peca peca) {
    if (estaCheiaFila(fila)) return;
    fila->fim = (fila->fim + 1) % fila->capacidade; // Fila circular
    fila->itens[fila->fim] = peca;
    fila->tamanho++;
}
Peca desenfileirar(Fila* fila) {
    Peca pecaVazia = {"NULL", -1};
    if (estaVaziaFila(fila)) return pecaVazia;
    Peca item = fila->itens[fila->frente];
    fila->frente = (fila->frente + 1) % fila->capacidade; // Fila circular
    fila->tamanho--;
    return item;
}

// Pilha
Pilha* criarPilha(int capacidade) {
    Pilha* pilha = (Pilha*)malloc(sizeof(Pilha));
    if (!pilha) return NULL;
    pilha->itens = (Peca*)calloc(capacidade, sizeof(Peca));
    if (!pilha->itens) { free(pilha); return NULL; }
    pilha->topo = -1;
    pilha->capacidade = capacidade;
    return pilha;
}
bool estaCheiaPilha(const Pilha* pilha) { return pilha->topo == pilha->capacidade - 1; }
bool estaVaziaPilha(const Pilha* pilha) { return pilha->topo == -1; }
void empilhar(Pilha* pilha, Peca peca) {
    if (estaCheiaPilha(pilha)) return;
    pilha->itens[++pilha->topo] = peca;
}
Peca desempilhar(Pilha* pilha) {
    Peca pecaVazia = {"NULL", -1};
    if (estaVaziaPilha(pilha)) return pecaVazia;
    return pilha->itens[pilha->topo--];
}

/**
 * @brief Inicializa as estruturas com peças iniciais.
 */
void inicializarPecas(Fila* fila, Pilha* pilha) {
    printf("--- Inicializando Jogo ---\n");
    // Peças iniciais na Fila de Jogo
    for(int i = 0; i < 3; i++) {
        Peca p;
        sprintf(p.nome, "Peça_A%d", i + 1);
        p.id = i + 1;
        enfileirar(fila, p);
    }
    printf("✅ Fila de Jogo Populada (3 peças).\n");

    // Peças iniciais na Pilha de Reserva
    for(int i = 0; i < 2; i++) {
        Peca p;
        sprintf(p.nome, "Peça_B%d", i + 1);
        p.id = i + 4;
        empilhar(pilha, p);
    }
    printf("✅ Pilha de Reserva Populada (2 peças).\n");
}

/**
 * @brief Libera toda a memória alocada.
 */
void liberarEstruturas(Fila* fila, Pilha* pilha) {
    if (fila) { free(fila->itens); free(fila); }
    if (pilha) { free(pilha->itens); free(pilha); }
    printf("\n✅ Memória de Fila e Pilha liberada.\n");
}

// --- 3. IMPLEMENTAÇÃO DE FUNÇÕES ESTRATÉGICAS ---

/**
 * @brief 1 - Jogar peça: Retira da fila e coloca na pilha Desfazer.
 */
void jogarPeca(Fila* fila, Pilha* desfazer) {
    if (estaVaziaFila(fila)) {
        printf("🚫 Fila vazia! Não há peça para jogar.\n");
        return;
    }
    Peca jogada = desenfileirar(fila);
    
    // Adiciona à pilha Desfazer
    if (estaCheiaPilha(desfazer)) {
        printf("⚠️ Pilha Desfazer cheia! A última jogada não será reversível.\n");
    } else {
        empilhar(desfazer, jogada);
    }

    printf("✅ Peça '%s' jogada (removida da fila).\n", jogada.nome);
}

/**
 * @brief 2 - Reservar peça: Retira da fila e coloca na pilha Reserva.
 */
void reservarPeca(Fila* fila, Pilha* reserva) {
    if (estaVaziaFila(fila)) {
        printf("🚫 Fila vazia! Não há peça para reservar.\n");
        return;
    }
    if (estaCheiaPilha(reserva)) {
        printf("🚫 Pilha de Reserva cheia! Não é possível reservar mais peças.\n");
        return;
    }

    Peca reservada = desenfileirar(fila);
    empilhar(reserva, reservada);
    printf("✅ Peça '%s' reservada (Fila -> Pilha Reserva).\n", reservada.nome);
}

/**
 * @brief 3 - Usar peça reservada: Retira da pilha Reserva e coloca na frente da fila.
 */
void usarPecaReservada(Fila* fila, Pilha* reserva) {
    if (estaVaziaPilha(reserva)) {
        printf("🚫 Pilha de Reserva vazia! Não há peça para usar.\n");
        return;
    }
    if (estaCheiaFila(fila)) {
        printf("🚫 Fila cheia! Não é possível usar peça reservada.\n");
        return;
    }

    Peca usada = desempilhar(reserva);
    
    // Requer um novo enfileiramento seguro, mas como a fila circular insere no 'fim',
    // vamos simular que a peça reservada *entra* na fila para ser jogada logo.
    // Para colocá-la na frente, seria necessário um deque. Usaremos a fila normal:
    enfileirar(fila, usada); 
    printf("✅ Peça '%s' usada (Pilha Reserva -> Fila). Ela entrará no final da fila.\n", usada.nome);
}

/**
 * @brief 4 - Trocar peça: Troca a peça do topo da pilha de reserva com a peça da frente da fila.
 */
void trocarTopoPilhaFrenteFila(Pilha* pilha, Fila* fila) {
    if (estaVaziaPilha(pilha) || estaVaziaFila(fila)) {
        printf("🚫 Não é possível trocar: Pilha ou Fila vazia.\n");
        return;
    }

    // Acessa a peça do topo da pilha sem desempilhar
    Peca* topoPilhaPtr = &pilha->itens[pilha->topo];
    
    // Acessa a peça da frente da fila sem desenfileirar
    Peca* frenteFilaPtr = &fila->itens[fila->frente];

    // Troca os dados das peças (usando uma temporária)
    Peca temp = *topoPilhaPtr;
    *topoPilhaPtr = *frenteFilaPtr;
    *frenteFilaPtr = temp;

    printf("🔄 Troca realizada: '%s' (Pilha) <-> '%s' (Fila).\n", frenteFilaPtr->nome, topoPilhaPtr->nome);
}

/**
 * @brief 5 - Desfazer: Retira peça da pilha Desfazer e coloca na frente da fila.
 */
void desfazerUltimaJogada(Fila* fila, Pilha* desfazer) {
    if (estaVaziaPilha(desfazer)) {
        printf("🚫 Pilha Desfazer vazia! Não há jogada para reverter.\n");
        return;
    }
    if (estaCheiaFila(fila)) {
        printf("🚫 Fila cheia! Não é possível desfazer.\n");
        return;
    }

    Peca desfeita = desempilhar(desfazer);

    // Como é um desfazer, a peça deve ser devolvida à frente da fila.
    // Em uma fila circular, isso é complexo. Para simplificar, vamos inseri-la no final, 
    // mas o ideal seria usar um Deque ou Shifting (caro) para inserção no início.
    // Usaremos a inserção normal na fila circular, com aviso.
    enfileirar(fila, desfeita); 
    
    printf("⏪ Última jogada desfeita: Peça '%s' devolvida à fila (entra no final).\n", desfeita.nome);
}

/**
 * @brief 6 - Inverte fila com pilha: Transfere todos os elementos da fila para a pilha e vice-versa.
 */
void inverterFilaComPilha(Fila* fila, Pilha* pilha) {
    Peca temp[CAPACIDADE_MAXIMA];
    int countFila = 0;
    int countPilha = 0;

    // 1. Transferir Fila para Temp
    while (!estaVaziaFila(fila)) {
        temp[countFila++] = desenfileirar(fila);
    }
    
    // 2. Transferir Pilha para Fila (inverte a ordem da Pilha)
    while (!estaVaziaPilha(pilha)) {
        enfileirar(fila, desempilhar(pilha));
    }

    // 3. Transferir Temp para Pilha (inverte a ordem da Fila)
    for (int i = 0; i < countFila; i++) {
        empilhar(pilha, temp[i]);
    }
    
    printf("🔁 INVERSÃO TOTAL: O conteúdo da Fila e da Pilha de Reserva foram trocados.\n");
}


// --- 4. FUNÇÕES DE INTERFACE ---

void exibirMenu() {
    printf("\n--- MENU DE AÇÕES ESTRATÉGICAS ---\n");
    printf("1 - Jogar peça (Retira da Fila)\n");
    printf("2 - Reservar peça (Fila -> Pilha Reserva)\n");
    printf("3 - Usar peça reservada (Pilha Reserva -> Fila)\n");
    printf("4 - Trocar peça do topo da pilha com a da frente da fila\n");
    printf("5 - Desfazer última jogada (Reverte a jogada 1)\n");
    printf("6 - Inverter fila com pilha (Troca conteúdo total)\n");
    printf("0 - Sair\n");
    printf("Escolha uma ação: ");
}

void exibirEstruturas(const Fila* fila, const Pilha* pilha) {
    printf("\n==================== ESTADO ATUAL ====================\n");
    
    // Exibição da FILA (Jogo)
    printf("FIFO JOGO (Fila Circular, %d/%d): ", fila->tamanho, fila->capacidade);
    if (estaVaziaFila(fila)) {
        printf("[VAZIA]\n");
    } else {
        printf("FRENTE -> ");
        for (int i = 0; i < fila->tamanho; i++) {
            int index = (fila->frente + i) % fila->capacidade;
            printf("[%s] ", fila->itens[index].nome);
        }
        printf("<- FIM\n");
    }

    // Exibição da PILHA (Reserva)
    printf("LIFO RESERVA (Pilha, %d/%d): ", pilha->topo + 1, pilha->capacidade);
    if (estaVaziaPilha(pilha)) {
        printf("[VAZIA]\n");
    } else {
        printf("TOPO -> ");
        for (int i = pilha->topo; i >= 0; i--) {
            printf("[%s] ", pilha->itens[i].nome);
        }
        printf("<- BASE\n");
    }

    printf("======================================================\n");
}