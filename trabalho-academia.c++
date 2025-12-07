

#include <bits/stdc++.h>
using namespace std;
using ll = long long;

static const string EXERCICIOS_FILE = "exercicios.txt";
static const string FICHAS_FILE = "fichas.txt";
static const string HISTORICO_FILE = "historico.txt";

string trim(const string &s) {
    size_t a = s.find_first_not_of(" \t\r\n");
    if (a == string::npos) return "";
    size_t b = s.find_last_not_of(" \t\r\n");
    return s.substr(a, b - a + 1);
}

vector<string> split(const string &s, char delim=';') {
    vector<string> out;
    string cur;
    stringstream ss(s);
    while (getline(ss, cur, delim)) out.push_back(cur);
    return out;
}

string now_datetime_string() {
    time_t t = time(nullptr);
    tm tmv;
    localtime_r(&t, &tmv);
    char buf[64];
    strftime(buf, sizeof(buf), "%d/%m/%Y %H:%M:%S", &tmv);
    return string(buf);
}

class Exercicio {
protected:
    int id;
    string nome;
    bool ativo;
    static int proximoId;
public:
    Exercicio(string n) {
        this->nome = n;
        this->ativo = true;
        this->id = proximoId++;
    }
    Exercicio(int idExistente, string n, bool status) {
        this->id = idExistente;
        this->nome = n;
        this->ativo = status;
        if (idExistente >= proximoId) proximoId = idExistente + 1;
    }

    virtual ~Exercicio() = default;

    int getId() const { return id; }
    string getNome() const { return nome; }
    bool isAtivo() const { return ativo; }
    void setAtivo(bool v) { ativo = v; }

    virtual double calculaTempoMinutos() const = 0;   // tempo estimado em minutos
    virtual double calculaCalorias() const = 0;       // calorias estimadas
    virtual int tipo() const = 0; // 1 = Cardio, 2 = Força
    virtual void exibirDetalhes() const = 0;

    virtual string toArquivo() const = 0;
};

int Exercicio::proximoId = 1;

class Cardio : public Exercicio {
private:
    int duracao;
    double calPorMin;
public:
    Cardio(string n, int duracaoMin, double calMin) : Exercicio(n), duracao(duracaoMin), calPorMin(calMin) {}

    Cardio(int idExistente, string n, int duracaoMin, double calMin, bool status)
        : Exercicio(idExistente, n, status), duracao(duracaoMin), calPorMin(calMin) {}

    virtual double calculaTempoMinutos() const override {
        return (double)duracao;
    }
    virtual double calculaCalorias() const override {
        return duracao * calPorMin;
    }
    virtual int tipo() const override { return 1; }
    virtual void exibirDetalhes() const override {
        cout << "Tipo: Cardio\n";
        cout << "ID: " << id << " | Nome: " << nome << " | Status: " << (ativo ? "Ativo" : "Inativo") << "\n";
        cout << "Duracao (min): " << duracao << " | Calorias/min: " << calPorMin << "\n";
    }
    virtual string toArquivo() const override {
        stringstream ss;
        ss << 1 << ";" << id << ";" << nome << ";" << duracao << ";" 
           << fixed << setprecision(2) << calPorMin << ";" << (ativo ? 1 : 0);
        return ss.str();
    }
};

class Forca : public Exercicio {
private:
    double carga;
    int series;
    int repeticoes;
    int descanso;
public:
    Forca(string n, double carga, int series, int repeticoes, int descanso)
        : Exercicio(n), carga(carga), series(series), repeticoes(repeticoes), descanso(descanso) {}

    Forca(int idExistente, string n, double carga, int series, int repeticoes, int descanso, bool status)
        : Exercicio(idExistente, n, status), carga(carga), series(series), repeticoes(repeticoes), descanso(descanso) {}

