// OptFrame - Optimization Framework

// Copyright (C) 2009, 2010, 2011
// http://optframe.sourceforge.net/
//
// This file is part of the OptFrame optimization framework. This framework
// is free software; you can redistribute it and/or modify it under the
// terms of the GNU Lesser General Public License v3 as published by the
// Free Software Foundation.

// This framework is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License v3 for more details.

// You should have received a copy of the GNU Lesser General Public License v3
// along with this library; see the file COPYING.  If not, write to the Free
// Software Foundation, 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301,
// USA.

#ifndef TESTMODULE_HPP_
#define TESTMODULE_HPP_

#include "../OptFrameModule.hpp"
#include <math.h>

template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class M = OPTFRAME_DEFAULT_EMEMORY>
class TestModule: public OptFrameModule<R, ADS, M>
{
public:
	string id()
	{
		return "test";
	}
	string usage()
	{
		string u = "test N T TF BF [ initsol id | loadsol id ] EVAL METHOD OUTPUTFILE [solution_name]\n WHERE:\n";
		u += "N is the number of tests to be executed;\n";
		u += "T is the timelimit, in seconds, for each test; (0 for no timelimit)\n";
		u += "TF is the target evaluation function value;\n";
		u += "BF is the best known evaluation function value;\n";
		u += "EVAL is the main evaluator; (e.g. ev 0)\n";
		u += "METHOD is the method to be tested with its own parameters;\n";
		u += "OUTPUTFILE is the output file;\n";
		u += "[solution_name] is a name given to the best found solution (optional).";

		return u;
	}

	void run(vector<OptFrameModule<R, ADS, M>*>& all_modules, HeuristicFactory<R, ADS, M>* factory, map<string, string>* dictionary, string input)
	{
		Scanner scanner(input);

		if (!scanner.hasNext())
		{
			cout << "Usage: " << usage() << endl;
			return;
		}

		int n = scanner.nextInt();
		int t = scanner.nextDouble();
		double tf = scanner.nextDouble();
		double bf = scanner.nextDouble();

      // -----------------------------
      // option 'loadsol' or 'initsol'
      // -----------------------------

      InitialSolution<R, ADS>* initsol = NULL;
      Solution<R, ADS>* s = NULL;

      string sol_option = scanner.next();
      int option_id = scanner.nextInt();

      if (sol_option == "OptFrame:loadsol")
         factory->assign(s, "OptFrame:loadsol", option_id);

      if (sol_option == "OptFrame:initsol")
         factory->assign(initsol, "OptFrame:initsol", option_id);

      // -------------------------------

		Evaluator<R, ADS, M>* eval = factory->read_ev(&scanner);
		pair<Heuristic<R, ADS, M>*, string> method = factory->createHeuristic(scanner.rest());

		Heuristic<R, ADS, M>* h = method.first;

		string rest = method.second;

		Scanner scan_rest(rest);
		//TODO: check \" in filename to allow spaces in the name
		string filename = scan_rest.next();

		if (t == 0) // if no timelimit
			t = 1000000000;

		long timelimit = t;

		if (bf == 0) // if best is zero
			bf = 0.00001;

		FILE* file = fopen(filename.c_str(), "a");
		if (!file)
		{
			cout << "Error creating file '" << filename << "'" << endl;
			return;
		}

		fprintf(file, "PARAMETERS:%s\n", input.c_str());

		//bool minimization = eval->betterThan(1, 2); // TODO
		Solution<R, ADS>* s_star = NULL;

		double s_fo_ini = 0;
		double s_t_ini = 0;
		double s_fo_end = 0;
		double s_t_end = 0;
		double min_fo = 1000000000;
		double min_t = 1000000000;
		double max_fo = -1000000000;
		double max_t = -1000000000;

		double t_now = 0;
		double fo_now = 0;
		double variance = 0;

		vector<long double> s_fo_tests(n);

		for (int i = 0; i < n; i++)
		{
			cout << "Test " << i << "... Running";
			Timer t(false);

         if (initsol)
            s = &initsol->generateSolution();
         else
            cout << "no initsol!" << endl;

			t_now = t.now();
			Evaluation< M > & e = eval->evaluate(*s);
			fo_now = e.evaluation();
			delete &e;
			fprintf(file, "%.3f\t%.3f\t", fo_now, t_now);
			s_fo_ini += fo_now;
			s_t_ini += t_now;

			Solution<R, ADS>* s2 = &h->search(*s, timelimit, tf);
			t_now = t.now();
			Evaluation< M > & e2 = eval->evaluate(*s2);
			fo_now = e2.evaluation();
			delete &e2;
			s_fo_tests.at(i) = fo_now;
			fprintf(file, "%.3f\t%.3f\t", fo_now, t_now);
			s_fo_end += fo_now;
			s_t_end += t_now;

			if (fo_now < min_fo)
				min_fo = fo_now;
			if (t_now < min_t)
				min_t = t_now;
			if (fo_now > max_fo)
				max_fo = fo_now;
			if (t_now > max_t)
				max_t = t_now;

			cout << "... Finished! (" << t.now() << "secs.)" << endl;

			if (!s_star)
				s_star = &s2->clone();
			else if (eval->betterThan(*s2, *s_star))
			{
				delete s_star;
				s_star = &s2->clone();
			}

         if (initsol)
            delete s;

			delete s2;

			fprintf(file, "\n");
		}

		s_fo_ini /= n;
		s_t_ini /= n;
		s_fo_end /= n;
		s_t_end /= n;

		// calculating variance
		for (int i = 0; i < n; i++)
		{
			variance += pow((s_fo_end - s_fo_tests[i]), 2);
		}
		variance /= (n * 1.0);

		fprintf(file, "AVERAGE:\t%f\t%f\t%f\t%f\n", s_fo_ini, s_t_ini, s_fo_end, s_t_end);
		fprintf(file, "MIN:\t-\t-\t%f\t%f\n", min_fo, min_t);
		fprintf(file, "MAX:\t-\t-\t%f\t%f\n", max_fo, max_t);
		if (min_fo == 0)
			min_fo = 0.00001;
		fprintf(file, "VARIABILITY:\t-\t-\t%f\t-\n", (s_fo_end - min_fo) / min_fo);
		fprintf(file, "VARIANCE: \t-\t-\t%f\t-\n", variance);
		fprintf(file, "STANDARD DEVIATION : \t-\t-\t%f\t-\n", sqrt(variance));
		fprintf(file, "GAP_FROM_BEST:\t%f\t-\t%f\t-\n", (s_fo_ini - bf) / bf, (s_fo_end - bf) / bf);
		fprintf(file, "GAP_FROM_AVG:\t%f\t-\t%f\t-\n", (s_fo_ini - bf) / s_fo_ini, (s_fo_end - bf) / s_fo_end);
		fprintf(file, "IMPROVEMENT:\t-\t-\t%f\t-\n", (bf - min_fo) / bf);
		fprintf(file, "BEST(LIT):\t%f\n", bf);

		fclose(file);

		int new_id = factory->addComponent(*s_star);

		stringstream str;
		str << "OptFrame:loadsol " << new_id;
		string s_new_id = str.str();

		cout << "'" << s_new_id << "' added." << endl;

		if (scan_rest.hasNext())
		{
			string new_name = scan_rest.next();
			run_module("define", all_modules, factory, dictionary, new_name + " " + s_new_id);
		}
	}

};

#endif /* TESTMODULE_HPP_ */
