/*
	This file is part of libpq-async++
	Copyright (C) 2011-2018 Michel Denommee (and other contributing authors)
	
	This library is free software; you can redistribute it and/or
	modify it under the terms of the GNU Lesser General Public
	License as published by the Free Software Foundation; either
	version 2.1 of the License, or (at your option) any later version.

	This library is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
	Lesser General Public License for more details.

	You should have received a copy of the GNU Lesser General Public
	License along with this library; if not, write to the Free Software
	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
*/

#ifndef _pq_asyncpp_sys_h
#define _pq_asyncpp_sys_h

#include "exceptions.h"
#include "utils.h"


namespace pq_async {

class oid {
public:
	oid()
		: _oid(0)
	{
	}
	
	oid(const u_int32_t val)
		: _oid(val)
	{
	}
	
	oid(const oid& b)
	{
		_oid = b._oid;
	}
	
	u_int32_t data() const
	{
		return _oid;
	}
	
	operator u_int32_t() const
	{
		return _oid;
	}
	
	operator std::string() const
	{
		return num_to_str(_oid);
	}
	
	bool operator ==(const oid& b) const
	{
		return _oid == b._oid;
	}
	
	bool operator !=(const oid& b) const
	{
		return _oid != b._oid;
	}
	
	bool operator <(const oid& b) const
	{
		return _oid < b._oid;
	}
	
	bool operator >(const oid& b) const
	{
		return _oid > b._oid;
	}
	
	bool operator <=(const oid& b) const
	{
		return _oid <= b._oid;
	}
	
	bool operator >=(const oid& b) const
	{
		return _oid >= b._oid;
	}
	
private:
	u_int32_t _oid;
	
};

} //ns pq_async

#endif //_pq_asyncpp_sys_h