    virtual double calculaTempoMinutos() const override {

        double segundos = (double)series * repeticoes * 3.0 + (double)series * descanso;
        return segundos / 60.0;
    }
    virtual double calculaCalorias() const override {
        return (double)series * repeticoes * carga * 0.15;
    }
    virtual int tipo() const override { return 2; }
    virtual void exibirDetalhes() const override {
        cout << "Tipo: Forca\n";
        cout << "ID: " << id << " | Nome: " << nome << " | Status: " << (ativo ? "Ativo" : "Inativo") << "\n";
        cout << "Carga (kg): " << carga << " | Series: " << series << " | Repeticoes: " << repeticoes
             << " | Descanso (s): " << descanso << "\n";
    }
    virtual string toArquivo() const override {
        stringstream ss;
        ss << 2 << ";" << id << ";" << nome << ";" << fixed << setprecision(2) << carga << ";"
           << series << ";" << repeticoes << ";" << descanso << ";" << (ativo ? 1 : 0);
        return ss.str();
    }
};

class Ficha {
private:
    int id;
    string nome;
    vector<Exercicio*> lista;
    static int proximoIdFicha;
public:
    Ficha(string nome) : nome(nome) { id = proximoIdFicha++; }
    Ficha(int idExistente, string nomeExistente) : id(idExistente), nome(nomeExistente) {
        if (idExistente >= proximoIdFicha) proximoIdFicha = idExistente + 1;
    }
    int getId() const { return id; }
    string getNome() const { return nome; }

    void adicionarExercicio(Exercicio* e) {
        lista.push_back(e);
    }

    const vector<Exercicio*>& getExercicios() const { return lista; }

    double totalTempoMinutos() const {
        double s = 0.0;
        for (auto e : lista) if (e) s += e->calculaTempoMinutos();
        return s;
    }
    double totalCalorias() const {
        double s = 0.0;
        for (auto e : lista) if (e) s += e->calculaCalorias();
        return s;
    }

    void exibirFicha() const {
        cout << "Ficha ID: " << id << " | Nome: " << nome << "\n";
        if (lista.empty()) {
            cout << "  (sem exercicios)\n";
        } else {
            for (size_t i = 0; i < lista.size(); ++i) {
                auto e = lista[i];
                if (!e) continue;
                cout << "  [" << i+1 << "] ID " << e->getId() << " - " << e->getNome()
                     << " - Tipo: " << (e->tipo() == 1 ? "Cardio" : "Forca") << (e->isAtivo() ? "" : " (Inativo)") << "\n";
            }
        }
        cout << "Totais -> Tempo (min): " << fixed << setprecision(2) << totalTempoMinutos()
             << " | Calorias: " << fixed << setprecision(2) << totalCalorias() << "\n";
    }

    string toArquivo() const {
        stringstream ss;
        ss << id << ";" << nome << ";" << lista.size();
        for (auto e : lista) {
            ss << ";" << (e ? e->getId() : 0);
        }
        return ss.str();
    }
};

int Ficha::proximoIdFicha = 1;

struct RegistroHistorico {
    string dataHora;
    int idFicha;
    string nomeFicha;
    double tempoTotal;
    double caloriasTotal;

    string toArquivo() const {
        stringstream ss;
        ss << dataHora << ";" << idFicha << ";" << nomeFicha << ";" << fixed << setprecision(2) << tempoTotal << ";" << fixed << setprecision(2) << caloriasTotal;
        return ss.str();
    }

    void exibir() const {
        cout << dataHora << " | Ficha ID: " << idFicha << " | " << nomeFicha << " | Tempo (min): " << fixed << setprecision(2) << tempoTotal
             << " | Calorias: " << fixed << setprecision(2) << caloriasTotal << "\n";
    }
};

vector<unique_ptr<Exercicio>> repositorioExercicios;
vector<Ficha> repositorioFichas;
vector<RegistroHistorico> historicoExec;

Exercicio* buscarExercicioPorId(int id) {
    for (auto &p : repositorioExercicios) {
        if (p->getId() == id) return p.get();
    }
    return nullptr;
}

Ficha* buscarFichaPorId(int id) {
    for (auto &f : repositorioFichas) {
        if (f.getId() == id) return &f;
    }
    return nullptr;
}

