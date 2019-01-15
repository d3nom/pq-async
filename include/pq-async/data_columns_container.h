/*
    This file is part of pq-async
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

#ifndef _libpq_async_data_columns_container_h
#define _libpq_async_data_columns_container_h


#include "data_common.h"
#include "data_column.h"

namespace pq_async{

class data_columns_container;
typedef std::shared_ptr< pq_async::data_columns_container > 
    sp_data_columns_container;

class data_columns_container
    : public std::vector<sp_data_column>
{
public:

    data_columns_container();
    virtual ~data_columns_container();

    sp_data_column get_col(int idx);
    sp_data_column get_col(const char* col_name);
    
    int32_t get_col_index(const char* col_name) const
    {
        std::string tmp_name = pq_async::str_to_lower(col_name);

        for(unsigned int i = 0; i < this->size(); ++i)
            if(strcmp((*this)[i]->get_cname(), tmp_name.c_str()) == 0)
                return (int)i;
        
        return -1;
    }


private:

};

} //namespace pq_async

#endif //_libpq_async_data_columns_container_h
