#include "parser2.h"

int main(){
    ifstream inFile("D:/case1.txt");
    char buf[BufLength], buf1[BufLength], buf2[BufLength], buf3[BufLength], nameBuf[NameLength],
        * bufPtr, * charPtr1, * charPtr2;
    int intBuf1, intBuf2, intBuf3, intBuf4, eqNum = NA, specPrintJacMNA = 0;
    double douBuf1, douBuf2, douBuf3, douBuf4=300;
    NodeHead nodeList;
    CompHead compList;
    ModelHead modelList;
    CompType typeBuf;
    Component* compPtr, * compPtr1, * compPtr2;
    Node* nodePtr, * nodePtr1, * nodePtr2;
    Model* modelPtr;
    TranType TtypeBuf;
    

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
    cout << endl;
    // output circuit information
    ofstream outFile("D:/case1out.txt");
    if (!outFile.is_open()) {
        // 向文件中写入数据
        cout << "Error opening file!" << endl;
        outFile.close();
    }
    EquaType eqType = Modified;
    int datum = 0;
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

    ifstream inFile1("D:/case1out.txt");
    string line;
    if (!inFile1) {
        cerr << "Failed to open file " << endl;
        return 1;
    }
    // 存储变量名及其值
    map<string, string> variables; 
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
                variables[variableName] = "1e+300";
            else
                variables[variableName] = valueStr;
        }
    }

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
                    equationStr.replace(pos, i.first.length(), i.second);
                    pos += i.second.length();
                }
            }
            size_t pos1 = 0;
            while ((pos1 = equationStr.find("exp", pos1)) != string::npos) {
                equationStr.replace(pos1, 3, "exp_");
                pos1 += 4;
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
                    elementStr.replace(pos, i.first.length(), i.second);
                    pos += i.second.length();
                }
            }
            size_t pos1 = 0;
            while ((pos1 = elementStr.find("exp", pos1)) != string::npos) {
                elementStr.replace(pos1, 3, "exp_");
                pos1 += 4;
            }
            jacobians[elementName] = elementStr;
        }
    }
    //while (getline(inFile1, line)) {
    //    if (line.find("Circuit Equations:") != string::npos)
    //        break;
    //    // 去除变量名和值字符串中的空格与分号
    //    line.erase(remove_if(line.begin(), line.end(), isSpaceOrSemicolon), line.end());
    //    // 解析每一行，假设格式为"variableName = value"
    //    size_t delimiterPos = line.find("=");
    //    if (delimiterPos != string::npos) {
    //        string variableName = line.substr(0, delimiterPos);
    //        string valueStr = line.substr(delimiterPos + 1);
    //        // 处理无穷
    //        if (valueStr == "inf")
    //            variables[variableName] = numeric_limits<double>::max();
    //        else {
    //            // 将值的字符串转换为double类型
    //            double value;
    //            istringstream iss(valueStr);
    //            if (!(iss >> value)) {
    //                cerr << "Failed to parse value: " << valueStr << endl;
    //                continue;  // 跳过无法解析行
    //            }
    //            variables[variableName] = value;
    //        }
    //    }
    //} 
    /*int sizeGuess;
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
    }*/
    
    //map<string, string> equations;
    //while (getline(inFile1, line)) {
    //    if (line.find("Jacobians:") != string::npos)
    //        break;
    //    line.erase(remove_if(line.begin(), line.end(), isSpaceOrSemicolon), line.end());
    //    size_t delimiterPos = line.find("=");
    //    if (delimiterPos != string::npos) {
    //        string equationName = line.substr(0, delimiterPos);
    //        string equationStr = line.substr(delimiterPos + 1);
    //        for (auto& i : variables) {
    //            size_t pos = 0;
    //            /*ostringstream oss;
    //            oss.str("");
    //            oss << i.second;
    //            string ts = oss.str();
    //            cout << "我" << ts << endl;*/
    //            while ((pos = equationStr.find(i.first, pos)) != string::npos) {
    //                equationStr.replace(pos, i.first.length(), doubleToString(i.second,30));
    //                pos += to_string(i.second).length();
    //            }
    //        }
    //        size_t pos1 = 0;
    //        while ((pos1 = equationStr.find("exp", pos1)) != string::npos) {
    //            equationStr.replace(pos1, 3, "exp_");
    //            pos1 += 4;
    //        }
    //        equations[equationName] = equationStr;
    //        
    //    }
    //}
    /*cout << equations["F(1)"] << endl;
    cout << equations["F(2)"] << endl;
    cout << equations["F(3)"] << endl;
    cout << equations["F(4)"] << endl;
    cout << equations["F(5)"] << endl;
    cout << equations["F(6)"] << endl;
    cout << equations["F(7)"] << endl;
    cout << equations["F(8)"] << endl;*/
    /*map<string, string> jacobians;
    while (getline(inFile1, line)) {
        line.erase(remove_if(line.begin(), line.end(), isSpaceOrSemicolon), line.end());
        size_t delimiterPos = line.find("=");
        if (delimiterPos != string::npos) {
            string elementName = line.substr(0, delimiterPos);
            string elementStr = line.substr(delimiterPos + 1);
            for (auto& i : variables) {
                size_t pos = 0;
                while ((pos = elementStr.find(i.first, pos)) != string::npos) {
                    elementStr.replace(pos, i.first.length(), doubleToString(i.second, 30));
                    pos += to_string(i.second).length();
                }
            }
            size_t pos1 = 0;
            while ((pos1 = elementStr.find("exp", pos1)) != string::npos) {
                elementStr.replace(pos1, 3, "exp_");
                pos1 += 4;
            }
            jacobians[elementName] = elementStr;
        }
    }*/
    inFile1.close();

    VectorXd init(8);
    //init << 0, 0, 0, 0, 0, 0, 0, 0;//nr不行homo可以
    //init << 100, 100, 100, 100, 100, 100, 100, 100;//nr不行homo可以
    init << 0.7 ,0.6 ,10 ,0.7 ,1.5 ,10, 0, 0;//nr和homo都可以
    VectorXd solution1 = NRIteration(init, equations, jacobians, 1e-3, 100);
    VectorXd solution2 = HMIteration(init, equations, jacobians, 1e-3, 0.01);
    cout << "Solution N-R:" << endl << solution1 << endl;
    cout << "Solution Homotopy:" << endl << solution2 << endl;
    //cout << "Solution TranCc4:" << endl;
    //TranCc4(0.001, 0.04);
    return 0;
}
