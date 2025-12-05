#include <iostream>
#include <fstream>
#include <sstream>
#include <cctype>
#include <cmath>
#include <string>
#include <cstdlib>
#include <cstddef>

using namespace std;

struct Node {
    string data;
    Node* next;
};

struct Stack {
    Node* top;
    void init() { top = NULL; }
    bool isEmpty() { return top == NULL; }
    void push(const string &value) {
        Node* newNode = new Node;
        newNode->data = value;
        newNode->next = top;
        top = newNode;
    }
    string pop() {
        if (isEmpty()) return "";
        Node* tmp = top;
        string val = tmp->data;
        top = tmp->next;
        delete tmp;
        return val;
    }
    string peek() {
        return isEmpty() ? "" : top->data;
    }
};

int precedence(char op) {
    switch (op) {
        case '+': case '-': return 1;
        case '*': case '/': return 2;
        case '^': return 3;
        default: return 0;
    }
}

bool isOperator(char c) {
    return c=='+' || c=='-' || c=='*' || c=='/' || c=='^';
}

string infixToPostfix(const string &infix) {
    Stack stackOp; stackOp.init();
    stringstream output;

    for (size_t i = 0; i < infix.size(); ++i) {
        char c = infix[i];
        if (isspace(c)) continue;

        if (isdigit(c)) {
            // manejar números multi-dígito
            while (i < infix.size() && (isdigit(infix[i]) || infix[i]=='.')) {
                output << infix[i];
                ++i;
            }
            output << ' ';
            --i;
        }
        else if (c == '(') {
            string s(1, c);
            stackOp.push(s);
        }
        else if (c == ')') {
            while (!stackOp.isEmpty() && stackOp.peek() != "(") {
                output << stackOp.pop() << ' ';
            }
            // eliminar '(' si existe
            if (!stackOp.isEmpty()) stackOp.pop();
        }
        else if (isOperator(c)) {
            while (!stackOp.isEmpty() && precedence(stackOp.peek()[0]) >= precedence(c)) {
                output << stackOp.pop() << ' ';
            }
            stackOp.push(string(1, c));
        }
    }

    while (!stackOp.isEmpty()) {
        output << stackOp.pop() << ' ';
    }

    return output.str();
}

int strToInt(const string &s) {
    return atoi(s.c_str());
}
string intToStr(int x) {
    stringstream ss; ss << x; return ss.str();
}

int evaluatePostfix(const string &postfix) {
    Stack stackVal; stackVal.init();
    stringstream ss(postfix);
    string token;

    while (ss >> token) {
        if (isdigit(token[0])) {
            stackVal.push(token);
        } else if (isOperator(token[0])) {
            string sb = stackVal.pop();
            string sa = stackVal.pop();
            int b = strToInt(sb);
            int a = strToInt(sa);
            int res = 0;
            switch (token[0]) {
                case '+': res = a + b; break;
                case '-': res = a - b; break;
                case '*': res = a * b; break;
                case '/': res = a / b; break;
                case '^': res = (int) pow(a, b); break;
            }
            stackVal.push(intToStr(res));
        }
    }

    return strToInt(stackVal.pop());
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        cerr << "Uso: " << argv[0] << " <archivo_entrada>" << endl;
        return 1;
    }

    ifstream infile(argv[1]);
    if (!infile.is_open()) {
        cerr << "No se pudo abrir el archivo de entrada." << endl;
        return 1;
    }

    ofstream outfile("exp_postfijas.txt");
    if (!outfile.is_open()) {
        cerr << "No se pudo crear el archivo de salida." << endl;
        return 1;
    }

    string line;
    while (getline(infile, line, ';')) {
        // limpiar espacios extremos
        bool allspace = true;
        for (size_t i=0;i<line.size();++i) if (!isspace(line[i])) { allspace=false; break; }
        if (line.empty() || allspace) continue;

        string postfix = infixToPostfix(line);
        int result = evaluatePostfix(postfix);
        outfile << "Expresión: " << postfix << "; Resultado: " << result << endl;
    }

    infile.close();
    outfile.close();
    cout << "Archivo exp_postfijas.txt generado exitosamente." << endl;
    return 0;
}

