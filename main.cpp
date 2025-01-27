/**
 * Projeto Pr�tico GAC108 � Estruturas de Dados
 * @autor Leonardo Guimar�es Oliveira
 * Data: 26 de Janeiro de 2025
 */

#include <iostream>
#include <fstream>
#include <cstring>
#include <string>
#include <stdexcept>
using namespace std;

static const char* ARQUIVO_BINARIO = "sequence_set.bin";
static const char* ARQUIVO_CSV     = "dados.csv";

static const int TAMANHO_BLOCO = 50;
static const int MAX_REG_CSV   = 2000;

// Estrutura para cabe�alho do arquivo bin�rio
struct Cabecalho {
    int posPrimeiroBloco;
    int proximoBlocoLivre;
    int totalBlocos;
};

// Cada atleta (Id,Name,City,Sport,Event,NOC)
struct Atleta {
    int  id;
    char nome[50];
    char cidade[50];
    char esporte[50];
    char evento[50];
    char noc[10];
};

// Cada bloco do Sequence Set: at� 50 atletas e um ponteiro para o pr�ximo bloco
struct Bloco {
    int  quantidadeRegistros;
    int  menorId;
    int  maiorId;
    int  proxBloco;
    Atleta registros[TAMANHO_BLOCO];
};

// Tamanhos em bytes
static const int TAMANHO_CABECALHO = sizeof(Cabecalho);
static const int TAMANHO_BLOCO_ARQ = sizeof(Bloco);

// Calcula a posi��o, em bytes, de um bloco 'indice' no arquivo
streampos obterOffsetBloco(int indice) {
    return TAMANHO_CABECALHO + indice * TAMANHO_BLOCO_ARQ;
}

// Gravar/Ler cabe�alho
void gravarCabecalho(fstream &arq, const Cabecalho &c) {
    arq.seekp(0, ios::beg);
    arq.write(reinterpret_cast<const char*>(&c), TAMANHO_CABECALHO);
}

void lerCabecalho(fstream &arq, Cabecalho &c) {
    arq.seekg(0, ios::beg);
    arq.read(reinterpret_cast<char*>(&c), TAMANHO_CABECALHO);
}

// Gravar/Ler um bloco
void gravarBloco(fstream &arq, int indice, const Bloco &b) {
    arq.seekp(obterOffsetBloco(indice), ios::beg);
    arq.write(reinterpret_cast<const char*>(&b), TAMANHO_BLOCO_ARQ);
}

void lerBloco(fstream &arq, int indice, Bloco &b) {
    arq.seekg(obterOffsetBloco(indice), ios::beg);
    arq.read(reinterpret_cast<char*>(&b), TAMANHO_BLOCO_ARQ);
}

// Apaga o arquivo bin�rio e grava um cabe�alho inicial vazio
void criarArquivoVazio() {
    ofstream ofs(ARQUIVO_BINARIO, ios::binary | ios::trunc);
    ofs.close();

    try {
        fstream arq(ARQUIVO_BINARIO, ios::in | ios::out | ios::binary);
        if(!arq.is_open()) {
            throw runtime_error("Erro ao criar arquivo binario!");
        }
        Cabecalho cab;
        cab.posPrimeiroBloco  = -1;
        cab.proximoBlocoLivre = 0;
        cab.totalBlocos       = 0;
        gravarCabecalho(arq, cab);

        arq.close();
        cout << "Arquivo binario criado/zerado.\n";
    }
    catch(const exception &e) {
        cout << "Excecao: " << e.what() << "\n";
    }
}

// Cria um novo bloco vazio no indice 'indice'
void criarNovoBloco(fstream &arq, Cabecalho &cab, int indice) {
    Bloco b;
    b.quantidadeRegistros = 0;
    b.menorId = -1;
    b.maiorId = -1;
    b.proxBloco = -1;

    for(int i = 0; i < TAMANHO_BLOCO; i++) {
        b.registros[i].id = -1;
        memset(b.registros[i].nome,    0, sizeof(b.registros[i].nome));
        memset(b.registros[i].cidade,  0, sizeof(b.registros[i].cidade));
        memset(b.registros[i].esporte, 0, sizeof(b.registros[i].esporte));
        memset(b.registros[i].evento,  0, sizeof(b.registros[i].evento));
        memset(b.registros[i].noc,     0, sizeof(b.registros[i].noc));
    }

    gravarBloco(arq, indice, b);
    cab.totalBlocos++;
    if(cab.posPrimeiroBloco == -1) {
        cab.posPrimeiroBloco = indice;
    }
    cab.proximoBlocoLivre = indice + 1;
    gravarCabecalho(arq, cab);
}