void salvarExerciciosArquivo() {
    ofstream ofs(EXERCICIOS_FILE);
    if (!ofs) {
        cerr << "Erro ao abrir " << EXERCICIOS_FILE << " para escrita.\n";
        return;
    }
    for (auto &p : repositorioExercicios) {
        ofs << p->toArquivo() << "\n";
    }
    ofs.close();
}

void salvarFichasArquivo() {
    ofstream ofs(FICHAS_FILE);
    if (!ofs) {
        cerr << "Erro ao abrir " << FICHAS_FILE << " para escrita.\n";
        return;
    }
    for (auto &f : repositorioFichas) {
        ofs << f.toArquivo() << "\n";
    }
    ofs.close();
}

void salvarHistoricoArquivo() {
    ofstream ofs(HISTORICO_FILE);
    if (!ofs) {
        cerr << "Erro ao abrir " << HISTORICO_FILE << " para escrita.\n";
        return;
    }
    for (auto &r : historicoExec) {
        ofs << r.toArquivo() << "\n";
    }
    ofs.close();
}

void carregarExerciciosArquivo() {
    repositorioExercicios.clear();
    ifstream ifs(EXERCICIOS_FILE);
    if (!ifs) {
        return;
    }
    string linha;
    while (getline(ifs, linha)) {
        linha = trim(linha);
        if (linha.empty()) continue;
        auto campos = split(linha, ';');
        if (campos.size() < 6) continue;
        int tipo = stoi(campos[0]);
        int id = stoi(campos[1]);
        string nome = campos[2];
        if (tipo == 1) {
            int dur = stoi(campos[3]);
            double cpm = stod(campos[4]);
            bool status = (stoi(campos[5]) != 0);
            repositorioExercicios.emplace_back(new Cardio(id, nome, dur, cpm, status));
        } else if (tipo == 2) {
            if (campos.size() < 8) continue;
            double carga = stod(campos[3]);
            int series = stoi(campos[4]);
            int rep = stoi(campos[5]);
            int descanso = stoi(campos[6]);
            bool status = (stoi(campos[7]) != 0);
            repositorioExercicios.emplace_back(new Forca(id, nome, carga, series, rep, descanso, status));
        }
    }
    ifs.close();
}

void carregarFichasArquivo() {
    repositorioFichas.clear();
    ifstream ifs(FICHAS_FILE);
    if (!ifs) return;
    string linha;
    while (getline(ifs, linha)) {
        linha = trim(linha);
        if (linha.empty()) continue;
        auto campos = split(linha, ';');
        if (campos.size() < 3) continue;
        int idFicha = stoi(campos[0]);
        string nomeFicha = campos[1];
        int totalEx = stoi(campos[2]);
        Ficha f(idFicha, nomeFicha);
        for (int i = 0; i < totalEx; ++i) {
            if (3 + i >= (int)campos.size()) break;
            int idEx = stoi(campos[3 + i]);
            Exercicio* e = buscarExercicioPorId(idEx);
            if (e) f.adicionarExercicio(e);
            else {
            }
        }
        repositorioFichas.push_back(move(f));
    }
    ifs.close();
}

void carregarHistoricoArquivo() {
    historicoExec.clear();
    ifstream ifs(HISTORICO_FILE);
    if (!ifs) return;
    string linha;
    while (getline(ifs, linha)) {
        linha = trim(linha);
        if (linha.empty()) continue;
        auto campos = split(linha, ';');
        if (campos.size() < 5) continue;
        RegistroHistorico r;
        r.dataHora = campos[0];
        r.idFicha = stoi(campos[1]);
        r.nomeFicha = campos[2];
        r.tempoTotal = stod(campos[3]);
        r.caloriasTotal = stod(campos[4]);
        historicoExec.push_back(r);
    }
    ifs.close();
}

void carregarTudo() {
    carregarExerciciosArquivo();
    carregarFichasArquivo();
    carregarHistoricoArquivo();
}

