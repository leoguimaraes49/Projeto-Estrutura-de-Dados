
README - Projeto Sequence Set

Este projeto implementa um Sequence Set em C++ para armazenar dados de atletas. O Sequence Set é uma estrutura de dados baseada em blocos de tamanho fixo, onde cada bloco armazena registros ordenados por uma chave (neste caso, o campo id). Cada bloco aponta para o próximo, formando uma sequência encadeada em um arquivo binário.

---

Estrutura do Arquivo

1. Cabeçalho  
   - Contém metadados como:
     - posPrimeiroBloco: índice do primeiro bloco de dados (ou -1 se nenhum).  
     - proximoBlocoLivre: índice onde será criado o próximo bloco.  
     - totalBlocos: quantos blocos foram criados no arquivo.  

2. Blocos  
   - Cada bloco tem:
     - quantidadeRegistros: quantos registros estão ocupados.  
     - menorId e maiorId: delimitam o intervalo de IDs contidos no bloco.  
     - proxBloco: índice do próximo bloco (ou -1 se não houver).  
     - Um array fixo de registros (Atleta), ordenados por id.

3. Registros (Atleta)  
   - Cada registro possui id, nome, cidade, esporte, evento e noc.  
   - Usamos char[] para facilitar a escrita e leitura binária.

---

Funcionalidades

1. Criar/Zerar Arquivo Binário  
   - Apaga o arquivo antigo e grava um cabeçalho inicial, sem blocos.  

2. Ler CSV e Inserir Registros  
   - Lê o arquivo dados.csv (ignorando a 1ª linha de cabeçalho) e, para cada linha, cria um registro de Atleta e insere no Sequence Set.  

3. Inserir Registro via Teclado  
   - Permite digitar os campos (id, nome, etc.) e insere no Sequence Set.  

4. Remover Registro por ID  
   - Percorre os blocos procurando o id. Ao encontrar, remove deslocando registros no bloco e atualiza o menorId/maiorId.  

5. Buscar Registro por ID  
   - Varre o encadeamento de blocos (observando menorId e maiorId), buscando o id específico.  

6. Exibir Blocos  
   - Lê todos os blocos gravados no arquivo e mostra as informações de cada um (quantidade de registros, intervalo de id, proxBloco) e cada registro armazenado.

---

Compilação e Execução

Compilação

No Linux (usando g++):
```bash
g++ -o sequence_set main.cpp
```
No Windows (MinGW):
```bash
g++ -o sequence_set.exe main.cpp
```
Ou, se estiver usando uma IDE (Code::Blocks, Visual Studio, etc.), basta criar um projeto e incluir o main.cpp.

Execução

Após a compilação:
```bash
./sequence_set
```
(em Linux), ou
```bash
sequence_set.exe
```
(em Windows).

O programa exibirá um menu com as opções:

1. Criar/Zerar arquivo binário  
2. Ler CSV e inserir registros  
3. Inserir registro via teclado  
4. Remover registro por ID  
5. Buscar registro por ID  
6. Exibir blocos  
7. Sair  

---

Estrutura de Pastas

```
/projeto
  |-- main.cpp          (código fonte principal)
  |-- dados.csv         (arquivo CSV com colunas: Id,Name,City,Sport,Event,NOC)
  |-- sequence_set.bin  (arquivo binário criado em tempo de execução)
  |-- README.md         (este arquivo)
```

---

Observações

- O arquivo CSV (dados.csv) deve ter uma linha de cabeçalho:  
  ```
  Id,Name,City,Sport,Event,NOC
  ```
  seguida das linhas com valores correspondentes.
- A alocação de memória para armazenar os registros do CSV é feita com new, para evitar problemas de estouro de pilha se o CSV for grande.
- Se desejar recomeçar o arquivo binário, use a opção 1 do menu (Criar/Zerar arquivo binário).

---

Fim do README
