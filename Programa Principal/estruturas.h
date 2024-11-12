#ifndef ESTRUTURAS_H
#define ESTRUTURAS_H

typedef struct Pedagio
{
    char nomeCidade[50];
    float valor;
    struct Pedagio *proximo;
} Pedagio;

typedef struct Cidade
{
    char nomeCidade[50];
    float distancia;
    Pedagio *pedagios;
    struct Cidade *proxima;
    struct Cidade *anterior;
} Cidade;

typedef struct RodoviaAdjacente
{
    char nomeRodovia[50];
    char cidadeConexao[50];
    struct RodoviaAdjacente *proxima;
} RodoviaAdjacente;

typedef struct Rodovia
{
    char nome[50];
    Cidade *cidades;
    float pedagio;
    struct Rodovia *proxima;
    RodoviaAdjacente *rodovias_adjacentes;
} Rodovia;

void carregarRodoviasDeArquivo(Rodovia **lista, char *nomeArquivo);
Rodovia *buscarRodovia(Rodovia *lista, char nome[]);
Rodovia *inserirRodovia(Rodovia *lista, char nome[]);
Rodovia *removerRodovia(Rodovia *lista, char nome[]);
Cidade *inserirCidade(Rodovia *rodovia, char nomeCidade[], float distancia);
void adicionarPedagio(Rodovia *rodovia, char nomeCidade1[], char nomeCidade2[], float valorPedagio);
float calcularPedagioTotal(Cidade *cidades);
void removerCidade(Rodovia *rodovia, char nomeCidade[]);
void percursoCidades(Rodovia *rodovia, char cidadeInicio[], char cidadeFim[]);
void listarCruzamentos(Rodovia *listaRodovias);
void imprimirRodovias(Rodovia *lista);
void salvarRodoviasArquivo(Rodovia *lista, char *nomeArquivo);
void listarCruzamentosEspecificos(Rodovia *rodovia1, Rodovia *rodovia2);
void converterMinusculo(char *str);
void apresentarInformacoes();
void menu();
void conectarRodovias(Rodovia *listaRodovias);
void percursoEntreRodovias(Rodovia *listaRodovias, char cidadeInicio[], char cidadeFim[]);
void calcularPercurso(Rodovia *rodovia, char cidadeInicio[], char cidadeFim[], float *totalPedagio, float *totalDistancia);
void removerEspacos(char *texto);

#endif