// Insere um atleta ordenadamente em um bloco. Retorna false se cheio
bool inserirNoBloco(Bloco &b, const Atleta &A) {
    if(b.quantidadeRegistros >= TAMANHO_BLOCO) return false;

    int pos = 0;
    bool fim = false;
    while(!fim) {
        if(pos >= b.quantidadeRegistros) {
            fim = true;
        } else {
            if(b.registros[pos].id < A.id) pos++;
            else fim = true;
        }
        if(pos >= TAMANHO_BLOCO) fim = true;
    }

    for(int i = b.quantidadeRegistros; i > pos; i--) {
        b.registros[i] = b.registros[i - 1];
    }
    b.registros[pos] = A;
    b.quantidadeRegistros++;

    b.menorId = b.registros[0].id;
    b.maiorId = b.registros[b.quantidadeRegistros - 1].id;
    return true;
}

// Percorre blocos para inserir um Atleta. Cria novo bloco se o �ltimo estiver cheio
void inserirAtleta(const Atleta &A) {
    try {
        fstream arq(ARQUIVO_BINARIO, ios::in | ios::out | ios::binary);
        if(!arq.is_open()) {
            throw runtime_error("Erro ao abrir arquivo binario para inserir!");
        }
        Cabecalho cab;
        lerCabecalho(arq, cab);

        if(cab.totalBlocos == 0) {
            criarNovoBloco(arq, cab, cab.proximoBlocoLivre);
            lerCabecalho(arq, cab);
        }

        bool inserido = false;
        int idx = cab.posPrimeiroBloco;
        while(!inserido) {
            Bloco b;
            lerBloco(arq, idx, b);
            bool coube = inserirNoBloco(b, A);
            if(coube) {
                gravarBloco(arq, idx, b);
                inserido = true;
            } else {
                if(b.proxBloco == -1) {
                    int novo = cab.proximoBlocoLivre;
                    criarNovoBloco(arq, cab, novo);
                    lerCabecalho(arq, cab);

                    Bloco b2;
                    lerBloco(arq, novo, b2);
                    inserirNoBloco(b2, A);
                    gravarBloco(arq, novo, b2);

                    b.proxBloco = novo;
                    gravarBloco(arq, idx, b);
                    inserido = true;
                } else {
                    idx = b.proxBloco;
                }
            }
        }
        arq.close();
    }
    catch(const exception &e) {
        cout << "Excecao em inserirAtleta: " << e.what() << "\n";
    }
}

// Remove um registro por ID
void removerAtleta(int id) {
    try {
        fstream arq(ARQUIVO_BINARIO, ios::in | ios::out | ios::binary);
        if(!arq.is_open()) {
            throw runtime_error("Erro ao abrir arquivo binario para remocao!");
        }
        Cabecalho cab;
        lerCabecalho(arq, cab);

        if(cab.posPrimeiroBloco == -1) {
            arq.close();
            cout << "Registro nao encontrado (arquivo vazio).\n";
            return;
        }

        bool removido = false;
        bool fim = false;
        int idx = cab.posPrimeiroBloco;

        while(!fim) {
            if(idx == -1) {
                fim = true;
            } else {
                Bloco b;
                lerBloco(arq, idx, b);

                bool achou = false;
                for(int i=0; i<b.quantidadeRegistros && !achou; i++) {
                    if(b.registros[i].id == id) {
                        for(int j=i; j<b.quantidadeRegistros-1; j++) {
                            b.registros[j] = b.registros[j+1];
                        }
                        b.quantidadeRegistros--;
                        if(b.quantidadeRegistros > 0) {
                            b.menorId = b.registros[0].id;
                            b.maiorId = b.registros[b.quantidadeRegistros - 1].id;
                        } else {
                            b.menorId = -1;
                            b.maiorId = -1;
                        }
                        gravarBloco(arq, idx, b);
                        removido = true;
                        achou = true;
                    }
                }
                if(removido) fim = true;
                else idx = b.proxBloco;
            }
        }
        arq.close();

        if(removido) cout << "Registro removido com sucesso.\n";
        else         cout << "Registro nao encontrado.\n";
    }
    catch(const exception &e) {
        cout << "Excecao em removerAtleta: " << e.what() << "\n";
    }
}

