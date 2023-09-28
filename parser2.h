#pragma once
#include "parser1.h"
double computeFunction(string& functionStr, VectorXd x);
VectorXd equationsResult(map<string, string> equations, VectorXd x);
MatrixXd jacobiansResult(map<string, string> jac, VectorXd x);
VectorXd NRIteration(VectorXd x0, map<string, string> eqt, map<string, string> jac, double tol, int maxIter);
VectorXd HMIteration(VectorXd x0, map<string, string> eqt, map<string, string> jac, double tol, double step);
double exp_(double x);
bool isSpaceOrSemicolon(char c);
bool isAccurate(VectorXd result, double acc);
char* strComponentType(Component* compPtr);
void printComponents(Component* compPtr);
double stripString(char* stringIn);
void printNodes(Node* nodePtr, int compFlag);

//string doubleToString(double number, int precision) {
//    stringstream ss;
//    ss << fixed << setprecision(precision) << number;
//    return ss.str();
//}

bool isAccurate(VectorXd result, double acc) {
    bool jug = true;
    for (int i = 0; i < result.size(); ++i) {
        if (result(i) > acc || -result(i) > acc) {
            jug = false;
        }
    }
    return jug;

}

bool isSpaceOrSemicolon(char c) {
    return c == ' ' || c == ';';
}

double exp_(double x) {
    if (isinf(exp(x)))
        return numeric_limits<double>::max();  // 返回 double 类型的最大值
    else
        return exp(x);
}

double computeFunction(string& functionStr, VectorXd x) {
    for (int i = 0; i < x.size(); i++) {
        size_t pos = 0;
        while ((pos = functionStr.find("X(" + to_string(i + 1) + ")", pos)) != string::npos) {
            functionStr.replace(pos, 4, to_string(x(i)));
            pos += 4;
        }
    }
    typedef exprtk::symbol_table<double> symbol_table_t;
    typedef exprtk::expression<double> expression_t;
    typedef exprtk::parser<double> parser_t;
    symbol_table_t symbol_table;
    symbol_table.add_function("exp_", exp_);
    expression_t expression;
    expression.register_symbol_table(symbol_table);
    parser_t parser;
    parser.compile(functionStr, expression);
    return expression.value();
}

VectorXd equationsResult(map<string, string> equations, VectorXd x) {
    VectorXd f(equations.size());
    int num;
    size_t startPos, endPos;
    string numStr;
    for (auto& i : equations) {
        // 查找数字的起始位置和结束位置
        startPos = i.first.find('(') + 1;
        endPos = i.first.find(')');
        // 提取数字子串
        numStr = i.first.substr(startPos, endPos - startPos);
        num = atoi(numStr.c_str());
        //cout << "f" << num-1<<":" << i.second << endl;
        f(num - 1) = computeFunction(i.second, x);
        //cout << f(num - 1) << endl;
    }

    return f;
}

