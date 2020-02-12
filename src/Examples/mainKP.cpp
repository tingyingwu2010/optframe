// ===================================
// Main.cpp file generated by OptFrame
// Project KnapsackProblem
// ===================================

#include <iostream>
#include <math.h>
#include <stdlib.h>

#include "../OptFrame/Heuristics/EvolutionaryAlgorithms/BRKGA.hpp"
#include "../OptFrame/Heuristics/LocalSearches/BestImprovement.hpp"
#include "../OptFrame/Heuristics/LocalSearches/FirstImprovement.hpp"
#include "../OptFrame/Heuristics/LocalSearches/HillClimbing.hpp"
#include "../OptFrame/Heuristics/LocalSearches/RandomDescentMethod.hpp"
#include "../OptFrame/Heuristics/SA/BasicSimulatedAnnealing.hpp"
#include "../OptFrame/Util/CheckCommand.hpp"
#include "../OptFrame/Util/RandGenMersenneTwister.hpp"
#include "../OptFrame/Util/printable.h"
#include "KP.h"
using namespace std;
using namespace optframe;
using namespace KP;

int
main(int argc, char** argv)
{
   RandGenMersenneTwister rg;                   // declara um bom gerador de números aleatórios
   Scanner scanner(new File("instance-4.txt")); // carrega arquivo no disco
   ProblemInstance p(scanner);                  // cria um problema-teste usando o arquivo carregado
   MyEvaluator ev(p);
   ConstructiveRandom c1(p);
   NSSeqBitFlip ns1(p, rg);
   cout << "will generate solution" << endl;
   SolutionKP s = *c1.generateSolution(10); // timelimit (10???)
   s.print();
   Evaluation<> e = ev.evaluate(s);
   e.print();
   cout << "GUD" << endl;

   CheckCommand<RepKP, MY_ADS, SolutionKP> check; // cria o módulo de testes (opcional)
   Evaluator<SolutionKP>& ev1 = ev;
   check.add(ev1);             // carrega a avaliação para testes
   check.add(c1);             // carrega o construtivo para testes
   check.add(ns1);            // carrega a vizinhança para testes
   check.run(100, 10);        // executa testes com 10 iterações

   NSSeq<SolutionKP>* nsseq_bit = &ns1;

   BasicSimulatedAnnealing<SolutionKP> sa(ev, c1, *nsseq_bit, 0.98, 100, 900.0, rg);
   SOSC sosc; // stop criteria
   pair<SolutionKP, Evaluation<>> r = *sa.search(sosc);
   r.first.print();
   r.second.print();

   BestImprovement<SolutionKP> bi(ev, ns1);
   FirstImprovement<SolutionKP> fi(ev, ns1);
   HillClimbing<SolutionKP> sd(ev, bi);
   HillClimbing<SolutionKP> pm(ev, fi);
   RandomDescentMethod<SolutionKP> rdm(ev, ns1, 10);
   //
   pair<SolutionKP, Evaluation<>> se(s, e);
   sd.lsearch(se, sosc).second.print();  // executa e imprime HC + BI
   pm.lsearch(se, sosc).second.print();  // executa e imprime HC + FI
   rdm.lsearch(se, sosc).second.print(); // executa e imprime RDM com 10 iterações

   EvaluatorSubsetRandomKeys<SolutionKP> eprk(ev1, 0, p.N - 1);
   BRKGA<RepKP, SolutionKP> brkga(eprk, p.N, 1000, 30, 0.4, 0.3, 0.6);

   //pair<CopySolution<random_keys>, Evaluation<>>* r2 = brkga.search(sosc);
   pair<SolutionKP, Evaluation<>> r2 = *brkga.search(sosc);
   r2.first.print();
   r2.second.print();

   cout << "Program ended successfully" << endl;
   return 0;
}
