// ===================================
// Main.cpp file generated by OptFrame
// Project HFMVRP
// ===================================

#include <stdlib.h>
#include <math.h>
#include <iostream>

#include "HFMVRP.h"
#include "../OptFrame/Heuristics/GRASP/BasicGRASP.hpp"
#include "../OptFrame/Heuristics/ILS/IteratedLocalSearchLevels.hpp"
#include "../OptFrame/Heuristics/ILS/ILSLPerturbation.hpp"
#include "../OptFrame/Heuristics/LocalSearches/FirstImprovement.hpp"
#include "../OptFrame/Heuristics/LocalSearches/BestImprovement.hpp"
#include "../OptFrame/Heuristics/LocalSearches/RVND.hpp"
#include "../OptFrame/Heuristics/Empty.hpp"
#include "../OptFrame/Heuristics/LocalSearches/VariableNeighborhoodDescentUpdateADS.hpp"
#include "../OptFrame/Heuristics/EvolutionaryAlgorithms/ES.hpp"
#include "../OptFrame/Util/RandGenMersenneTwister.hpp"
#include "../OptFrame/CloneConstructive.hpp"
#include "../OptFrame/NSSeq.hpp"
#include "../OptFrame/Timer.hpp"

using namespace std;

using namespace HFMVRP;

