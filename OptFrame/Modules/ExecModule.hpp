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

#ifndef EXECMODULE_HPP_
#define EXECMODULE_HPP_

#include "../OptFrameModule.hpp"
#include "../Constructive.h"

template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class M = OPTFRAME_DEFAULT_EMEMORY>
class ExecModule: public OptFrameModule<R, ADS, M>
{
public:

	virtual ~ExecModule()
	{
	}

	string id()
	{
		return "exec";
	}

	string usage()
	{
		return "exec [ <initsol> id | <loadsol> id | <initpop> id popSize | <loadpop> id ] target_fo timelimit method [output_solution_name]";
	}

	void run(vector<OptFrameModule<R, ADS, M>*>& all_modules, HeuristicFactory<R, ADS, M>* factory, map<string, string>* dictionary, string input)
	{
		cout << "exec: " << input << endl;
		Scanner scanner(input);

		if (!scanner.hasNext())
		{
			cout << "Usage: " << usage() << endl;
			return;
		}

		string sol = scanner.next();

		if ((sol != "initsol") && (sol != "loadsol") && (sol != "initpop") && (sol != "loadpop"))
		{
			cout << "First parameter must be either 'initsol', 'loadsol', 'initpop', 'loadpop'!" << endl;
			cout << "Usage: " << usage() << endl;
			return;
		}

		string id = scanner.next();

		Solution<R, ADS>* s = NULL;
		bool needDelete = false;

		Population<R, ADS> *p = NULL;
		//bool needDeletePop = false;

		if (sol == "loadsol")
		{
			Scanner s2(sol + " " + id);
         factory->readComponent(s, s2);
		}

		if (sol == "initsol")
		{
			Scanner s2(sol + " " + id);
			InitialSolution<R, ADS>* initsol = NULL;
         factory->readComponent(initsol, s2);
			s = &initsol->generateSolution();
			needDelete = true;
		}

		if (sol == "loadpop")
		{
			Scanner s2(sol + " " + id);
         factory->readComponent(p, s2);
		}

		if (sol == "initpop")
		{
			Scanner s2(sol + " " + id);
			InitialPopulation<R, ADS>* initpop = NULL;
         factory->readComponent(initpop, s2);

			unsigned popSize = scanner.nextInt();
			p = &initpop->generatePopulation(popSize);
		}

		double target_fo = scanner.nextDouble();
		double timelimit = scanner.nextDouble();

		pair<HTrajectory<R, ADS, M>*, string> method = factory->createHeuristic(scanner.rest());
		scanner = Scanner(method.second);

		// ---

		string s_new_id = "";

		if (sol == "initsol" || sol == "loadsol")
		{
			Solution<R, ADS>* sFinal = &method.first->search(*s, timelimit, target_fo);

			if (needDelete)
				delete s;

			int new_id = factory->addComponent(*sFinal);

			stringstream str;
			str << "loadsol " << new_id;
			s_new_id = str.str();

			cout << "'" << s_new_id << "' added." << endl;
		}
		else if (sol == "initpop" || sol == "loadpop")
		{
			Population<R, ADS> *pFinal;
			Population<R, ADS> *pAux;

			pAux = &(p->clone());

			pFinal = &method.first->search(*pAux, timelimit, target_fo);

			for (unsigned i = 0; i < pAux->size(); i++)
				delete &(pAux->at(i));

			int new_id = factory->addComponent(*pFinal);

			stringstream str;
			str << "loadpop " << new_id;
			s_new_id = str.str();

			cout << "'" << s_new_id << "' added." << endl;
		}

		// ---

		if (scanner.hasNext())
		{
			string new_name = scanner.next();
			OptFrameModule<R, ADS, M>::run_module("define", all_modules, factory, dictionary, new_name + " " + s_new_id);
		}

	}

};

#endif /* EXECMODULE_HPP_ */
