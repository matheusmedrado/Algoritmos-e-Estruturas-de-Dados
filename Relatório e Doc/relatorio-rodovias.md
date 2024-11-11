# Relatório: Sistema de Gerenciamento de Rodovias

## Introdução

O problema apresentado envolve o desenvolvimento de um sistema para gerenciamento de rodovias e suas cidades interligadas, com funcionalidades para controle de pedágios e análise de percursos. A solução foi implementada utilizando estruturas de dados encadeadas em C, especificamente duas estruturas principais:

### Estruturas de Dados Utilizadas

1. **Estrutura Rodovia**:
```c
typedef struct Rodovia {
    char nome[50];
    Cidade *cidades;
    float pedagio;
    struct Rodovia *proxima;
} Rodovia;
```

2. **Estrutura Cidade**:
```c
typedef struct Cidade {
    char nomeCidade[50];
    float distancia;
    float pedagio;
    struct Cidade *proxima;
    struct Cidade *anterior;
} Cidade;
```

A solução foi desenhada usando uma lista encadeada simples para as rodovias e uma lista duplamente encadeada para as cidades dentro de cada rodovia. Esta escolha foi feita considerando:

- A lista simples para rodovias é suficiente pois não há necessidade de navegação bidirecional entre rodovias
- A lista dupla para cidades facilita operações como remoção e inserção ordenada por distância
- O encadeamento duplo permite calcular percursos nos dois sentidos da rodovia

## Documentação do Código

### Funções Principais

#### 1. Gerenciamento de Rodovias

**`Rodovia *inserirRodovia(Rodovia *lista, char nome[])`**
- Cria uma nova rodovia com o nome especificado
- Verifica duplicidade de nomes (case-insensitive)
- Aloca memória dinamicamente para a nova rodovia
- Insere no início da lista de rodovias
- Retorna a nova lista atualizada

**`Rodovia *removerRodovia(Rodovia *lista, char nome[])`**
- Busca a rodovia pelo nome
- Ajusta os ponteiros da lista
- Libera a memória da rodovia removida
- Retorna a lista atualizada

#### 2. Gerenciamento de Cidades

**`Cidade *inserirCidade(Rodovia *rodovia, char nomeCidade[], float distancia)`**
- Cria uma nova cidade com nome e distância especificados
- Insere a cidade na posição correta baseada na distância
- Mantém a lista de cidades ordenada por distância
- Atualiza ponteiros anterior/próximo apropriadamente

**`void removerCidade(Rodovia *rodovia, char nomeCidade[])`**
- Localiza a cidade pelo nome
- Ajusta os ponteiros da lista duplamente encadeada
- Libera a memória da cidade removida

#### 3. Funcionalidades de Análise

**`void percursoEntreCidades(Rodovia *rodovia, char cidadeInicio[], char cidadeFim[])`**
- Calcula e exibe o percurso entre duas cidades
- Mostra distâncias parciais e totais
- Calcula custos de pedágio no trajeto

**`void listarCruzamentos(Rodovia *rodovia1, Rodovia *rodovia2)`**
- Identifica pontos em comum entre duas rodovias
- Mostra as distâncias relativas em cada rodovia

## Exemplos de Uso

### 1. Criando e Populando uma Rodovia

```c
Rodovia *lista = NULL;
lista = inserirRodovia(lista, "BR-101");
inserirCidade(lista, "Florianópolis", 0.0);
inserirCidade(lista, "Joinville", 180.5);
inserirCidade(lista, "Curitiba", 300.2);
```

Saída:
```
Rodovia: BR-101
Cidade: Florianópolis, Distância: 0.00
Cidade: Joinville, Distância: 180.50
Cidade: Curitiba, Distância: 300.20
```

### 2. Calculando Percurso

```c
percursoEntreCidades(lista, "Florianópolis", "Curitiba");
```

Saída:
```
=== Percurso na rodovia BR-101 ===
Início do percurso em: Florianópolis (km 0.00)

-> Próxima cidade: Joinville (km 180.50)
   Distância do trecho: 180.50 km

-> Próxima cidade: Curitiba (km 300.20)
   Distância do trecho: 119.70 km

=== Fim do percurso ===
Distância total percorrida: 300.20 km
Custo total em pedágios: R$ 0.00
```

### 3. Verificando Cruzamentos

```c
Rodovia *lista = NULL;
lista = inserirRodovia(lista, "BR-101");
inserirCidade(lista, "São Paulo", 0.0);
inserirCidade(lista, "Curitiba", 408.2);

lista = inserirRodovia(lista, "BR-116");
inserirCidade(lista, "Curitiba", 0.0);
inserirCidade(lista, "Porto Alegre", 546.5);

listarCruzamentos(lista, lista->proxima);
```

Saída:
```
Pontos de cruzamento entre BR-101 e BR-116:
- Curitiba (km 408.20 na BR-101, km 0.00 na BR-116)
```

## Conclusão

### Desafios Enfrentados

1. **Gerenciamento de Memória**: 
   - A implementação cuidadosa da liberação de memória foi crucial para evitar vazamentos
   - O uso de ponteiros duplos em algumas funções foi necessário para modificar a lista original

2. **Ordenação das Cidades**: 
   - Manter as cidades ordenadas por distância durante a inserção requereu atenção especial aos ponteiros
   - A lista duplamente encadeada facilitou este processo mas aumentou a complexidade do código

3. **Manipulação de Strings**:
   - A comparação case-insensitive de nomes exigiu cuidados especiais
   - O tratamento de strings em C requereu atenção para evitar buffer overflows

### Lições Aprendidas

1. A escolha da estrutura de dados correta (lista dupla para cidades) simplificou várias operações
2. A modularização do código em funções específicas melhorou a manutenibilidade
3. A implementação de funções auxiliares (como toLowerStr) reduziu a duplicação de código
4. O uso de constantes e tipos definidos facilitou futuras modificações no código
5. A importância de validar entradas e tratar erros adequadamente

A implementação demonstrou a eficácia das estruturas de dados escolhidas para resolver o problema proposto, permitindo um gerenciamento eficiente de rodovias e cidades com suas respectivas características e relacionamentos.