int main(int argc, char **argv)
{
	cout << argv[0] << endl;
	RandGenMersenneTwister rg;
	long seed = time(NULL);
	// seed = 100;
	cout << "Seed = " << seed << endl;
	srand(seed);
	rg.setSeed(seed);

	Timer walltimer;

	//srand(seed);

	if (argc != 7)
	{
		cout << "Parametros incorretos!" << endl;
		cout << "Os parametros esperados sao: nome tempo fo_alvo mutationRate mi batch" << endl;
		exit(1);
	}

	const char* caminho = argv[1];
	double tempo = atof(argv[2]);
	double fo_alvo = atof(argv[3]);
	double mutationRate = atof(argv[4]);
	int mi = atoi(argv[5]);
	int batch = atoi(argv[6]);

	string nome = caminho;

	cout << "Parametros:" << endl;
	cout << "nome=" << nome << endl;
	cout << "tempo=" << tempo << endl;
	cout << "fo_alvo=" << fo_alvo << endl;
	cout << "mutationRate=" << mutationRate << endl;
	cout << "mi=" << mi << endl;
	cout << "batch=" << batch << endl;

	Scanner scanner(new File(nome));

	ProblemInstance* p = new ProblemInstance(scanner);
	HFMVRPEvaluator* eval = new HFMVRPEvaluator(*p);
	HFMVRPADSManager* adsMan = new HFMVRPADSManager(*p);
	double alpha = 0;
	ConstructiveSavingsRandomized is(*p, rg, *adsMan);

	NSSeq<RepHFMVRP, AdsHFMVRP>* nsseq_deltaIterator_delta_2opt = new NSSeqVRP2Opt<int, AdsHFMVRP, DeltaMoveVRP2Opt, ProblemInstance, DeltaNSIteratorVRP2Opt<DeltaMoveVRP2Opt> >(p);
	NSSeq<RepHFMVRP, AdsHFMVRP>* nsseq_deltaIterator_delta_or1 = new NSSeqVRPOrOpt1<int, AdsHFMVRP, DeltaMoveVRPOrOpt1, ProblemInstance, DeltaNSIteratorVRPOrOpt1<DeltaMoveVRPOrOpt1> >(p);
	NSSeq<RepHFMVRP, AdsHFMVRP>* nsseq_deltaIterator_delta_or2 = new NSSeqVRPOrOpt2<int, AdsHFMVRP, DeltaMoveVRPOrOpt2, ProblemInstance, DeltaNSIteratorVRPOrOpt2<DeltaMoveVRPOrOpt2> >(p);
	NSSeq<RepHFMVRP, AdsHFMVRP>* nsseq_deltaIterator_delta_exchange = new NSSeqVRPExchange<int, AdsHFMVRP, DeltaMoveVRPExchange, ProblemInstance, DeltaNSIteratorVRPExchange<DeltaMoveVRPExchange> >(p);
	//NSSeq<RepHFMVRP, AdsHFMVRP>* nsseq_delta_shift10 = new NSSeqVRPShift10<int, AdsHFMVRP, DeltaMoveVRPShift10, ProblemInstance> (p);
	//NSSeq<RepHFMVRP, AdsHFMVRP>* nsseq_delta_swap11 = new NSSeqVRPSwap1_1<int, AdsHFMVRP, DeltaMoveVRPSwap1_1, ProblemInstance> (p);
	NSSeq<RepHFMVRP, AdsHFMVRP>* nsseq_deltaIterator_swap11 = new NSSeqVRPSwap1_1<int, AdsHFMVRP, DeltaMoveVRPSwap1_1, ProblemInstance, DeltaNSIteratorVRPSwap1_1<DeltaMoveVRPSwap1_1> >(p);
	NSSeq<RepHFMVRP, AdsHFMVRP>* nsseq_deltaIterator_shift10 = new NSSeqVRPShift10<int, AdsHFMVRP, DeltaMoveVRPShift10, ProblemInstance, DeltaNSIteratorVRPShift10<DeltaMoveVRPShift10> >(p);

	ILSLPerturbationLPlus2Prob<RepHFMVRP, AdsHFMVRP>* ilsl_pert;

	ilsl_pert = new ILSLPerturbationLPlus2Prob<RepHFMVRP, AdsHFMVRP>(*eval, 100000, *nsseq_deltaIterator_shift10, rg);
	ilsl_pert->add_ns(*nsseq_deltaIterator_swap11);
	ilsl_pert->add_ns(*nsseq_deltaIterator_delta_2opt);
	ilsl_pert->add_ns(*nsseq_deltaIterator_delta_or1);
	ilsl_pert->add_ns(*nsseq_deltaIterator_delta_or2);
	ilsl_pert->add_ns(*nsseq_deltaIterator_delta_exchange);
	int myints[] =
	{ 3, 3, 1, 1, 1, 1 };
	vector<int> priorites(myints, myints + sizeof(myints) / sizeof(int));
	ilsl_pert->changeProb(priorites);

	vector<LocalSearch<RepHFMVRP, AdsHFMVRP>*> vLS;
	BestImprovement<RepHFMVRP, AdsHFMVRP> fiI(*eval, *nsseq_deltaIterator_delta_2opt);
	BestImprovement<RepHFMVRP, AdsHFMVRP> fiII(*eval, *nsseq_deltaIterator_delta_or1);
	BestImprovement<RepHFMVRP, AdsHFMVRP> fiIII(*eval, *nsseq_deltaIterator_delta_or2);
	BestImprovement<RepHFMVRP, AdsHFMVRP> fiIV(*eval, *nsseq_deltaIterator_delta_exchange);
	BestImprovement<RepHFMVRP, AdsHFMVRP> fiV(*eval, *nsseq_deltaIterator_swap11);
	BestImprovement<RepHFMVRP, AdsHFMVRP> fiVI(*eval, *nsseq_deltaIterator_shift10);
	vLS.push_back(&fiI);
	vLS.push_back(&fiII);
	vLS.push_back(&fiIII);
	vLS.push_back(&fiIV);
	vLS.push_back(&fiV);
	vLS.push_back(&fiVI);

	RVND<RepHFMVRP, AdsHFMVRP> newVND(*eval, vLS, rg);
	VariableNeighborhoodDescentUpdateADS<RepHFMVRP, AdsHFMVRP> newVNDUpdateADS(*eval, *adsMan, vLS);
	BasicGRASP<RepHFMVRP, AdsHFMVRP> basicGrasp(*eval, is, newVNDUpdateADS, alpha, 1000);

	EmptyLocalSearch<RepHFMVRP, AdsHFMVRP> emptyLS;
	vector<NSSeq<RepHFMVRP, AdsHFMVRP>*> vNSeq;
	vNSeq.push_back(nsseq_deltaIterator_delta_2opt);
	vNSeq.push_back(nsseq_deltaIterator_delta_or1);
	vNSeq.push_back(nsseq_deltaIterator_delta_or2);
	vNSeq.push_back(nsseq_deltaIterator_delta_exchange);
	vNSeq.push_back(nsseq_deltaIterator_swap11);
	vNSeq.push_back(nsseq_deltaIterator_shift10);

	pair<Solution<RepHFMVRP, AdsHFMVRP>&, Evaluation&>* initialPairGrasp = basicGrasp.search(10, 0);
	CloneConstructive<RepHFMVRP, AdsHFMVRP> cloneSolAsConstructive(initialPairGrasp->first);

	vector<int> vNSeqMaxApplication(vNSeq.size(), 1000);
	int selectionType = 1;
//	double mutationRate = 1;
//	int mi = 5;
	//int batach = 5;
	ES<RepHFMVRP, AdsHFMVRP> es(*eval, cloneSolAsConstructive, vNSeq, vNSeqMaxApplication, emptyLS, selectionType, mutationRate, rg, mi, 6 * mi, 50000, "./MyProjects/Results/esOutput", batch);
	es.setMessageLevel(4);

	pair<Solution<RepHFMVRP, AdsHFMVRP>&, Evaluation&>* initialSol = es.search(180, 0);
	double objFuncES = initialSol->second.getObjFunction();
	cout << "getObjFunction: " << objFuncES << endl;

	size_t found = nome.find_last_of("/\\");
	string instanceName = nome.substr(found + 1);
	FILE* arquivo = fopen("./MyProjects/Results/log.txt", "a");
	if (!arquivo)
	{
		cout << "ERRO: falha ao criar arquivo \"./Results/log.txt\"" << endl;
	}
	else
	{
		fprintf(arquivo, "%s\t%f\t%f\t%d\t%d\t%ld\n", instanceName.c_str(), mutationRate, objFuncES, mi, batch, seed);
		fclose(arquivo);
	}

	/*	// ==================== BEGIN ILS AND GRASP ===========================
	 //pair<Solution<RepHFMVRP, AdsHFMVRP>&, Evaluation&>* initialPair = basicGrasp.search(30, 0);

	 CloneConstructive<RepHFMVRP, AdsHFMVRP> cloneSolAsConstructive(initialSol->first);
	 IteratedLocalSearchLevels<RepHFMVRP, AdsHFMVRP> ilsl(*eval, cloneSolAsConstructive, newVNDUpdateADS, *ilsl_pert, 130, 15);
	 ilsl.setMessageLevel(4);

	 pair<Solution<RepHFMVRP, AdsHFMVRP>&, Evaluation&>* pairILS = ilsl.search(60, 0);
	 Solution<RepHFMVRP, AdsHFMVRP> ilsSOL = pairILS->first;

	 EvaluationHFMVRP value = eval->evaluate(ilsSOL.getR());
	 cout << "getObjFunction: " << value.getObjFunction() << endl;
	 cout << "getInfMeasure: " << value.getInfMeasure() << endl;
	 value.print();

	 eval->exportEXCEL(ilsSOL.getR(), "./MyProjects/Results/teste");*/

	cout << "Program ended successfully" << endl;

	return 0;
}
