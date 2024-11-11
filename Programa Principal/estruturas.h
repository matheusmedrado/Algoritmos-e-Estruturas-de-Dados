#ifndef ESTRUTURAS_H
#define ESTRUTURAS_H

typedef struct Cidade
{
    char nomeCidade[50];
    float distancia;
    float pedagio;
    struct Cidade *proxima;
    struct Cidade *anterior;
} Cidade;

typedef struct Rodovia
{
    char nome[50];
    Cidade *cidades;
    float pedagio;
    struct Rodovia *proxima;
} Rodovia;

void carregarRodoviasDeArquivo(Rodovia **lista, char *nomeArquivo);
Rodovia *buscarRodovia(Rodovia *lista, char nome[]);
Rodovia *inserirRodovia(Rodovia *lista, char nome[]);
Rodovia *removerRodovia(Rodovia *lista, char nome[]);
Cidade *inserirCidade(Rodovia *rodovia, char nomeCidade[], float distancia);
void adicionarPedagio(Rodovia *rodovia, char nomeCidade1[], char nomeCidade2[], float valorPedagio);
float calcularPedagioTotal(Cidade *cidades);
void removerCidade(Rodovia *rodovia, char nomeCidade[]);
void percursoEntreCidades(Rodovia *rodovia, char cidadeInicio[], char cidadeFim[]);
void listarTodosCruzamentos(Rodovia *listaRodovias);
void imprimirRodovias(Rodovia *lista);
void imprimirRodoviasEmArquivo(Rodovia *lista, char *nomeArquivo);
void listarCruzamentos(Rodovia *rodovia1, Rodovia *rodovia2);
void toLowerStr(char *str);
void apresentar_informacoes_e_obter_consentimento();
void menu();

#endif