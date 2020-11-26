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

#ifndef OPTFRAME_BRKGA_HPP_
#define OPTFRAME_BRKGA_HPP_

#include <algorithm>

#include "../../SingleObjSearch.hpp"
#include "../../Population.hpp"
#include "../../InitialPopulation.h"

#include "RKGA.hpp"

// BRKGA - Biased-Random Key Genetic Algorithms

namespace optframe
{

// It seems that R is connected to S here... S<R> is rule perhaps?
// maybe better to not provide XRS directly here (used to be = = RSolution<R>)
// RKeys is the representation of random keys
//
//template<XRepresentation R, XRSolution<R> XRS, XEvaluation XEv, XRepresentation RKeys = optframe::random_keys>
//
// Assuming XSolution allows "raw representation" such as vector<int>...
// Will require XSolution instead of XRepresentation, since legacy Evaluation already deals with that
// User may also want to "decode" the ADS from keys, if necessary...
//
template<XSolution S, XEvaluation XEv, optframe::comparability KeyType = double, XESolution XES = pair<S, XEv>>
class BRKGA: public RKGA<S, XEv, KeyType, XES>
{
   //using RSK = RSolution<RKeys>;
   using RSK = std::vector<KeyType>;
protected:
	double probElitism;

public:

	BRKGA(DecoderRandomKeys<S, XEv, KeyType>& _decoder, InitialPopulation<S, XEv>& _initPop, unsigned numGen, unsigned _popSize, double fracTOP, double fracBOT, double probElielitismRate, double _probElitism) :
		RKGA<S, XEv, KeyType, XES>(_decoder, _initPop, numGen, _popSize, fracTOP, fracBOT), probElitism(_probElitism)
	{
		assert(probElitism > 0.5);
		assert(probElitism <= 1.0);
	}

	BRKGA(DecoderRandomKeys<S, XEv, KeyType>& _decoder, int key_size, unsigned numGen, unsigned popSize, double fracTOP, double fracBOT, double _probElitism) :
			RKGA<S, XEv, KeyType, XES>(_decoder, key_size, numGen, popSize, fracTOP, fracBOT), probElitism(_probElitism)
	{
		assert(probElitism > 0.5);
		assert(probElitism <= 1.0);
	}

	BRKGA(Evaluator<S, XEv>& _evaluator, int key_size, unsigned numGen, unsigned _popSize, double fracTOP, double fracBOT, double _probElitism) :
			RKGA<S, XEv, KeyType, XES>(_evaluator, key_size, numGen, _popSize, fracTOP, fracBOT), probElitism(_probElitism)
	{
		assert(probElitism > 0.5);
		assert(probElitism <= 1.0);
	}

	virtual ~BRKGA()
	{
	}

	virtual RSK& cross(const Population<RSK, XEv>& pop) const
	{
		assert(this->sz > 0); // In case of using InitPop, maybe must receive a Selection or Crossover object...

		const RSK& p1 = pop.at(rand() % this->eliteSize);
		const RSK& p2 = pop.at(this->eliteSize + rand() % (pop.size()-this->eliteSize));

		random_keys* v = new random_keys(this->sz, 0.0);
		for (int i = 0; i < this->sz; i++)
			if ((rand() % 1000000)/1000000.0 < probElitism)
				//v->at(i) = p1.getR()[i];
            v->at(i) = p1[i];
			else
				//v->at(i) = p2.getR()[i];
            v->at(i) = p2[i];
		//return *new RSK(v);
      return *v; // TODO: pass by std::move() or unique_ptr
	}

};

} // namespace optframe

#endif /*OPTFRAME_BRKGA_HPP_*/