// Busca um registro por ID
bool buscarAtleta(int id, Atleta &retorno) {
    try {
        fstream arq(ARQUIVO_BINARIO, ios::in | ios::binary);
        if(!arq.is_open()) {
            throw runtime_error("Erro ao abrir arquivo binario para busca!");
        }
        Cabecalho cab;
        lerCabecalho(arq, cab);

        if(cab.posPrimeiroBloco == -1) {
            arq.close();
            return false;
        }

        bool fim = false;
        bool achou = false;
        int idx = cab.posPrimeiroBloco;

        while(!fim) {
            if(idx == -1) {
                fim = true;
            } else {
                Bloco b;
                lerBloco(arq, idx, b);

                if(b.menorId != -1 && id < b.menorId) {
                    fim = true;
                } else if(b.maiorId != -1 && id > b.maiorId) {
                    idx = b.proxBloco;
                } else {
                    for(int i=0; i<b.quantidadeRegistros && !achou; i++) {
                        if(b.registros[i].id == id) {
                            retorno = b.registros[i];
                            achou = true;
                        }
                    }
                    if(!achou) idx = b.proxBloco;
                }
            }
            if(achou) fim = true;
        }
        arq.close();
        return achou;
    }
    catch(const exception &e) {
        cout << "Excecao em buscarAtleta: " << e.what() << "\n";
        return false;
    }
}

// Exibe todos os blocos do arquivo
void exibirBlocos() {
    try {
        fstream arq(ARQUIVO_BINARIO, ios::in | ios::binary);
        if(!arq.is_open()) {
            throw runtime_error("Erro ao abrir arquivo binario para leitura!");
        }
        Cabecalho cab;
        lerCabecalho(arq, cab);

        cout << "\n===== CABECALHO =====\n";
        cout << "posPrimeiroBloco = "  << cab.posPrimeiroBloco
             << ", proximoBlocoLivre = " << cab.proximoBlocoLivre
             << ", totalBlocos = "       << cab.totalBlocos << "\n\n";

        for(int i=0; i<cab.totalBlocos; i++) {
            Bloco b;
            lerBloco(arq, i, b);
            cout << "BLOCO " << i
                 << ": qtdRegistros = " << b.quantidadeRegistros
                 << ", menorId = "      << b.menorId
                 << ", maiorId = "      << b.maiorId
                 << ", proxBloco = "    << b.proxBloco << "\n";

            for(int k=0; k<b.quantidadeRegistros; k++) {
                const Atleta &A = b.registros[k];
                cout << "  [" << k << "] "
                     << "Id = "     << A.id
                     << ", Nome = "    << A.nome
                     << ", Cidade = "  << A.cidade
                     << ", Esporte = " << A.esporte
                     << ", Evento = "  << A.evento
                     << ", NOC = "     << A.noc << "\n";
            }
            cout << "\n";
        }
        arq.close();
    }
    catch(const exception &e) {
        cout << "Excecao em exibirBlocos: " << e.what() << "\n";
    }
}

// Separa cada linha CSV em 6 campos (Id,Name,City,Sport,Event,NOC)
void processarLinhaCSV(const string &linha, string campos[6]) {
    bool entreAspas = false;
    string temp;
    int idx = 0;
    for(char c : linha) {
        if(c == '"') {
            entreAspas = !entreAspas;
        } else if(c == ',' && !entreAspas) {
            if(idx < 6) campos[idx] = temp;
            temp.clear();
            idx++;
        } else {
            temp.push_back(c);
        }
    }
    if(!temp.empty() && idx < 6) {
        campos[idx] = temp;
    }
}

