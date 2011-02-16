//Rename to mainTSP.cpp to run the example

// ===================================
// Main.cpp file generated by OptFrame
// Project Traveling Salesman Problem
// ===================================

#include <stdlib.h>
#include <math.h>

#include <iostream>

using namespace std;

#include <set>

#include "./OptFrame/OptFrame.hpp"
#include "./OptFrame/Examples/TSP.h"

int main(int argc, char **argv)
{
	srand(time(NULL));
	RandGen rg(time(NULL));

	// Optimal value for berlin52 is 7542

	Scanner scanner(new File("./OptFrame/Examples/TSP/tsplib/berlin52.txt"));

	TSPProblemInstance* p = new TSPProblemInstance(scanner);

	RandomInitialSolutionTSP is(p,rg);

	SolutionTSP& s = is.generateSolution();

	NSEnumSwap ns(p, rg);

	s.print();

	TSPEvaluator eval(p);
	EvaluationTSP* e;

	e = &eval.evaluate(s);

	e->print();
	cout << endl;

	OptFrame<RepTSP, MemTSP> optframe(rg);
	optframe.factory.add_initsol(&is);
	optframe.factory.add_ev(&eval);
	optframe.factory.add_ns(&ns);

	optframe.execute("define is_random initsol 0");
	optframe.execute("define my_eval ev 0");
	optframe.execute("define swap ns 0");

	//optframe.execute();
	optframe.execute("read example.opt");

	cout << "Program ended successfully" << endl;

	return 0;
}
