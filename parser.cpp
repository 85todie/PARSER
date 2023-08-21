// parser.cpp: 定义应用程序的入口点。
//

#include "parser.h"

double stripString(char* stringIn);
void printComponents(Component* compPtr);
void printNodes(Node* nodePtr, int compFlag);
char* strComponentType(Component* compPtr);
VectorXd circuitEquations(const VectorXd& x);
MatrixXd jacobianMatrix(const VectorXd& x);
VectorXd newtonRaphsonIteration(const VectorXd& x0, int maxIter, double tol);
bool isSpaceOrSemicolon(char c) {
    return c == ' ' || c == ';';
}
double computeFunction(string& functionStr) {
    typedef exprtk::symbol_table<double> symbol_table_t;
    typedef exprtk::expression<double> expression_t;
    typedef exprtk::parser<double> parser_t;

    symbol_table_t symbol_table;
    symbol_table.add_constants();

    expression_t expression;
    expression.register_symbol_table(symbol_table);

    parser_t parser;
    parser.compile(functionStr, expression);

    return expression.value();
}
VectorXd equationsResult(map<string, string> equations) {
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
        f(num-1) = computeFunction(i.second);
        //cout << f(num - 1) << endl;
    }

    return f;
}

// 定义计算雅可比矩阵(matrixXd类型的纯数字矩阵)的函数
MatrixXd jacobiansResult(map<string, string> jac) {
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
        rs = i.first.substr(startPos, endPos - middlePos - 1);
        l = atoi(ls.c_str());
        r = atoi(rs.c_str());
        j(--l, --r) = computeFunction(i.second);
    }
    return j;
}
VectorXd NRIteration(VectorXd x0, map<string, string> eqt, map<string, string> jac, int maxIter, double tol) {
    VectorXd x = x0;
    for (int i = 0; i < maxIter; ++i) {
        VectorXd F = equationsResult(eqt);
        MatrixXd J = jacobiansResult(jac);

        VectorXd delta_x = J.colPivHouseholderQr().solve(-F);
        x += delta_x;

        if (delta_x.norm() < tol) {
            cout << "Converged in " << i + 1 << " iterations." << endl;
            return x;
        }
    }
    cout << "Did not converge within the maximum number of iterations." << endl;
    return x;
}
int main()
{
    ifstream inFile("C:/Users/85todie/source/repos/parser/Netlist.txt");
    char buf[BufLength], buf1[BufLength], buf2[BufLength], buf3[BufLength], nameBuf[NameLength],
        * bufPtr, * charPtr1, * charPtr2;
    int intBuf1, intBuf2, intBuf3, intBuf4, datum = NA, eqNum = NA, specPrintJacMNA = 0;
    double douBuf1, douBuf2, douBuf3, douBuf4 = NULL;
    NodeHead nodeList;
    CompHead compList;
    ModelHead modelList;
    CompType typeBuf;
    Component* compPtr, * compPtr1, * compPtr2;
    Node* nodePtr, * nodePtr1, * nodePtr2;
    Model* modelPtr;
    TranType TtypeBuf;
    EquaType eqType = Modified;

    if (!inFile.is_open()) {
        std::cout << "Failed to open file." << std::endl;
        return 1;
    }

    inFile.getline(buf, BufLength);
    inFile.getline(buf, BufLength);

    while (inFile.good()) {
        if ((buf == NULL) || (*buf == '\0')) {
            inFile.getline(buf, BufLength);
            continue;
        }
        strcpy(buf1, buf);
        if (!strcmp(strtok(buf1, " "), ".model")) {
            strcpy(buf2, strtok(NULL, " "));
            charPtr1 = strtok(NULL, " ");
            if (!strcmp(charPtr1, "PNP"))
                TtypeBuf = PNP;
            else if (!strcmp(charPtr1, "NPN"))
                TtypeBuf = NPN;
            else if (!strcmp(charPtr1, "NMOS"))
                TtypeBuf = NMOS;
            else if (!strcmp(charPtr1, "PMOS"))
                TtypeBuf = PMOS;
            charPtr1 = strtok(NULL, " ");
            while (charPtr1 != NULL) {
                strcpy(buf3, "");
                if ((charPtr1[0] == 'I') && (charPtr1[1] == 'S') && (charPtr1[2] == '=')) {
                    douBuf1 = stripString(charPtr1);
                }
                if ((charPtr1[0] == 'B') && (charPtr1[1] == 'F') && (charPtr1[2] == '=')) {
                    douBuf2 = stripString(charPtr1);
                }
                if ((charPtr1[0] == 'B') && (charPtr1[1] == 'R') && (charPtr1[2] == '=')) {
                    douBuf3 = stripString(charPtr1);
                }
                if ((charPtr1[0] == 'T') && (charPtr1[1] == 'E') && (charPtr1[4] == '=')) {
                    douBuf4 = stripString(charPtr1);
                }
                charPtr1 = strtok(NULL, " ");
            }
            modelPtr = new Model(buf2, TtypeBuf, douBuf1, douBuf2, douBuf3, douBuf4);
            modelList.addModel(modelPtr);
        }
        if (isalpha(*buf)) {

            //  EDIT THIS SECTION IF NEW COMPONENTS ARE ADDED!!!
            //  we could do some rearranging in this section to catch each type in order.
            switch (*buf) {
            case 'v':
            case 'V':
                typeBuf = VSource;
                strcpy(nameBuf, strtok(buf, " "));
                intBuf1 = atoi(strtok(NULL, " "));
                intBuf2 = atoi(strtok(NULL, " "));
                douBuf1 = atof(strtok(NULL, " "));
                compPtr = new Component(typeBuf, douBuf1, NA, intBuf1, intBuf2, NA, NA, NULL, nameBuf);
                compList.addComp(compPtr);
                break;
            case 'i':
            case 'I':
                cout << "I" << endl;
                typeBuf = ISource;
                strcpy(nameBuf, strtok(buf, " "));
                intBuf1 = atoi(strtok(NULL, " "));
                intBuf2 = atoi(strtok(NULL, " "));
                douBuf1 = atof(strtok(NULL, " "));
                compPtr = new Component(typeBuf, douBuf1, NA, intBuf1, intBuf2, NA, NA, NULL, nameBuf);
                compList.addComp(compPtr);
                break;
            case 'q':
            case 'Q':
                typeBuf = BJT;
                strcpy(nameBuf, strtok(buf, " "));
                intBuf1 = atoi(strtok(NULL, " "));
                intBuf2 = atoi(strtok(NULL, " "));
                intBuf3 = atoi(strtok(NULL, " "));
                compPtr = new Component(typeBuf, NA, NA, intBuf1, intBuf2, intBuf3, NA,
                    modelList.getModel(strtok(NULL, " ")), nameBuf);
                compList.addComp(compPtr);
                break;
            case 'm':
            case 'M':
                typeBuf = MOSFET;
                strcpy(nameBuf, strtok(buf, " "));
                intBuf1 = atoi(strtok(NULL, " "));
                intBuf2 = atoi(strtok(NULL, " "));
                intBuf3 = atoi(strtok(NULL, " "));
                intBuf4 = atoi(strtok(NULL, " "));
                compPtr = new Component(typeBuf, NA, NA, intBuf1, intBuf2, intBuf3, intBuf4,
                    modelList.getModel(strtok(NULL, " ")), nameBuf);
                compList.addComp(compPtr);
                break;
            case 'r':
            case 'R':
                typeBuf = Resistor;
                strcpy(nameBuf, strtok(buf, " "));
                intBuf1 = atoi(strtok(NULL, " "));
                intBuf2 = atoi(strtok(NULL, " "));
                douBuf1 = atof(strtok(NULL, " "));
                compPtr = new Component(typeBuf, douBuf1, NA, intBuf1, intBuf2, NA, NA, NULL, nameBuf);
                compList.addComp(compPtr);
                break;
            case 'd':
            case 'D':
                typeBuf = Diode;
                strcpy(nameBuf, strtok(buf, " "));
                intBuf1 = atoi(strtok(NULL, " "));
                intBuf2 = atoi(strtok(NULL, " "));
                charPtr1 = strtok(NULL, " ");
                while (charPtr1 != NULL) {
                    if ((charPtr1[0] == 'I') && (charPtr1[1] == 'S') && (charPtr1[2] == '=')) {
                        douBuf1 = stripString(charPtr1);
                    }
                    if ((charPtr1[0] == 'T') && (charPtr1[1] == 'E') && (charPtr1[4] == '=')) {
                        douBuf2 = stripString(charPtr1);
                    }
                    charPtr1 = strtok(NULL, " ");
                }
                compPtr = new Component(typeBuf, douBuf1, douBuf2, intBuf1, intBuf2, NA, NA, NULL, nameBuf);
                compList.addComp(compPtr);
                break;
            case 'c':
            case 'C':
                typeBuf = Capacitor;
                strcpy(nameBuf, strtok(buf, " "));
                intBuf1 = atoi(strtok(NULL, " "));
                intBuf2 = atoi(strtok(NULL, " "));
                douBuf1 = atof(strtok(NULL, " "));
                compPtr = new Component(typeBuf, douBuf1, NA, intBuf1, intBuf2, NA, NA, NULL, nameBuf);
                compList.addComp(compPtr);
                break;
            case 'l':
            case 'L':
                typeBuf = Inductor;
                strcpy(nameBuf, strtok(buf, " "));
                intBuf1 = atoi(strtok(NULL, " "));
                intBuf2 = atoi(strtok(NULL, " "));
                douBuf1 = atof(strtok(NULL, " "));
                compPtr = new Component(typeBuf, douBuf1, NA, intBuf1, intBuf2, NA, NA, NULL, nameBuf);
                compList.addComp(compPtr);
                break;
            };
        }
        inFile.getline(buf, BufLength);
    }
    inFile.close();

    compPtr1 = compList.getComp(0);
    while (compPtr1 != NULL) {
        for (int b = 0; b < 3; b++) {
            if ((!compPtr1->isCon(b)) && (compPtr1->getConVal(b) != NA)) {
                intBuf1 = compPtr1->getConVal(b); // ~> getting the connector number as in the netlist file
                nodePtr1 = nodeList.addNode();
                nodePtr1->setNameNum(intBuf1);
                compPtr1->connect(b, nodePtr1);
                nodePtr1->connect(b, compPtr1);

                // now search and connect all other appropriate connectors to this node.
                // error checking should be added to prevent duplicated, or skipped connectors.
                compPtr2 = compPtr1->getNext();
                while (compPtr2 != NULL) {
                    for (int c = 0; c < 3; c++) { //~> verifying which one of the others connectors (of components) are connected to the node above
                        if (compPtr2->getConVal(c) == intBuf1) { //~> if next component in the list of components has a connector with the same name (conNum) of the connector above, connect it to the same node.
                            compPtr2->connect(c, nodePtr1);
                            nodePtr1->connect(c, compPtr2);
                            break;                                    //~> As a component can only have one connector with the same name (connected in the same node), don't search the others and go out of the 'for' loop
                        }
                    }
                    compPtr2 = compPtr2->getNext();
                }
            }
        }
        compPtr1 = compPtr1->getNext();
    }

    datum = 0;

    // Loop to find lastnode
    nodePtr = nodeList.getNode(0); //~> getting the pointer to the first node, pointed by 'headNode'
    int lastnode = nodePtr->getNameNum();
    while (nodePtr != NULL) {
        lastnode = (nodePtr->getNameNum() > lastnode) ? nodePtr->getNameNum() : lastnode;
        nodePtr = nodePtr->getNext();
    }

    //~> Checking the component list
//~> Comment this part to omit
    compPtr = compList.getComp(0);
    printComponents(compPtr);
    nodePtr = nodeList.getNode(0);
    printNodes(nodePtr, 1);

    // output circuit information
    ofstream outFile("D:/parserout.txt");
    if (!outFile.is_open()) {
        // 向文件中写入数据
        cout << "Error opening file!" << endl;
        outFile.close();
    }

    outFile << "%Equation Type:     ";
    if (eqType == Nodal)
        outFile << "NODAL" << endl;
    else if (eqType == Modified)
        outFile << "MODIFIED NODAL" << endl;
    outFile << "%Datum Node:        " << datum << endl;

    // create value table
    outFile << endl
        << "%****************************************************************" << endl;
    outFile << "%                      Component Values:" << endl;
    compPtr = compList.getComp(0);
    while (compPtr != NULL) {
        compPtr->printVal(outFile);
        compPtr = compPtr->getNext();
    }
    outFile << endl
        << "%****************************************************************" << endl;


    // go down the nodal list and have components announce themselves
    outFile << endl << "%                      Circuit Equations: " << endl;
    nodePtr = nodeList.getNode(0);
    while (nodePtr != NULL) {
        if (nodePtr->getNameNum() != datum) {
            nodePtr->printNodal(outFile, datum, lastnode);
        }
        nodePtr = nodePtr->getNext();
    }

    //go down the component list and give equations for all sources
    compPtr = compList.getComp(0);
    while (compPtr != NULL) {
        compPtr->specialPrint(outFile, datum);
        compPtr = compPtr->getNext();
    }

    //~> go down the component list and give supernode equations for all float sources (Nodal Analysis)
    if (eqType != Modified) {
        compPtr = compList.getComp(0);
        while (compPtr != NULL) {
            compPtr->printSuperNode(outFile, datum, lastnode);
            compPtr = compPtr->getNext();
        }
    }

    // go down the node list and give additional MNA equations
    if (eqType == Modified) {
        nodePtr = nodeList.getNode(0);
        while (nodePtr != NULL) {
            if (nodePtr->getNameNum() != datum)
                nodePtr->printMNA(outFile, datum, lastnode);
            nodePtr = nodePtr->getNext();
        }
    }

    outFile << endl
        << "%********************************************************************" << endl;
    outFile << endl << "%                      Jacobians: " << endl;
    nodePtr1 = nodeList.getNode(0);
    while (nodePtr1 != NULL) {   //~> this loop handles the nodes not connected to a Vsource and those ones that are not the 'datum' node
        if (nodePtr1->getNameNum() != datum) {
            nodePtr2 = nodeList.getNode(0);
            while (nodePtr2 != NULL) {
                if (nodePtr2->getNameNum() != datum) {
                    nodePtr1->printJac(outFile, datum, nodePtr2, lastnode, eqType);
                }
                nodePtr2 = nodePtr2->getNext();
            }
        }
        nodePtr1 = nodePtr1->getNext();
    }

    // go down the component list and give equations for all sources
    compPtr = compList.getComp(0);
    while (compPtr != NULL) {
        nodePtr2 = nodeList.getNode(0);
        compPtr2 = compList.getComp(0);
        while (nodePtr2 != NULL) {
            if (nodePtr2->getNameNum() != datum) {
                compPtr->specialPrintJac(outFile, datum, nodePtr2/**/, lastnode, eqType, compPtr2, &specPrintJacMNA /**/); // ~> specPrintJacMNA is used to verify if the jacobians w.r.t. the Modified equations was already printed to print only once.
            }
            nodePtr2 = nodePtr2->getNext();
        }
        specPrintJacMNA = 0;
        compPtr = compPtr->getNext();
    }

    // print the Jacobians for the additional MNA equations
    if (eqType == Modified) {
        nodePtr1 = nodeList.getNode(0);
        while (nodePtr1 != NULL) {
            if (nodePtr1->getNameNum() != datum) {
                nodePtr2 = nodeList.getNode(0);
                while (nodePtr2 != NULL) {
                    if (nodePtr2->getNameNum() != datum)
                        nodePtr1->printJacMNA(outFile, datum, nodePtr2, lastnode);
                    nodePtr2 = nodePtr2->getNext();
                }
            }
            nodePtr1 = nodePtr1->getNext();
        }
    }
    outFile.close();

    ifstream inFile1("D:/parserout1.txt");
    string line;
    if (!inFile1) {
        cerr << "Failed to open file " << endl;
        return 1;
    }
    // 存储变量名及其对应的值
    map<string, double> variables; 
    while (getline(inFile1, line)) {
        if (line.find("Circuit Equations:") != string::npos)
            break;
        // 去除变量名和值字符串中的空格与分号
        line.erase(remove_if(line.begin(), line.end(), isSpaceOrSemicolon), line.end());
        // 解析每一行，假设格式为"variableName = value"
        size_t delimiterPos = line.find("=");
        if (delimiterPos != string::npos) {
            string variableName = line.substr(0, delimiterPos);
            string valueStr = line.substr(delimiterPos + 1);
            // 处理无穷
            if (valueStr == "inf")
                variables[variableName] = numeric_limits<double>::max();
            else if (valueStr == "-inf")
                variables[variableName] = -numeric_limits<double>::max();//numeric_limits<double>::infinity()
            else {
                // 将值的字符串转换为double类型
                double value;
                istringstream iss(valueStr);
                if (!(iss >> value)) {
                    cerr << "Failed to parse value: " << valueStr << endl;
                    continue;  // 跳过无法解析行
                }
                // 存储变量名及其对应的值
                variables[variableName] = value;
            }

        }
    }
    int sizeGuess;
    string tempGK;
    int count=0;
    double tempGV;
    cout << "input guessSize:" << endl;
    cin >> sizeGuess;
    VectorXd init(sizeGuess);
    cout << "input guessName and guessValue:" << endl;
    for (int i = 0; i < sizeGuess; ++i) {
        cin >> tempGK;
        cin >> tempGV;
        variables[tempGK] = tempGV;
        init(count++) = tempGV;
    }
    /*for ( auto& pair : variables) {
        cout << "Key: " << pair.first << ", Value: " << pair.second << endl;
    }*/
    map<string, string> equations;
    while (getline(inFile1, line)) {
        if (line.find("Jacobians:") != string::npos)
            break;
        line.erase(remove_if(line.begin(), line.end(), isSpaceOrSemicolon), line.end());
        size_t delimiterPos = line.find("=");
        if (delimiterPos != string::npos) {
            string equationName = line.substr(0, delimiterPos);
            string equationStr = line.substr(delimiterPos + 1);
            for (auto& i : variables) {
                size_t pos = 0;
                while ((pos = equationStr.find(i.first, pos)) != string::npos) {
                    equationStr.replace(pos, i.first.length(), to_string(i.second));
                    pos += to_string(i.second).length();
                }
            }
            equations[equationName] = equationStr;

        }
    }
    map<string, string> jacobians;
    while (getline(inFile1, line)) {
        line.erase(remove_if(line.begin(), line.end(), isSpaceOrSemicolon), line.end());
        size_t delimiterPos = line.find("=");
        if (delimiterPos != string::npos) {
            string elementName = line.substr(0, delimiterPos);
            string elementStr = line.substr(delimiterPos + 1);
            for (auto& i : variables) {
                size_t pos = 0;
                while ((pos = elementStr.find(i.first, pos)) != string::npos) {
                    elementStr.replace(pos, i.first.length(), to_string(i.second));
                    pos += to_string(i.second).length();
                }
            }
            jacobians[elementName] = elementStr;

        }
    }
    inFile1.close();

    VectorXd solution = NRIteration(init,equations,jacobians, 100, 1e-6);
    cout << "Solution:" << endl << solution << endl;

    return 0;
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

VectorXd circuitEquations(const VectorXd& x) {
    double x1 = x(0);
    double x2 = x(1);

    VectorXd f(2);
    f(0) = x1 + x2 * x2 - 4;
    f(1) = exp(x1) - x2 - 1;

    return f;
}

// 定义计算雅可比矩阵的函数
MatrixXd jacobianMatrix(const VectorXd& x) {
    double x1 = x(0);
    double x2 = x(1);

    MatrixXd J(2, 2);
    J(0, 0) = 1;
    J(0, 1) = 2 * x2;
    J(1, 0) = exp(x1);
    J(1, 1) = -1;

    return J;
}

// 定义 N-R 迭代函数
VectorXd newtonRaphsonIteration(const VectorXd& x0, int maxIter, double tol) {
    VectorXd x = x0;

    for (int i = 0; i < maxIter; ++i) {
        VectorXd f = circuitEquations(x);
        MatrixXd J = jacobianMatrix(x);

        VectorXd delta_x = J.colPivHouseholderQr().solve(-f);
        x += delta_x;

        if (delta_x.norm() < tol) {
            std::cout << "Converged in " << i + 1 << " iterations." << std::endl;
            return x;
        }
    }

    std::cout << "Did not converge within the maximum number of iterations." << std::endl;
    return x;
}