// L� o CSV, converte campos e preenche um array de 'Atleta'
int lerArquivoCSV(Atleta* vetor) {
    ifstream fin(ARQUIVO_CSV);
    cout << "Lendo registros do CSV... Aguarde...\n";
    if(!fin.is_open()) {
        cout << "Erro ao abrir " << ARQUIVO_CSV << "\n";
        return 0;
    }

    string linha;
    getline(fin, linha); // ignora cabe�alho

    int count = 0;
    while(count < MAX_REG_CSV && getline(fin, linha)) {
        if(linha.size() < 2) {
            continue;
        }

        string campos[6];
        processarLinhaCSV(linha, campos);

        int idTmp = -1;
        bool ok = true;
        try {
            idTmp = stoi(campos[0]);
        } catch(...) {
            ok = false;
        }

        if(ok) {
            vetor[count].id = idTmp;

            strncpy(vetor[count].nome, campos[1].c_str(), sizeof(vetor[count].nome) - 1);
            vetor[count].nome[sizeof(vetor[count].nome) - 1] = '\0';

            strncpy(vetor[count].cidade, campos[2].c_str(), sizeof(vetor[count].cidade) - 1);
            vetor[count].cidade[sizeof(vetor[count].cidade) - 1] = '\0';

            strncpy(vetor[count].esporte, campos[3].c_str(), sizeof(vetor[count].esporte) - 1);
            vetor[count].esporte[sizeof(vetor[count].esporte) - 1] = '\0';

            strncpy(vetor[count].evento, campos[4].c_str(), sizeof(vetor[count].evento) - 1);
            vetor[count].evento[sizeof(vetor[count].evento) - 1] = '\0';

            strncpy(vetor[count].noc, campos[5].c_str(), sizeof(vetor[count].noc) - 1);
            vetor[count].noc[sizeof(vetor[count].noc) - 1] = '\0';

            count++;
        }
    }

    fin.close();
    return count;
}

// L� dados via teclado e insere um novo atleta
void inserirViaTeclado() {
    Atleta A;
    memset(&A, 0, sizeof(A));

    cout << "Digite o ID: ";
    cin >> A.id;
    cin.ignore();

    cout << "Nome: ";
    cin.getline(A.nome, sizeof(A.nome));

    cout << "Cidade: ";
    cin.getline(A.cidade, sizeof(A.cidade));

    cout << "Esporte: ";
    cin.getline(A.esporte, sizeof(A.esporte));

    cout << "Evento: ";
    cin.getline(A.evento, sizeof(A.evento));

    cout << "NOC: ";
    cin.getline(A.noc, sizeof(A.noc));

    inserirAtleta(A);
    cout << "Registro inserido com sucesso.\n";
}

int main() {
    bool sair = false;
    while(!sair) {
        cout << "\n=========== MENU ===========\n";
        cout << "1 - Criar/Zerar arquivo binario\n";
        cout << "2 - Ler CSV e inserir registros\n";
        cout << "3 - Inserir registro via teclado\n";
        cout << "4 - Remover registro por ID\n";
        cout << "5 - Buscar registro por ID\n";
        cout << "6 - Exibir blocos\n";
        cout << "7 - Sair\n";
        cout << "Opcao: ";
        int opc;
        cin >> opc;
        cin.ignore();

        switch(opc) {
            case 1: {
                criarArquivoVazio();
            } break;

            case 2: {
                Atleta* vetor = new Atleta[MAX_REG_CSV];
                int qtd = lerArquivoCSV(vetor);
                cout << "Lidos " << qtd << " registros do CSV.\n";
                cout << "Inserindo registros no Sequence Set...\n";
                for(int i=0; i<qtd; i++) {
                    inserirAtleta(vetor[i]);
                }
                cout << "Registros inseridos com sucesso!\n";
                delete[] vetor;
            } break;

            case 3: {
                inserirViaTeclado();
            } break;

            case 4: {
                cout << "Digite o ID a remover: ";
                int rid;
                cin >> rid;
                removerAtleta(rid);
            } break;

            case 5: {
                cout << "Digite o ID a buscar: ";
                int bid;
                cin >> bid;
                Atleta resultado;
                bool ok = buscarAtleta(bid, resultado);
                if(ok) {
                    cout << "Encontrado:\n";
                    cout << "Id = "       << resultado.id
                         << ", Nome = "    << resultado.nome
                         << ", Cidade = "  << resultado.cidade
                         << ", Esporte = " << resultado.esporte
                         << ", Evento = "  << resultado.evento
                         << ", NOC = "     << resultado.noc << "\n";
                } else {
                    cout << "Nao encontrado.\n";
                }
            } break;

            case 6: {
                exibirBlocos();
            } break;

            case 7: {
                sair = true;
            } break;

            default: {
                cout << "Opcao invalida.\n";
            } break;
        }
    }
    return 0;
}
