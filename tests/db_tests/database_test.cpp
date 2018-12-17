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

#include <gmock/gmock.h>
#include "../db_test_base.h"

namespace pq_async{ namespace tests{

class database_test
	: public db_test_base
{
public:
	void drop_table()
	{
		db->execute("drop table if exists database_test");
	}
	
	void create_table()
	{
		this->drop_table();
		db->execute(
			"create table database_test("
			"id serial primary key, value text"
			");"
		);
	}
	
	void SetUp() override
	{
		db_test_base::SetUp();
		this->create_table();
	}
	
	void TearDown() override
	{
		this->drop_table();
		db_test_base::TearDown();
	}
};


TEST_F(database_test, max_connection_sync_test)
{
	try{
		// max con minus the used connection in the the db_test_base
		//size_t max_con = pq_async::connection_pool::get_max_conn() -1;
		size_t nb_con = pq_async::connection_pool::get_max_conn() +20;
		sp_database dbs[nb_con + 1];
		dbs[nb_con] = this->db;
		
		for(size_t i = 0; i < nb_con; ++i)
			dbs[i] = pq_async::database::open(pq_async_connection_string);
		++nb_con;
		
		for(size_t i = 0; i < nb_con; ++i){
			auto ldb = dbs[i];
			ldb->execute(
				"insert into database_test(value) values ($1)",
				std::string("acb") + num_to_str(i)
			);
			std::cout << "execute " << i << " completed" << std::endl;
		}
		
		for(size_t i = 0; i < nb_con; ++i){
			auto ldb = dbs[i];
			auto tbl = ldb->query("select * from database_test");
			std::cout << "select " << i << " completed" << std::endl;
		}

	}catch(const std::exception& err){
		std::cout << "Error: " << err.what() << std::endl;
		FAIL();
	}
}

TEST_F(database_test, max_connection_async_test)
{
	try{
		size_t nb_con = pq_async::connection_pool::get_max_conn()*2;
		sp_database dbs[nb_con];
		dbs[0] = this->db;
		dbs[0]->get_strand()->data(0);
		
		std::cout << "running test with, max pool size: "
			<< pq_async::connection_pool::get_max_conn()
			<< ", db count: " << nb_con << std::endl;
		
		for(size_t i = 1; i < nb_con; ++i){
			dbs[i] = pq_async::database::open(pq_async_connection_string);
			dbs[i]->get_strand()->data(i);
		}
		for(size_t i = 0; i < nb_con; ++i){
			auto ldb = dbs[i];
			pq_async_log_debug("starting exec %i", i);
			ldb->execute(
				"insert into database_test(value) values ($1)",
				std::string("acb-") + num_to_str(i),
				[i](const cb_error& err){
					if(err){
						std::cout << "exec " << i 
							<< " err: " << err << std::endl;
						FAIL();
						return;
					}
					pq_async_log_debug("exec %i completed", i);
				}
			);
		}
		
		for(size_t i = 0; i < nb_con; ++i){
			auto ldb = dbs[i];
			pq_async_log_debug("starting select %i", i);
			ldb->query(
				"select * from database_test",
				[i](const cb_error& err, sp_data_table tbl){
					if(err){
						std::cout << "select " << i 
							<< " err: " << err << std::endl;
						FAIL();
						return;
					}
					pq_async_log_debug("select %i completed", i);
				}
			);
		}
		
		pq_async::event_queue::get_default()->run();
		
		db->query(
			"select * from database_test",
		[nb_con](const cb_error& err, sp_data_table tbl){
				if(err){
					std::cout << "err: " << err << std::endl;
					FAIL();
					return;
				}
			
			ASSERT_THAT(tbl->size(), testing::Eq(nb_con));
		});

		pq_async::event_queue::get_default()->run();
		
	}catch(const std::exception& err){
		std::cout << "Error: " << err.what() << std::endl;
		FAIL();
	}
}


TEST_F(database_test, max_connection_async2_test)
{
	try{
		size_t nb_con = pq_async::connection_pool::get_max_conn()*2;
		sp_database dbs[nb_con];
		dbs[0] = this->db;
		dbs[0]->get_strand()->data(0);
		
		std::cout << "running test with, max pool size: "
			<< pq_async::connection_pool::get_max_conn()
			<< ", db count: " << nb_con << std::endl;
		
		for(size_t i = 1; i < nb_con; ++i){
			dbs[i] = pq_async::database::open(pq_async_connection_string);
			dbs[i]->get_strand()->data(i);
		}
		
		auto eq = pq_async::event_queue::get_default();
		
		eq->each(dbs, dbs+nb_con,
		[eq](const sp_database& ldb, async_cb ecb)->void{
			eq->series({
				[ldb](async_cb scb){
					int i = ldb->get_strand()->data();
					ldb->execute(
						"insert into database_test(value) values ($1)",
						std::string("acb-") + num_to_str(i),
						[scb, i](const cb_error& err){
							if(err){
								std::cout << "exec " << i 
									<< " err: " << err << std::endl;
								scb(err);
								return;
							}
							pq_async_log_debug("exec %i completed", i);
							scb(nullptr);
						}
					);
				},
				[ldb](async_cb scb){
					int i = ldb->get_strand()->data();
					ldb->query(
						"select * from database_test",
						[scb, i](const cb_error& err, sp_data_table tbl){
							if(err){
								std::cout << "select " << i 
									<< " err: " << err << std::endl;
								scb(err);
								return;
							}
							pq_async_log_debug("select %i completed", i);
							scb(nullptr);
						}
					);
				}
			}, ecb);
		}, [](const cb_error& err)->void{
			if(err)
				FAIL();
		});
		
		eq->run();
		
		db->query(
			"select * from database_test",
		[nb_con](const cb_error& err, sp_data_table tbl){
				if(err){
					std::cout << "err: " << err << std::endl;
					FAIL();
					return;
				}
			
			ASSERT_THAT(tbl->size(), testing::Eq(nb_con));
		});
		
		eq->run();
		
	}catch(const std::exception& err){
		std::cout << "Error: " << err.what() << std::endl;
		FAIL();
	}
}


}} //namespace pq_async::tests