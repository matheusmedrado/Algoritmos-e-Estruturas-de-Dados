#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "estruturas.h"

void carregarRodoviasDeArquivo(Rodovia **lista, char *nomeArquivo)
{
    FILE *arquivo = fopen(nomeArquivo, "r");
    if (arquivo == NULL)
    {
        printf("Erro ao abrir o arquivo %s para leitura!\n", nomeArquivo);
        return;
    }

    char linha[256];
    char nomeRodovia[50];
    float pedagioRodovia;

    while (fgets(linha, sizeof(linha), arquivo))
    {
        if (sscanf(linha, "Rodovia: %49s", nomeRodovia) == 1)
        {
            fgets(linha, sizeof(linha), arquivo);
            sscanf(linha, "Pedágio: R$ %f", &pedagioRodovia);

            Rodovia *novaRodovia = inserirRodovia(*lista, nomeRodovia);
            if (novaRodovia != NULL)
            {
                novaRodovia->pedagio = pedagioRodovia;
                *lista = novaRodovia;

                char nomeCidade[50];
                float distancia, pedagio;

                while (fgets(linha, sizeof(linha), arquivo) && linha[0] == '\t')
                {
                    if (sscanf(linha, "\tCidade: %49[^,], Distância: %f, Pedágio: R$ %f",
                               nomeCidade, &distancia, &pedagio) == 3)
                    {
                        Cidade *novaCidade = inserirCidade(novaRodovia, nomeCidade, distancia);
                        if (novaCidade != NULL && pedagio > 0)
                        {

                            if (novaCidade->anterior != NULL)
                            {
                                Pedagio *novoPedagio = (Pedagio *)malloc(sizeof(Pedagio));
                                if (novoPedagio != NULL)
                                {
                                    strcpy(novoPedagio->nomeCidade, novaCidade->nomeCidade);
                                    novoPedagio->valor = pedagio;
                                    novoPedagio->proximo = novaCidade->anterior->pedagios;
                                    novaCidade->anterior->pedagios = novoPedagio;
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    fclose(arquivo);
    printf("Dados carregados de '%s' com sucesso!\n", nomeArquivo);
}

Rodovia *buscarRodovia(Rodovia *lista, char nome[])
{
    Rodovia *rodoviaAtual = lista;
    char nomeBusca[50], nomeAtual[50];

    strcpy(nomeBusca, nome);
    converterMinusculo(nomeBusca);

    while (rodoviaAtual != NULL)
    {
        strcpy(nomeAtual, rodoviaAtual->nome);
        converterMinusculo(nomeAtual);

        if (strcmp(nomeAtual, nomeBusca) == 0)
        {
            return rodoviaAtual;
        }
        rodoviaAtual = rodoviaAtual->proxima;
    }
    return NULL;
}

Rodovia *inserirRodovia(Rodovia *lista, char nome[])
{
    Rodovia *atual = lista;
    char nomeAtual[50], nomeBusca[50];
    strcpy(nomeBusca, nome);
    converterMinusculo(nomeBusca);

    while (atual != NULL)
    {
        strcpy(nomeAtual, atual->nome);
        converterMinusculo(nomeAtual);
        if (strcmp(nomeAtual, nomeBusca) == 0)
        {
            printf("Erro: A rodovia '%s' já existe!\n", nome);
            return lista;
        }
        atual = atual->proxima;
    }

    Rodovia *novaRodovia = (Rodovia *)malloc(sizeof(Rodovia));
    if (novaRodovia == NULL)
    {
        printf("Erro de alocação de memória!\n");
        return lista;
    }

    strcpy(novaRodovia->nome, nome);
    novaRodovia->cidades = NULL;
    novaRodovia->proxima = lista;
    novaRodovia->pedagio = 0.0;

    return novaRodovia;
}

Rodovia *removerRodovia(Rodovia *lista, char nome[])
{
    Rodovia *rodoviaAtual = lista;
    Rodovia *anterior = NULL;

    while (rodoviaAtual != NULL && strcmp(rodoviaAtual->nome, nome))
    {
        anterior = rodoviaAtual;
        rodoviaAtual = rodoviaAtual->proxima;
    }

    if (rodoviaAtual == NULL)
    {
        return lista;
    }

    if (anterior == NULL)
    {
        lista = rodoviaAtual->proxima;
    }
    else
    {
        anterior->proxima = rodoviaAtual->proxima;
    }

    Cidade *cidadeAtual = rodoviaAtual->cidades;
    while (cidadeAtual != NULL)
    {
        Cidade *proximaCidade = cidadeAtual->proxima;

        // Free all pedagios for this city
        Pedagio *pedagioAtual = cidadeAtual->pedagios;
        while (pedagioAtual != NULL)
        {
            Pedagio *proximoPedagio = pedagioAtual->proximo;
            free(pedagioAtual);
            pedagioAtual = proximoPedagio;
        }

        free(cidadeAtual);
        cidadeAtual = proximaCidade;
    }

    free(rodoviaAtual);
    return lista;
}

Cidade *inserirCidade(Rodovia *rodovia, char nomeCidade[], float distancia)
{
    Cidade *novaCidade = (Cidade *)malloc(sizeof(Cidade));
    if (novaCidade == NULL)
    {
        printf("Erro de alocação de memória!\n");
        return NULL;
    }

    strcpy(novaCidade->nomeCidade, nomeCidade);
    novaCidade->distancia = distancia;
    novaCidade->pedagios = NULL;
    novaCidade->proxima = NULL;
    novaCidade->anterior = NULL;

    if (rodovia->cidades == NULL || rodovia->cidades->distancia > distancia)
    {
        novaCidade->proxima = rodovia->cidades;
        if (rodovia->cidades != NULL)
        {
            rodovia->cidades->anterior = novaCidade;
        }
        rodovia->cidades = novaCidade;
    }
    else
    {
        Cidade *atual = rodovia->cidades;
        while (atual->proxima != NULL && atual->proxima->distancia < distancia)
        {
            atual = atual->proxima;
        }

        novaCidade->proxima = atual->proxima;
        novaCidade->anterior = atual;
        if (atual->proxima != NULL)
        {
            atual->proxima->anterior = novaCidade;
        }
        atual->proxima = novaCidade;
    }

    return novaCidade;
}

void adicionarPedagio(Rodovia *rodovia, char nomeCidade1[], char nomeCidade2[], float valorPedagio)
{
    Cidade *cidade1 = NULL;
    Cidade *cidade2 = NULL;
    Cidade *cidadeAtual = rodovia->cidades;

    while (cidadeAtual != NULL)
    {
        if (strcmp(cidadeAtual->nomeCidade, nomeCidade1) == 0)
        {
            cidade1 = cidadeAtual;
        }
        if (strcmp(cidadeAtual->nomeCidade, nomeCidade2) == 0)
        {
            cidade2 = cidadeAtual;
        }
        cidadeAtual = cidadeAtual->proxima;
    }

    if (cidade1 == NULL || cidade2 == NULL)
    {
        printf("Erro: Uma ou ambas as cidades não foram encontradas na rodovia.\n");
        return;
    }

    Pedagio *novoPedagio = (Pedagio *)malloc(sizeof(Pedagio));
    if (novoPedagio == NULL)
    {
        printf("Erro de alocação de memória!\n");
        return;
    }

    strcpy(novoPedagio->nomeCidade, nomeCidade2);
    novoPedagio->valor = valorPedagio;
    novoPedagio->proximo = cidade1->pedagios;
    cidade1->pedagios = novoPedagio;

    printf("Pedágio de R$ %.2f adicionado entre %s e %s.\n", valorPedagio, nomeCidade1, nomeCidade2);
}

float calcularPedagioTotal(Cidade *cidades)
{
    float total = 0.0;
    Cidade *atual = cidades;
    while (atual != NULL)
    {
        Pedagio *pedagioAtual = atual->pedagios;
        while (pedagioAtual != NULL)
        {
            total += pedagioAtual->valor;
            pedagioAtual = pedagioAtual->proximo;
        }
        atual = atual->proxima;
    }
    return total;
}

void removerCidade(Rodovia *rodovia, char nomeCidade[])
{
    if (rodovia->cidades == NULL)
    {
        printf("Nenhuma cidade para remover.\n");
        return;
    }

    Cidade *atual = rodovia->cidades;

    while (atual != NULL && strcmp(atual->nomeCidade, nomeCidade) != 0)
    {
        atual = atual->proxima;
    }

    if (atual == NULL)
    {
        printf("Cidade '%s' não encontrada na rodovia.\n", nomeCidade);
        return;
    }

    if (atual->anterior == NULL)
    {
        rodovia->cidades = atual->proxima;
        if (atual->proxima != NULL)
        {
            atual->proxima->anterior = NULL;
        }
    }
    else
    {
        atual->anterior->proxima = atual->proxima;
        if (atual->proxima != NULL)
        {
            atual->proxima->anterior = atual->anterior;
        }
    }

    // Free all pedagios before freeing the city
    Pedagio *pedagioAtual = atual->pedagios;
    while (pedagioAtual != NULL)
    {
        Pedagio *proximoPedagio = pedagioAtual->proximo;
        free(pedagioAtual);
        pedagioAtual = proximoPedagio;
    }

    free(atual);
    printf("Cidade '%s' removida com sucesso!\n", nomeCidade);
}

void percursoEntreCidades(Rodovia *rodovia, char cidadeInicio[], char cidadeFim[])
{
    Cidade *atual = rodovia->cidades;
    Cidade *cidadeInicial = NULL;
    Cidade *cidadeFinal = NULL;
    float distanciaTotal = 0;
    float pedagioTotal = 0;

    while (atual != NULL)
    {
        if (strcmp(atual->nomeCidade, cidadeInicio) == 0)
        {
            cidadeInicial = atual;
        }
        if (strcmp(atual->nomeCidade, cidadeFim) == 0)
        {
            cidadeFinal = atual;
        }
        atual = atual->proxima;
    }

    if (cidadeInicial == NULL)
    {
        printf("Cidade de início '%s' não encontrada na rodovia.\n", cidadeInicio);
        return;
    }
    if (cidadeFinal == NULL)
    {
        printf("Cidade de destino '%s' não encontrada na rodovia.\n", cidadeFim);
        return;
    }

    printf("\n=== Percurso na rodovia %s ===\n", rodovia->nome);
    printf("Início do percurso em: %s (km %.2f)\n", cidadeInicio, cidadeInicial->distancia);

    atual = cidadeInicial;
    while (atual != cidadeFinal && atual != NULL)
    {
        Cidade *proxima = atual->proxima;
        if (proxima != NULL)
        {
            float trechoDistancia = proxima->distancia - atual->distancia;
            distanciaTotal += trechoDistancia;

            printf("\n-> Próxima cidade: %s (km %.2f)\n", proxima->nomeCidade, proxima->distancia);
            printf("   Distância do trecho: %.2f km\n", trechoDistancia);

            Pedagio *pedagioAtual = atual->pedagios;
            while (pedagioAtual != NULL)
            {
                if (strcmp(pedagioAtual->nomeCidade, proxima->nomeCidade) == 0)
                {
                    pedagioTotal += pedagioAtual->valor;
                    break;
                }
                pedagioAtual = pedagioAtual->proximo;
            }

            if (pedagioAtual == NULL)
            {
                pedagioAtual = proxima->pedagios;
                while (pedagioAtual != NULL)
                {
                    if (strcmp(pedagioAtual->nomeCidade, atual->nomeCidade) == 0)
                    {
                        pedagioTotal += pedagioAtual->valor;
                        break;
                    }
                    pedagioAtual = pedagioAtual->proximo;
                }
            }
        }
        atual = atual->proxima;
    }

    printf("\n=== Fim do percurso ===\n");
    printf("Distância total percorrida: %.2f km\n", distanciaTotal);
    printf("Custo total em pedágios: R$ %.2f\n", pedagioTotal);
}

void listarTodosCruzamentos(Rodovia *listaRodovias)
{
    if (listaRodovias == NULL || listaRodovias->proxima == NULL)
    {
        printf("São necessárias pelo menos duas rodovias para verificar cruzamentos!\n");
        return;
    }

    int encontrouAlgumCruzamento = 0;
    printf("\n=== Listagem de Todos os Cruzamentos ===\n");

    for (Rodovia *r1 = listaRodovias; r1 != NULL; r1 = r1->proxima)
    {
        for (Rodovia *r2 = r1->proxima; r2 != NULL; r2 = r2->proxima)

        {
            Cidade *cidade1 = r1->cidades;
            int encontrouCruzamento = 0;
            char cidade1Lower[50], cidade2Lower[50];

            while (cidade1 != NULL)
            {
                Cidade *cidade2 = r2->cidades;
                strcpy(cidade1Lower, cidade1->nomeCidade);
                converterMinusculo(cidade1Lower);

                while (cidade2 != NULL)
                {
                    strcpy(cidade2Lower, cidade2->nomeCidade);
                    converterMinusculo(cidade2Lower);

                    if (strcmp(cidade1Lower, cidade2Lower) == 0)
                    {
                        if (!encontrouCruzamento)
                        {
                            printf("\nCruzamento entre %s e %s:\n", r1->nome, r2->nome);
                            encontrouCruzamento = 1;
                            encontrouAlgumCruzamento = 1;
                        }
                        printf("- %s (km %.2f na %s, km %.2f na %s)\n",
                               cidade1->nomeCidade,
                               cidade1->distancia,
                               r1->nome,
                               cidade2->distancia,
                               r2->nome);
                    }
                    cidade2 = cidade2->proxima;
                }
                cidade1 = cidade1->proxima;
            }
        }
    }

    if (!encontrouAlgumCruzamento)
    {
        printf("\nNenhum cruzamento encontrado entre as rodovias cadastradas.\n");
    }
}

void imprimirRodovias(Rodovia *lista)
{
    Rodovia *rodoviaAtual = lista;

    if (rodoviaAtual == NULL)
    {
        printf("Nenhuma rodovia registrada!\n");
        return;
    }

    while (rodoviaAtual != NULL)
    {
        float pedagioTotal = calcularPedagioTotal(rodoviaAtual->cidades);
        rodoviaAtual->pedagio = pedagioTotal;

        printf("Rodovia: %s\n", rodoviaAtual->nome);
        printf("Pedágio Total: R$ %.2f\n", pedagioTotal);

        Cidade *cidadeAtual = rodoviaAtual->cidades;
        while (cidadeAtual != NULL)
        {
            printf("Cidade: %s, Distância: %.2f",
                   cidadeAtual->nomeCidade, cidadeAtual->distancia);
            if (cidadeAtual->pedagios != NULL)
            {
                printf(", Pedágio: R$ %.2f", cidadeAtual->pedagios->valor);
            }
            printf("\n");
            cidadeAtual = cidadeAtual->proxima;
        }
        rodoviaAtual = rodoviaAtual->proxima;
        printf("\n");
    }
}

void imprimirRodoviasEmArquivo(Rodovia *lista, char *nomeArquivo)
{
    FILE *arquivo = fopen(nomeArquivo, "w");
    if (arquivo == NULL)
    {
        printf("Erro ao abrir o arquivo para escrita!\n");
        return;
    }

    Rodovia *rodoviaAtual = lista;

    if (rodoviaAtual == NULL)
    {
        fprintf(arquivo, "Nenhuma rodovia registrada!\n");
    }
    else
    {
        while (rodoviaAtual != NULL)
        {
            float pedagioTotal = calcularPedagioTotal(rodoviaAtual->cidades);
            rodoviaAtual->pedagio = pedagioTotal;

            fprintf(arquivo, "Rodovia: %s\n", rodoviaAtual->nome);
            fprintf(arquivo, "Pedágio: R$ %.2f\n", pedagioTotal);

            Cidade *cidadeAtual = rodoviaAtual->cidades;
            while (cidadeAtual != NULL)
            {
                fprintf(arquivo, "\tCidade: %s, Distância: %.2f",
                        cidadeAtual->nomeCidade, cidadeAtual->distancia);
                if (cidadeAtual->pedagios != NULL)
                {
                    fprintf(arquivo, ", Pedágio: R$ %.2f", cidadeAtual->pedagios->valor);
                }
                fprintf(arquivo, "\n");
                cidadeAtual = cidadeAtual->proxima;
            }

            rodoviaAtual = rodoviaAtual->proxima;
            fprintf(arquivo, "\n");
        }
    }

    fclose(arquivo);
}

void listarCruzamentos(Rodovia *rodovia1, Rodovia *rodovia2)
{
    if (rodovia1 == NULL || rodovia2 == NULL)
    {
        printf("Uma ou ambas as rodovias não foram encontradas!\n");
        return;
    }

    printf("\nPontos de cruzamento entre %s e %s:\n", rodovia1->nome, rodovia2->nome);

    Cidade *cidadeRodovia1 = rodovia1->cidades;
    int encontrouCruzamento = 0;
    char cidade1Lower[50], cidade2Lower[50];

    while (cidadeRodovia1 != NULL)
    {
        Cidade *cidadeRodovia2 = rodovia2->cidades;
        strcpy(cidade1Lower, cidadeRodovia1->nomeCidade);
        converterMinusculo(cidade1Lower);

        while (cidadeRodovia2 != NULL)
        {
            strcpy(cidade2Lower, cidadeRodovia2->nomeCidade);
            converterMinusculo(cidade2Lower);

            if (strcmp(cidade1Lower, cidade2Lower) == 0)
            {
                printf("- %s (km %.2f na %s, km %.2f na %s)\n",
                       cidadeRodovia1->nomeCidade,
                       cidadeRodovia1->distancia,
                       rodovia1->nome,
                       cidadeRodovia2->distancia,
                       rodovia2->nome);
                encontrouCruzamento = 1;
            }
            cidadeRodovia2 = cidadeRodovia2->proxima;
        }
        cidadeRodovia1 = cidadeRodovia1->proxima;
    }

    if (!encontrouCruzamento)
    {
        printf("Nenhum ponto de cruzamento encontrado entre estas rodovias.\n");
    }
}

void converterMinusculo(char *str)
{
    for (int i = 0; str[i]; i++)
    {
        str[i] = tolower(str[i]);
    }
}

void apresentarInformacoes()
{
    printf("Bem-vindo ao sistema de gerenciamento de rodovias!\n");
    printf("\nPara garantir a melhor experiência, por favor, leia as seguintes informações:\n");
    printf("\n- Utilize nomes claros e concisos para rodovias e cidades.\n");
    printf("- Priorize o uso do hífen (-) em nomes de rodovias (ex: BR-101).\n");
    printf("- Verifique a ortografia para evitar erros.\n");
    printf("- Faça backups regulares dos seus dados.\n");

    printf("\nAo utilizar este sistema, você concorda com as práticas acima?\n");
    printf("1 - Sim\n");
    printf("2 - Não\n");

    int opcao;
    scanf("%d", &opcao);

    if (opcao == 1)
    {
        menu();
    }
    else
    {
        printf("\n[Você não concordou com os termos de uso. O programa será encerrado.]\n");
    }
}

void menu()
{
    int opcao = -1;
    Rodovia *listaRodovias = NULL;
    char nomeRodovia[50], nomeCidade[50];
    float distancia;
    char rodovia1[50], rodovia2[50];
    Rodovia *r1, *r2;
    Rodovia *rodovia;
    char nomeArquivo[50] = "";

    printf("\nDeseja iniciar o programa com um arquivo em branco ou carregar um arquivo com os dados já existente?\n");
    printf("0 para começar em branco, 1 para carregar um arquivo com os dados:\n");
    int escolha;
    scanf("%d", &escolha);
    getchar();

    if (escolha == 1)
    {
        printf("Insira o nome do arquivo para carregar os dados (ex: rodovias.txt): ");
        scanf("%49s", nomeArquivo);
        getchar();
        carregarRodoviasDeArquivo(&listaRodovias, nomeArquivo);
    }

    while (opcao != 0)
    {
        printf("\n-> Escolha uma opcao:\n");
        printf("1. Inserir rodovia\n");
        printf("2. Remover rodovia\n");
        printf("3. Inserir cidade em rodovia \n");
        printf("4. Remover cidade de rodovia\n");
        printf("5. Ver percurso entre duas cidades\n");
        printf("6. Verificar cruzamento entre todas as rodovias\n");
        printf("7. Imprimir lista de rodovias\n");
        printf("8. Salvar lista em arquivo\n");
        printf("9. Listar todos os cruzamentos entre duas rodovias específicas\n");
        printf("10. Salvar alterações no arquivo atual\n");
        printf("0. Sair\n");
        scanf("%d", &opcao);
        getchar();

        switch (opcao)
        {
        case 1:
            printf("Insira o nome da rodovia: ");
            fgets(nomeRodovia, sizeof(nomeRodovia), stdin);
            nomeRodovia[strcspn(nomeRodovia, "\n")] = 0;
            listaRodovias = inserirRodovia(listaRodovias, nomeRodovia);
            break;

        case 2:
            printf("Insira o nome da rodovia a ser removida: ");
            fgets(nomeRodovia, sizeof(nomeRodovia), stdin);
            nomeRodovia[strcspn(nomeRodovia, "\n")] = 0;
            listaRodovias = removerRodovia(listaRodovias, nomeRodovia);
            break;

        case 3:
            printf("Insira o nome da rodovia para adicionar a cidade: ");
            fgets(nomeRodovia, sizeof(nomeRodovia), stdin);
            nomeRodovia[strcspn(nomeRodovia, "\n")] = 0;

            char nomeRodoviaLower[50];
            strcpy(nomeRodoviaLower, nomeRodovia);
            converterMinusculo(nomeRodoviaLower);

            Rodovia *rodovia = buscarRodovia(listaRodovias, nomeRodovia);
            if (rodovia != NULL)
            {
                printf("Insira o nome da cidade: ");
                fgets(nomeCidade, sizeof(nomeCidade), stdin);
                nomeCidade[strcspn(nomeCidade, "\n")] = 0;

                char nomeCidadeLower[50];
                strcpy(nomeCidadeLower, nomeCidade);
                converterMinusculo(nomeCidadeLower);

                Cidade *atual = rodovia->cidades;
                while (atual != NULL)
                {
                    char cidadeAtualLower[50];
                    strcpy(cidadeAtualLower, atual->nomeCidade);
                    converterMinusculo(cidadeAtualLower);

                    if (strcmp(cidadeAtualLower, nomeCidadeLower) == 0)
                    {
                        printf("Erro: A cidade '%s' já existe na rodovia '%s'!\n",
                               nomeCidade, nomeRodovia);
                        break;
                    }
                    atual = atual->proxima;
                }

                if (atual != NULL)
                {
                    break;
                }

                printf("Insira a distância da cidade em relação ao início da rodovia: ");
                if (scanf("%f", &distancia) != 1)
                {
                    printf("Erro: entrada inválida para distância\n");
                    while (getchar() != '\n')
                        ;
                    break;
                }
                getchar();

                Cidade *novaCidade = inserirCidade(rodovia, nomeCidade, distancia);
                if (novaCidade == NULL)
                {
                    printf("Erro ao inserir cidade\n");
                    break;
                }

                Cidade *tempCidade = rodovia->cidades;
                int cityCount = 0;
                while (tempCidade != NULL)
                {
                    cityCount++;
                    tempCidade = tempCidade->proxima;
                }

                if (cityCount > 1)
                {
                    printf("\nCidades presentes na rodovia %s:\n", nomeRodovia);
                    Cidade *cidadeAtual = rodovia->cidades;
                    int i = 1;
                    while (cidadeAtual != NULL)
                    {
                        if (strcmp(cidadeAtual->nomeCidade, nomeCidade) != 0)
                        {
                            printf("%d. %s (Distância: %.2f km)\n", i, cidadeAtual->nomeCidade, cidadeAtual->distancia);
                        }
                        cidadeAtual = cidadeAtual->proxima;
                        i++;
                    }

                    printf("\nDeseja adicionar pedágio para alguma cidade? (1-Sim, 0-Não): ");
                    int addPedagio;
                    scanf("%d", &addPedagio);
                    getchar();

                    if (addPedagio == 1)
                    {
                        printf("\nEscolha o número da cidade para adicionar pedágio: ");
                        int escolha;
                        scanf("%d", &escolha);
                        getchar();

                        if (escolha >= 1 && escolha < i)
                        {

                            cidadeAtual = rodovia->cidades;
                            int j = 1;
                            while (cidadeAtual != NULL && j < escolha)
                            {
                                cidadeAtual = cidadeAtual->proxima;
                                j++;
                            }

                            if (cidadeAtual != NULL)
                            {
                                float valorPedagio;
                                printf("Digite o valor do pedágio entre %s e %s: R$ ",
                                       nomeCidade, cidadeAtual->nomeCidade);
                                scanf("%f", &valorPedagio);
                                getchar();

                                adicionarPedagio(rodovia, nomeCidade,
                                                 cidadeAtual->nomeCidade, valorPedagio);
                            }
                        }
                        else
                        {
                            printf("Escolha inválida!\n");
                        }
                    }
                }

                printf("Cidade '%s' inserida com sucesso na rodovia '%s'!\n",
                       nomeCidade, nomeRodovia);
            }
            else
            {
                printf("Rodovia não encontrada!\n");
            }
            break;

        case 4:
            printf("Insira o nome da rodovia para remover a cidade: ");
            fgets(nomeRodovia, sizeof(nomeRodovia), stdin);
            nomeRodovia[strcspn(nomeRodovia, "\n")] = 0;
            rodovia = buscarRodovia(listaRodovias, nomeRodovia);
            if (rodovia != NULL)
            {
                printf("Insira o nome da cidade a ser removida: ");
                fgets(nomeCidade, sizeof(nomeCidade), stdin);
                nomeCidade[strcspn(nomeCidade, "\n")] = 0;
                removerCidade(rodovia, nomeCidade);
            }
            else
            {
                printf("Rodovia não encontrada!\n");
            }
            break;

        case 5:
            conectarRodovias(listaRodovias);

            char cidadeInicio[50], cidadeFim[50];
            printf("Insira a cidade de início: ");
            fgets(cidadeInicio, sizeof(cidadeInicio), stdin);
            cidadeInicio[strcspn(cidadeInicio, "\n")] = 0;

            printf("Insira a cidade de fim: ");
            fgets(cidadeFim, sizeof(cidadeFim), stdin);
            cidadeFim[strcspn(cidadeFim, "\n")] = 0;

            percursoEntreRodovias(listaRodovias, cidadeInicio, cidadeFim);
            break;

        case 6:
            printf("\nVerificando todos os cruzamentos entre rodovias...\n");
            listarTodosCruzamentos(listaRodovias);
            break;

        case 7:
            imprimirRodovias(listaRodovias);
            break;

        case 8:
            printf("Digite o nome do novo arquivo para salvar (ex: novo_rodovias.txt): ");
            char novoArquivo[50];
            scanf("%49s", novoArquivo);
            getchar();
            FILE *arquivo = fopen(novoArquivo, "w");
            if (arquivo == NULL)
            {
                printf("Erro ao criar novo arquivo!\n");
                break;
            }
            fclose(arquivo);
            imprimirRodoviasEmArquivo(listaRodovias, novoArquivo);
            printf("Dados salvos com sucesso em '%s'!\n", novoArquivo);
            break;

        case 9:
            printf("Insira o nome das duas rodovias para listar todos os cruzamentos:\n");
            fgets(rodovia1, sizeof(rodovia1), stdin);
            rodovia1[strcspn(rodovia1, "\n")] = 0;
            fgets(rodovia2, sizeof(rodovia2), stdin);
            rodovia2[strcspn(rodovia2, "\n")] = 0;
            r1 = buscarRodovia(listaRodovias, rodovia1);
            r2 = buscarRodovia(listaRodovias, rodovia2);
            if (r1 != NULL && r2 != NULL)
            {
                listarCruzamentos(r1, r2);
            }
            else
            {
                printf("Uma ou ambas as rodovias não foram encontradas!\n");
            }
            break;

        case 10:
            if (strlen(nomeArquivo) > 0)
            {
                imprimirRodoviasEmArquivo(listaRodovias, nomeArquivo);
                printf("Alterações salvas com sucesso em '%s'!\n", nomeArquivo);
            }
            else
            {
                printf("Nenhum arquivo foi carregado inicialmente. Use a opção 8 para salvar em um novo arquivo.\n");
            }
            break;

        default:
            if (opcao != 0)
            {
                printf("Opcao invalida!\n");
            }
            break;
        }
    }

    if (strlen(nomeArquivo) > 0)
    {
        printf("\nDeseja salvar as alterações antes de sair? (1-Sim, 0-Não): ");
        int salvar;
        scanf("%d", &salvar);
        if (salvar == 1)
        {
            imprimirRodoviasEmArquivo(listaRodovias, nomeArquivo);
            printf("Alterações salvas com sucesso em '%s'!\n", nomeArquivo);
        }
    }

    liberarMemoria(listaRodovias);
    printf("Saindo...\n");
}

void conectarRodovias(Rodovia *listaRodovias)
{
    for (Rodovia *r1 = listaRodovias; r1 != NULL; r1 = r1->proxima)
    {
        r1->rodovias_adjacentes = NULL;

        for (Cidade *c1 = r1->cidades; c1 != NULL; c1 = c1->proxima)
        {
            for (Rodovia *r2 = listaRodovias; r2 != NULL; r2 = r2->proxima)

            {
                if (r1 != r2)
                {
                    for (Cidade *c2 = r2->cidades; c2 != NULL; c2 = c2->proxima)
                    {
                        if (strcmp(c1->nomeCidade, c2->nomeCidade) == 0)
                        {
                            RodoviaAdjacente *nova = malloc(sizeof(RodoviaAdjacente));
                            strcpy(nova->nomeRodovia, r2->nome);
                            strcpy(nova->cidadeConexao, c1->nomeCidade);
                            nova->proxima = r1->rodovias_adjacentes;
                            r1->rodovias_adjacentes = nova;
                            break;
                        }
                    }
                }
            }
        }
    }
}

void percursoEntreRodovias(Rodovia *listaRodovias, char cidadeInicio[], char cidadeFim[])
{
    Rodovia *rodoviaInicio = NULL, *rodoviaFim = NULL;
    Cidade *cidadeInicioPtr = NULL, *cidadeFimPtr = NULL;
    char cidadeInicioMinuscula[50], cidadeFimMinuscula[50], cidadeAtualMinuscula[50];
    char cidadeInicioOriginal[50], cidadeFimOriginal[50];

    cidadeInicio[strcspn(cidadeInicio, "\n")] = 0;
    cidadeFim[strcspn(cidadeFim, "\n")] = 0;

    strcpy(cidadeInicioMinuscula, cidadeInicio);
    strcpy(cidadeFimMinuscula, cidadeFim);
    converterMinusculo(cidadeInicioMinuscula);
    converterMinusculo(cidadeFimMinuscula);
    removerEspacos(cidadeInicioMinuscula);
    removerEspacos(cidadeFimMinuscula);

    for (Rodovia *r = listaRodovias; r != NULL; r = r->proxima)
    {
        for (Cidade *c = r->cidades; c != NULL; c = c->proxima)
        {
            char cidadeTemp[50];
            strcpy(cidadeTemp, c->nomeCidade);
            char *virgula = strchr(cidadeTemp, ',');
            if (virgula)
                *virgula = '\0';

            strcpy(cidadeAtualMinuscula, cidadeTemp);
            converterMinusculo(cidadeAtualMinuscula);
            removerEspacos(cidadeAtualMinuscula);

            if (strcmp(cidadeAtualMinuscula, cidadeInicioMinuscula) == 0)
            {
                rodoviaInicio = r;
                cidadeInicioPtr = c;
                strcpy(cidadeInicioOriginal, cidadeTemp);
            }
            if (strcmp(cidadeAtualMinuscula, cidadeFimMinuscula) == 0)
            {
                rodoviaFim = r;
                cidadeFimPtr = c;
                strcpy(cidadeFimOriginal, cidadeTemp);
            }
        }
    }

    if (!rodoviaInicio || !rodoviaFim)
    {
        printf("Cidade de início ou fim não encontrada!\n");
        return;
    }

    printf("\n=== Percurso de %s até %s ===\n", cidadeInicioOriginal, cidadeFimOriginal);

    if (rodoviaInicio == rodoviaFim)
    {
        printf("Percurso na rodovia %s:\n", rodoviaInicio->nome);
        percursoEntreCidades(rodoviaInicio, cidadeInicioOriginal, cidadeFimOriginal);
        return;
    }

    RodoviaAdjacente *adj = rodoviaInicio->rodovias_adjacentes;
    while (adj != NULL)
    {
        if (strcmp(adj->nomeRodovia, rodoviaFim->nome) == 0)
        {
            float pedagioTotal = 0.0;
            float distanciaTotal = 0.0;

            printf("Rota encontrada:\n");
            printf("1. Início em %s pela rodovia %s\n", cidadeInicioOriginal, rodoviaInicio->nome);
            printf("2. Conexão em %s\n", adj->cidadeConexao);
            printf("3. Continuação pela rodovia %s até %s\n", rodoviaFim->nome, cidadeFimOriginal);

            printf("\nPrimeiro trecho (%s):\n", rodoviaInicio->nome);
            float pedagio1 = 0.0, distancia1 = 0.0;
            calcularPercurso(rodoviaInicio, cidadeInicioOriginal, adj->cidadeConexao, &pedagio1, &distancia1);
            pedagioTotal += pedagio1;
            distanciaTotal += distancia1;

            printf("\nSegundo trecho (%s):\n", rodoviaFim->nome);
            float pedagio2 = 0.0, distancia2 = 0.0;
            calcularPercurso(rodoviaFim, adj->cidadeConexao, cidadeFimOriginal, &pedagio2, &distancia2);
            pedagioTotal += pedagio2;
            distanciaTotal += distancia2;

            printf("\n=== Resumo do percurso completo ===\n");
            printf("Distância total da viagem: %.2f km\n", distanciaTotal);
            printf("Custo total em pedágios: R$ %.2f\n", pedagioTotal);
            return;
        }
        adj = adj->proxima;
    }

    printf("Não foi possível encontrar uma rota direta entre as cidades.\n");
}

void calcularPercurso(Rodovia *rodovia, char cidadeInicio[], char cidadeFim[], float *pedagioTotal, float *distanciaTotal)
{
    Cidade *atual = rodovia->cidades;
    Cidade *cidadeInicial = NULL;
    Cidade *cidadeFinal = NULL;
    *pedagioTotal = 0;
    *distanciaTotal = 0;

    while (atual != NULL)
    {
        if (strcmp(atual->nomeCidade, cidadeInicio) == 0)
            cidadeInicial = atual;
        if (strcmp(atual->nomeCidade, cidadeFim) == 0)
            cidadeFinal = atual;
        atual = atual->proxima;
    }

    if (!cidadeInicial || !cidadeFinal)
        return;

    printf("\n=== Percurso na rodovia %s ===\n", rodovia->nome);
    printf("Início do percurso em: %s (km %.2f)\n", cidadeInicio, cidadeInicial->distancia);

    atual = cidadeInicial;
    while (atual != cidadeFinal && atual != NULL)
    {
        Cidade *proxima = atual->proxima;
        if (proxima != NULL)
        {
            float trechoDistancia = proxima->distancia - atual->distancia;
            *distanciaTotal += trechoDistancia;

            printf("\n-> Próxima cidade: %s (km %.2f)\n", proxima->nomeCidade, proxima->distancia);
            printf("   Distância do trecho: %.2f km\n", trechoDistancia);

            Pedagio *pedagioAtual = atual->pedagios;
            while (pedagioAtual != NULL)
            {
                if (strcmp(pedagioAtual->nomeCidade, proxima->nomeCidade) == 0)
                {
                    *pedagioTotal += pedagioAtual->valor;
                    break;
                }
                pedagioAtual = pedagioAtual->proximo;
            }

            if (pedagioAtual == NULL)
            {
                pedagioAtual = proxima->pedagios;
                while (pedagioAtual != NULL)
                {
                    if (strcmp(pedagioAtual->nomeCidade, atual->nomeCidade) == 0)
                    {
                        *pedagioTotal += pedagioAtual->valor;
                        break;
                    }
                    pedagioAtual = pedagioAtual->proximo;
                }
            }
        }
        atual = atual->proxima;
    }

    printf("\n=== Fim do percurso ===\n");
    printf("Distância total percorrida: %.2f km\n", *distanciaTotal);
    printf("Custo total em pedágios: R$ %.2f\n", *pedagioTotal);
}

void removerEspacos(char *texto)

{
    char *inicio = texto;
    while (*inicio && isspace(*inicio))
        inicio++;
    if (inicio != texto)
    {
        memmove(texto, inicio, strlen(inicio) + 1);
    }

    int fim = strlen(texto) - 1;
    while (fim >= 0 && isspace(texto[fim]))
    {
        texto[fim] = '\0';
        fim--;
    }

    char *virgula = strchr(texto, ',');
    if (virgula != NULL)
    {
        char *espacoAntesVirgula = virgula - 1;
        while (espacoAntesVirgula > texto && isspace(*espacoAntesVirgula))
        {
            memmove(espacoAntesVirgula, espacoAntesVirgula + 1, strlen(espacoAntesVirgula));
            virgula--;
        }
    }
}

void liberarMemoria(Rodovia *lista)
{
    while (lista != NULL)
    {
        Rodovia *proximaRodovia = lista->proxima;

        Cidade *cidadeAtual = lista->cidades;
        while (cidadeAtual != NULL)
        {
            Cidade *proximaCidade = cidadeAtual->proxima;

            Pedagio *pedagioAtual = cidadeAtual->pedagios;
            while (pedagioAtual != NULL)
            {
                Pedagio *proximoPedagio = pedagioAtual->proximo;
                free(pedagioAtual);
                pedagioAtual = proximoPedagio;
            }

            free(cidadeAtual);
            cidadeAtual = proximaCidade;
        }

        RodoviaAdjacente *adjAtual = lista->rodovias_adjacentes;
        while (adjAtual != NULL)
        {
            RodoviaAdjacente *proximaAdj = adjAtual->proxima;
            free(adjAtual);
            adjAtual = proximaAdj;
        }

        free(lista);
        lista = proximaRodovia;
    }
}
