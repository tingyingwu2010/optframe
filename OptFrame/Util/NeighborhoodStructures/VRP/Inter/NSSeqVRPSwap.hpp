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


#ifndef OPTFRAME_NSSEQVRPSWAP_HPP_
#define OPTFRAME_NSSEQVRPSWAP_HPP_
/*
 Classical Problem: Vehicle Routing Problem

 */

// Framework includes
#include "../../../../Move.hpp"
#include "../../../../NSSeq.hpp"

using namespace std;

template<class T, class ADS = OPTFRAME_DEFAULT_ADS, class DS = OPTFRAME_DEFAULT_DS>
class MoveVRPSwap: public Move<vector<vector<T> >, ADS, DS >
{

	typedef vector<vector<T> > Routes;

protected:
	int r1, r2; // route 1 and route 2, respectively
	int c1, c2; // client in route 1 and client in route 2, respectively

	OPTFRAME_DEFAULT_PROBLEM* problem;
public:

	MoveVRPSwap(int _r1, int _r2, int _c1, int _c2, OPTFRAME_DEFAULT_PROBLEM* _problem = NULL) :
	r1(_r1), r2(_r2), c1(_c1), c2(_c2), problem(_problem)
	{
	}

	virtual ~MoveVRPSwap()
	{
	}

	int get_r1()
	{
		return r1;
	}

	int get_r2()
	{
		return r2;
	}

	int get_c1()
	{
		return c1;
	}

	int get_c2()
	{
		return c2;
	}

	bool canBeApplied(const Routes& rep, const ADS&)
	{
		bool all_positive = (r1 >= 0) && (r2 >= 0) && (c1 >= 0) && (c2 >= 0);
		return all_positive && (rep.size() >= 2);
	}

	MoveVRPSwap<T, ADS, DS >& apply(Routes& rep, const ADS&)
	{
		int aux;
		aux = rep.at(r1).at(c1);
		rep.at(r1).at(c1) = rep.at(r2).at(c2);
		rep.at(r2).at(c2) = aux;

		return *new MoveVRPSwap<T, ADS, DS >(r2, r1, c2, c1);
	}


	virtual bool operator==(const Move<Routes>& _m) const
	{
		const MoveVRPSwap<T, ADS, DS >& m1 = (const MoveVRPSwap<T, ADS, DS >&) _m;
		return ((m1.r1 == r1) && (m1.r2 == r2) && (m1.c1 == c1) && (m1.c2 == c2));
	}

	void print() const
	{
		cout << "MoveVRPSwap( ";
		cout << " route[" << r1 << "] client " << c1 << " <=>  route[" << r2 << "] client " << c2 << ")";
		cout << endl;
	}
};

template<class T, class ADS = OPTFRAME_DEFAULT_ADS, class DS = OPTFRAME_DEFAULT_DS,
		class MOVE = MoveVRPSwap<T, ADS, DS> , class P = OPTFRAME_DEFAULT_PROBLEM>
class NSIteratorVRPSwap: public NSIterator<vector<vector<T> >, ADS, DS >
{

	typedef vector<vector<T> > Routes;

private:

	MOVE* m;
	vector<MOVE*> moves;
	int r1, r2; // route 1 and route 2, respectively
	int c1, c2; // client in route 1 and client in route 2, respectively
	const Routes& r;

	P* p; // has to be the last

public:

	NSIteratorVRPSwap(const Routes& _r) :
	r(_r), p(_p)
	{
		m = NULL;
	}

	virtual ~NSIteratorVRPSwap()
	{
	}

	void first()
	{
		if (r.size() >= 2)
		{

			for (r1 = 0; r1 < r.size() - 2; r1++)
				for (c1 = 0; c1 < r.at(r1).size(); c1++)
					for (r2 = r1 + 1; r2 < r.size() - 1; r2++)
						for (c2 = 0; c2 < r.at(r2).size(); c2++)
							moves.push_back(new MOVE(r1, r2, c1, c2,p));

			if (moves.size() > 0)
			{
				m = moves[0];
				moves.erase(moves.begin() + 0);
			}
			else
				m = NULL;
		}
		else
			m = NULL;
	}

	void next()
	{
		if (moves.size() > 0)
		{
			m = moves[0];
			moves.erase(moves.begin() + 0);
		}
		else
			m = NULL;

	}

	bool isDone()
	{
		return m == NULL;
	}

	Move<T, ADS, DS>& current()
	{
		if (isDone())
		{
			cout << "There isnt any current element!" << endl;
			cout << "NSSeqVRPSwap. Aborting." << endl;
			exit(1);
		}

		return *m;
	}
};

template<class T, class ADS = OPTFRAME_DEFAULT_ADS, class DS = OPTFRAME_DEFAULT_DS,
		class MOVE = MoveVRPSwap<T, ADS, DS> , class P = OPTFRAME_DEFAULT_PROBLEM>
class NSSeqVRPSwap: public NSSeq<vector<vector<T> >, ADS, DS >
{

	typedef vector<vector<T> > Routes;

private:
	P* p; // has to be the last

public:

	NSSeqVRPSwap(P* _p = NULL) :
		p(_p)
	{
	}

	virtual ~NSSeqVRPSwap()
	{
	}

	Move<Routes, ADS, DS>& move(const Routes& , const ADS&)
	{
		//getchar();
		if (rep.size() < 2)
			return *new MOVE(-1, -1, -1, -1, p);

		int r1 = rand() % rep.size();

		if (rep.at(r1).size() == 0)
			return *new MOVE(-1, -1, -1, -1, p);

		int r2;

		do
		{
			r2 = rand() % rep.size();
		} while (r1 == r2);

		if (rep.at(r2).size() == 0)
			return *new MOVE(-1, -1, -1, -1, p);

		int c1 = rand() % rep.at(r1).size();
		int c2 = rand() % rep.at(r2).size();

		return *new MOVE(r1, r2, c1, c2, p);
	}

	virtual NSIteratorVRPSwap<T, ADS, DS, MOVE, P >& getIterator(const Routes& r, const ADS&)
	{
		return *new NSIteratorVRPSwap<T, ADS, DS, MOVE, P >(r, p);
	}

	virtual void print()
	{
		cout << "NSSeqVRPSwap with move: " << MOVE::idComponent();
	}
};

#endif /*OPTFRAME_NSSEQVRPSWAP_HPP_*/
