/* MapReduce Interface
Project: 
Author : 
Date   : 
*/

#include "../../SeqMR/SeqMR.h"
#include "../../util/Timer.hpp"
#include <iostream>
#include <cstdlib>

class MyMapper : public SeqMR_Mapper<int,int,int,int,int> {
public:
	MyMapper(MapReduce<int,int,int,int,int> * mr):SeqMR_Mapper<int,int,int,int,int>(mr){};
	virtual vector< pair<int,int> > map( pair<int,int> a ) 
	{ 
		vector< pair<int,int> > m; 
		//cout << "\tMapping..\n"; 
		m.push_back(make_pair(a.first,a.second));
		//cout << "\tOk\n"; 
		return m; 
	};
};

class MyReducer : public SeqMR_Reducer<int,int,int,int,int> {
public:
	MyReducer(MapReduce<int,int,int,int,int> * mr):SeqMR_Reducer<int,int,int,int,int>(mr){};
	virtual pair<int,int> reduce( pair<int, vector<int> > bs ) 
	{
		int reduced = 0;
		//cout << "\tReducing..\n"; 
		for (int i = 0 ; i < bs.second.size() ; i++) reduced += bs.second[i];
		//cout << "\tOk\n"; 
		return pair<int,int>(bs.first,reduced); 
	};
};

int main()
{
	cout << "SeqMR test" << endl;
	Timer timer;

	int n = 1000000;
	srand(n);

	vector< pair<int,int> > input;
	for (int i = 0 ; i < n ; i++) input.push_back(make_pair(rand()%n,rand()%n));

	SeqMR_MapReduce<int,int,int,int,int> mapReduce;
	MyMapper mapper(&mapReduce);
	MyReducer reducer(&mapReduce);

	vector< pair<int,int> > output = mapReduce.run(mapper,reducer,input);

	return 0;
}
