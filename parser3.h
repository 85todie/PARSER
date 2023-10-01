#pragma once
#include "parser2.h"
void tranAnalysis(map<string, string> eqtDC, map<string, string> jacDC, VectorXd x0_DC,
	map<string, string> eqtTran, map<string, string> jacTran,
	CompHead compList, int datum,double tStep, double tStop) {

	cout << "DC Initialize:" << endl;
	int xdcs = x0_DC.size(), cs = compList.getCount(Capacitor), is = compList.getCount(Inductor);
	double lv[30];
	VectorXd x0_Tran=HMIteration(eqtDC, jacDC, x0_DC,1e-3, 0.01);
	int con0, con1;
	for (int i = 0; i < cs; i++) {
		con0 = compList.getCompCon('c', i, 0);
		con1 = compList.getCompCon('c', i, 1);
		cout << "con0:" << con0 <<"con1:"<<con1<< endl;
		lv[i] = 0;
		if (con0 != datum)
			lv[i] += x0_DC(con0-1);
		if (con1 != datum)
			lv[i] -= x0_DC(con1-1);
	}

	cout << "Tran Start:" << endl;
	for (double i = tStep; i <= tStop; i += tStep) {
		cout << "------ Time: " << i << " ------" << endl;
		x0_Tran = HMIteration(eqtTran, jacTran, x0_Tran, 1e-3, 0.01,tStep,lv);
		for (int j = 0; j < cs;j++) {
			con0 = compList.getCompCon('c', j, 0);
			con1 = compList.getCompCon('c', j, 1);
			cout << "con0:" << con0 << "con1:" << con1 << endl;
			lv[j] = 0;
			if (con0 != datum)
				lv[j] += x0_Tran(con0 - 1);
			if (con1 != datum)
				lv[j] -= x0_Tran(con1 - 1);
		}
	}
 }