void salvarTudo() {
    salvarExerciciosArquivo();
    salvarFichasArquivo();
    salvarHistoricoArquivo();
}

void menuGerenciarBancoExercicios();
void menuGerenciarFichas();
void registrarExecucao();
void consultarHistorico();

void cadastrarCardio() {
    string nome;
    int duracao;
    double calMin;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cout << "Nome do Cardio: ";
    getline(cin, nome);
    cout << "Duracao (min): ";
    cin >> duracao;
    cout << "Calorias por minuto: ";
    cin >> calMin;
    repositorioExercicios.emplace_back(new Cardio(nome, duracao, calMin));
    cout << "Cardio cadastrado com sucesso.\n";
}

void cadastrarForca() {
    string nome;
    double carga;
    int series, rep, descanso;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cout << "Nome do exercicio de Forca: ";
    getline(cin, nome);
    cout << "Carga (kg): ";
    cin >> carga;
    cout << "Series: ";
    cin >> series;
    cout << "Repeticoes por serie: ";
    cin >> rep;
    cout << "Tempo de descanso (segundos): ";
    cin >> descanso;
    repositorioExercicios.emplace_back(new Forca(nome, carga, series, rep, descanso));
    cout << "Exercicio de força cadastrado com sucesso.\n";
}

void listarExercicios(bool mostrarInativos=false) {
    cout << "---- Lista de Exercicios ----\n";
    for (auto &p : repositorioExercicios) {
        if (!mostrarInativos && !p->isAtivo()) continue;
        cout << "ID: " << p->getId() << " | Nome: " << p->getNome() << " | Tipo: " << (p->tipo() == 1 ? "Cardio" : "Forca") << "\n";
        p->exibirDetalhes();
        cout << "  Tempo estimado (min): " << fixed << setprecision(2) << p->calculaTempoMinutos()
             << " | Calorias estimadas: " << fixed << setprecision(2) << p->calculaCalorias() << "\n\n";
    }
}

void excluirLogicoExercicio() {
    int id;
    cout << "Digite o ID do exercicio a marcar como Inativo: ";
    cin >> id;
    Exercicio* e = buscarExercicioPorId(id);
    if (!e) {
        cout << "Exercicio nao encontrado.\n";
        return;
    }
    e->setAtivo(false);
    cout << "Exercicio ID " << id << " marcado como Inativo.\n";
}

void menuGerenciarBancoExercicios() {
    while (true) {
        cout << "\n--- Gerenciar Banco de Exercicios ---\n";
        cout << "1. Cadastrar Cardio\n";
        cout << "2. Cadastrar Forca\n";
        cout << "3. Listar (ativos)\n";
        cout << "4. Listar (todos)\n";
        cout << "5. Excluir (logico)\n";
        cout << "0. Voltar\n";
        cout << "Escolha: ";
        int op; cin >> op;
        switch(op) {
            case 1: cadastrarCardio(); break;
            case 2: cadastrarForca(); break;
            case 3: listarExercicios(false); break;
            case 4: listarExercicios(true); break;
            case 5: excluirLogicoExercicio(); break;
            case 0: return;
            default: cout << "Opcao invalida.\n";
        }
    }
}

void criarFicha() {
    string nome;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cout << "Nome da ficha: ";
    getline(cin, nome);
    repositorioFichas.emplace_back(nome);
    cout << "Ficha criada com sucesso.\n";
}

void adicionarExercicioAFicha() {
    if (repositorioFichas.empty()) {
        cout << "Nenhuma ficha cadastrada.\n";
        return;
    }
    listarExercicios(false);
    cout << "Fichas existentes:\n";
    for (auto &f : repositorioFichas) {
        cout << "ID: " << f.getId() << " | Nome: " << f.getNome() << " | Qtd exercicios: " << f.getExercicios().size() << "\n";
    }
    int idFicha, idEx;
    cout << "Digite ID da ficha: ";
    cin >> idFicha;
    Ficha* f = buscarFichaPorId(idFicha);
    if (!f) { cout << "Ficha nao encontrada.\n"; return; }
    cout << "Digite ID do exercicio a adicionar: ";
    cin >> idEx;
    Exercicio* e = buscarExercicioPorId(idEx);
    if (!e) { cout << "Exercicio nao encontrado.\n"; return; }
    // Adiciona ponteiro (sem copiar objeto)
    f->adicionarExercicio(e);
    cout << "Exercicio adicionado na ficha.\n";
}