// 定义计算雅可比矩阵(matrixXd类型的纯数字矩阵)的函数
MatrixXd jacobiansResult(map<string, string> jac, VectorXd x) {
    int ms = sqrt(jac.size()), l, r;
    string ls, rs;
    MatrixXd j(ms, ms);
    size_t startPos, middlePos, endPos;
    for (auto& i : jac) {
        // 查找数字的起始位置和结束位置
        startPos = i.first.find('(') + 1;
        middlePos = i.first.find(',');
        endPos = i.first.find(')');
        // 提取数字子串
        ls = i.first.substr(startPos, middlePos - startPos);
        rs = i.first.substr(middlePos + 1, endPos - middlePos - 1);
        l = atoi(ls.c_str());
        r = atoi(rs.c_str());
        //cout << l << "," << r << ":" << i.second << endl<<endl;
        j(--l, --r) = computeFunction(i.second, x);
        //cout <<l<<","<<r<<":"<< j(l, r) << endl;
    }

    return j;
}
VectorXd NRIteration(VectorXd x0, map<string, string> eqt, map<string, string> jac, double tol, int maxIter) {
    VectorXd x = x0;
    VectorXd F;
    MatrixXd J;
    VectorXd delta_x;
    int count = 1;
    for (; count <= maxIter; ++count) {
        F = equationsResult(eqt, x);
        J = jacobiansResult(jac, x);
        //delta_x = J.lu().solve(-F);
        delta_x = J.colPivHouseholderQr().solve(-F);
        x += delta_x;
        if (isAccurate(delta_x, tol)) {
            cout << "N-R converge success in " << count << " iterations." << endl;
            return x;
        }
    }
    cout << "N-R converge fail in " << count << " iterations." << endl;
    return x;
}
VectorXd HMIteration(VectorXd x0, map<string, string> eqt, map<string, string> jac, double tol, double step) {
    int xs = x0.size();
    VectorXd x = x0;
    VectorXd F;
    MatrixXd J;
    VectorXd delta_x;
    VectorXd a(xs);
    //a << 0, 0, 0, 0, 0, 0, 0, 0;//solution1
    a << 10, 10, 10, 10, 10, 10, 10, 10;//solution3
    double lambda = 0;
    VectorXd sol;
    MatrixXd G(xs, xs);
    for (int i = 0; i < xs; i++) {
        for (int j = 0; j < xs; j++)
            G(i, j) = (i == j ? 1e-3 : 0);
    }
    int count;
    for (; lambda <= 1; ++count) {
        count = 0;
        while (count++<20) {
            F = lambda * equationsResult(eqt, x) + (1 - lambda) * G * (x - a);
            J = lambda * jacobiansResult(jac, x) + (1 - lambda) * G;
            delta_x = J.colPivHouseholderQr().solve(-F);
            x += delta_x;
            if (delta_x.norm() < tol) 
                break;
        }
        cout << "lambda: " << lambda << " NRcount: " << count << endl;
        if (lambda >= 0.99 && lambda <= 1.00) 
            sol = x;
        lambda += step;
    }
    return sol;
}

void TranCc4(double step, double end) {
    double r = 1000;
    double v = 3;
    double c = 1e-5;
    double uc = 0;

    double time = 0;
    while (time <= end) {
        cout << time << "s:" << uc << endl;
        uc = (step * v) / (r * c) + (1 - step / (r * c)) * uc;
        time += step;
    }
}

char* strComponentType(Component* compPtr) {

    char* compTypeName = new char[6];
    switch (compPtr->getType()) {

    case VSource: strcpy(compTypeName, "V"); break;
    case Resistor: strcpy(compTypeName, "R"); break;
    case BJT: strcpy(compTypeName, "T"); break;
    case MOSFET: strcpy(compTypeName, "M"); break;
    case ISource: strcpy(compTypeName, "I"); break;
    case Inductor: strcpy(compTypeName, "ind"); break;
    case Diode: strcpy(compTypeName, "Diode"); break;
    case Capacitor: strcpy(compTypeName, "Cap"); break;
    }

    return compTypeName;
}
double stripString(char* stringIn) {
    char buf[BufLength], buf2[BufLength];
    int a, b;
    strcpy(buf, stringIn);
    for (a = 0; buf[a] != '='; a++) {};
    a++;
    for (b = 0; buf[a] != '\0'; b++, a++)
        buf2[b] = buf[a];
    buf2[b] = '\0';
    return atof(buf2);
};

void printComponents(Component* compPtr) {
    char compTypeName[6];
    cout << endl << "Components: " << endl << endl;
    while (compPtr != NULL) {
        strcpy(compTypeName, strComponentType(compPtr));
        cout << "->" << compTypeName << compPtr->getcompNum();
        compPtr = compPtr->getNext();
    }
    cout << endl;
    return;
}

void printNodes(Node* nodePtr, int compFlag) {

    Connections* conPtr;
    cout << endl << "Nodes: " << endl << endl;
    while (nodePtr != NULL) {
        if (compFlag == 0) { //It is printed just the names of the nodes
            cout << "-> " << nodePtr->getNameNum();
        }
        else if (compFlag == 1) { //It is printed the nodes and the connections
            cout << "-> " << nodePtr->getNameNum() << " {";
            conPtr = nodePtr->getConList();
            while (conPtr->next != NULL) {
                cout << strComponentType(conPtr->comp) << conPtr->comp->getcompNum() << ", ";
                conPtr = conPtr->next;
            }
            cout << strComponentType(conPtr->comp) << conPtr->comp->getcompNum() << '}' << endl;
        }
        else {
            cout << "Invalid value for compFlag. (0) to print just nodes, (1) to print nodes and connections!";
            exit(1);

        }

        nodePtr = nodePtr->getNext();
    }


    return;
}


