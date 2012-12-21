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

#ifndef OPTFRAME_DIC_FUNCTION_HPP_
#define OPTFRAME_DIC_FUNCTION_HPP_

#include <iostream>
#include <ostream>

#include <vector>
#include <map>

#include "../Scanner++/Scanner.h"

#include "../OptFrameFunction.hpp"

#include "../OptFrameList.hpp"

#include <algorithm>

class DicFunction : public OptFrameFunction
{
public:

	virtual ~DicFunction()
	{
	}

	virtual string id()
	{
		return "dic";
	}

	virtual string usage()
	{
		return "dic( var ) : return value of variable in dictionary";
	}

	virtual string* run(vector<OptFrameFunction*>&, map< string, string >& dictionary, map< string,vector<string> >&, string body)
	{
		Scanner scanner(body);

		if(!scanner.hasNext())
			return NULL;

		string var = scanner.next();

		return new string(dictionary[var]);
	}
};

#endif /* OPTFRAME_DIC_FUNCTION_HPP_ */