void visualizarFichas() {
    if (repositorioFichas.empty()) {
        cout << "Nenhuma ficha cadastrada.\n";
        return;
    }
    for (auto &f : repositorioFichas) {
        f.exibirFicha();
        cout << "---------------------\n";
    }
}

void menuGerenciarFichas() {
    while (true) {
        cout << "\n--- Gerenciar Fichas de Treino ---\n";
        cout << "1. Criar Ficha\n";
        cout << "2. Adicionar Exercicio a Ficha\n";
        cout << "3. Visualizar Fichas\n";
        cout << "0. Voltar\n";
        cout << "Escolha: ";
        int op; cin >> op;
        switch(op) {
            case 1: criarFicha(); break;
            case 2: adicionarExercicioAFicha(); break;
            case 3: visualizarFichas(); break;
            case 0: return;
            default: cout << "Opcao invalida.\n";
        }
    }
}

void registrarExecucao() {
    if (repositorioFichas.empty()) {
        cout << "Nenhuma ficha cadastrada.\n";
        return;
    }
    cout << "Fichas:\n";
    for (auto &f : repositorioFichas) {
        cout << "ID: " << f.getId() << " | Nome: " << f.getNome() << "\n";
    }
    int idFicha;
    cout << "Digite ID da ficha que voce realizou: ";
    cin >> idFicha;
    Ficha* f = buscarFichaPorId(idFicha);
    if (!f) { cout << "Ficha nao encontrada.\n"; return; }
    double tempo = f->totalTempoMinutos();
    double calorias = f->totalCalorias();
    RegistroHistorico r;
    r.dataHora = now_datetime_string();
    r.idFicha = f->getId();
    r.nomeFicha = f->getNome();
    r.tempoTotal = tempo;
    r.caloriasTotal = calorias;
    historicoExec.push_back(r);
    cout << "Execucao registrada: " << r.dataHora << " | Tempo (min): " << fixed << setprecision(2) << tempo
         << " | Calorias: " << fixed << setprecision(2) << calorias << "\n";
}

void consultarHistorico() {
    if (historicoExec.empty()) {
        cout << "Historico vazio.\n";
        return;
    }
    cout << "--- Historico de Execucoes ---\n";
    for (auto &r : historicoExec) r.exibir();
}

void menuPrincipal() {
    while (true) {
        cout << "\n=== SGTP - Sistema de Gestao de Treinos Pessoais ===\n";
        cout << "1. Gerenciar Banco de Exercicios\n";
        cout << "2. Gerenciar Fichas de Treino\n";
        cout << "3. Registrar Execucao (Treinar)\n";
        cout << "4. Consultar Historico\n";
        cout << "0. Sair (salvar e encerrar)\n";
        cout << "Escolha: ";
        int op; cin >> op;
        switch(op) {
            case 1: menuGerenciarBancoExercicios(); break;
            case 2: menuGerenciarFichas(); break;
            case 3: registrarExecucao(); break;
            case 4: consultarHistorico(); break;
            case 0:
                cout << "Salvando dados...\n";
                salvarTudo();
                cout << "Dados salvos. Saindo.\n";
                return;
            default: cout << "Opcao invalida.\n";
        }
    }
}

int main() {
    cout << "Carregando dados...\n";
    carregarTudo();
    cout << "Dados carregados. Exercicios: " << repositorioExercicios.size() << " | Fichas: " << repositorioFichas.size() << " | Historico: " << historicoExec.size() << "\n";
    menuPrincipal();
    return 0;
}
