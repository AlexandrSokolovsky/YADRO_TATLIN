#define BOOST_TEST_MODULE mytests
#include <boost/test/included/unit_test.hpp>

#include "TapeController.h"
#include "TapeSort.h"

#include "Test_funcs.h"

TapeController cntrl("cfg.json");


//test move_right
BOOST_AUTO_TEST_CASE(move_right)
{
	generate_fin(cntrl);

	std::fstream fin(cntrl.Tape_in_name, std::ios::in | std::ios::out | std::ios::binary);

	long pos_g = fin.tellg();
	long pos_p = fin.tellp();
	for (int i = 0; i < cntrl.N; i++)
	{
		pos_g += sizeof(int32_t);
		pos_p += sizeof(int32_t);
		cntrl.move_right(fin);
		BOOST_REQUIRE_EQUAL(pos_p, fin.tellp());
		BOOST_REQUIRE_EQUAL(pos_g, fin.tellg());
	}
	fin.close();
	std::cout << "+ move_right approved" << std::endl;
}

//test move_left
BOOST_AUTO_TEST_CASE(move_left)
{
	generate_fin(cntrl);

	std::fstream fin(cntrl.Tape_in_name, std::ios::in | std::ios::out | std::ios::binary);
	fin.seekg(0,fin.end);

	long pos_g = fin.tellg();
	long pos_p = fin.tellp();
	for (int i = 0; i < cntrl.N; i++)
	{
		pos_g -= sizeof(int32_t);
		pos_p -= sizeof(int32_t);
		cntrl.move_left(fin);
		BOOST_REQUIRE_EQUAL(pos_p, fin.tellp());
		BOOST_REQUIRE_EQUAL(pos_g, fin.tellg());
	}
	fin.close();
	std::cout << "+ move_left approved" << std::endl;
}

//test read_data
BOOST_AUTO_TEST_CASE(read_data)
{
	std::fstream fin(cntrl.Tape_in_name, std::ios::in | std::ios::out | std::ios::binary);

	std::vector<int> gen_data(generate_fin(cntrl));

	for (int i = 0; i < cntrl.N; i++)
	{
		int test_val = cntrl.get_data(fin);
		int real_val = gen_data[i];
		BOOST_REQUIRE_EQUAL(test_val, real_val);

		cntrl.move_right(fin);
	}
	std::cout << "    + read all to the right approved" << std::endl;

	for (int i = cntrl.N-1; i >= 0; i--)
	{
		cntrl.move_left(fin);
		int test_val = cntrl.get_data(fin);
		int real_val = gen_data[i];
		BOOST_REQUIRE_EQUAL(test_val, real_val);		
	}
	std::cout << "    + read all to the left approved" << std::endl;

	fin.seekg(0, fin.beg);
	for (int i = 0; i < cntrl.N; i++)
	{
		int test_val = cntrl.get_data(fin);
		int real_val = gen_data[0];
		BOOST_REQUIRE_EQUAL(test_val, real_val);
	}
	std::cout << "    + read all without moving approved" << std::endl;

	fin.close();
	std::cout << "+ read_data approved" << std::endl;
}

//test write_data
BOOST_AUTO_TEST_CASE(write_data)
{
	std::fstream fin(cntrl.Tape_in_name, std::ios::in | std::ios::out | std::ios::binary);

	std::vector<int> gen_data(generate_fin(cntrl));

	for (int i = 0; i < cntrl.N; i++)
	{
		int test_val = 15466792; 
		cntrl.set_data(fin, test_val);
		int real_val = cntrl.get_data(fin);
		BOOST_REQUIRE_EQUAL(test_val, real_val);

		cntrl.move_right(fin);
	}
	std::cout << "    + write all to the right approved" << std::endl;

	for (int i = cntrl.N - 1; i >= 0; i--)
	{
		cntrl.move_left(fin);

		int test_val = 15466792;
		cntrl.set_data(fin, test_val);
		int real_val = cntrl.get_data(fin);
		BOOST_REQUIRE_EQUAL(test_val, real_val);		
	}
	std::cout << "    + write all to the left approved" << std::endl;

	fin.seekp(0, fin.beg);
	fin.seekg(0, fin.beg);
	for (int i = 0; i < cntrl.N; i++)
	{
		int test_val = 15466792;
		cntrl.set_data(fin, test_val);
		int real_val = cntrl.get_data(fin);
		BOOST_REQUIRE_EQUAL(test_val, real_val);
	}
	std::cout << "    + write all without moving approved" << std::endl;

	fin.close();
	std::cout << "+ write_data approved" << std::endl;
}


//test sorting
BOOST_AUTO_TEST_CASE(sorting)
{
	std::vector<int> gen_data(generate_fin(cntrl));

	TapeSort TapeSorter;
	TapeSorter.Sort(cntrl);

	{
		std::fstream fout(cntrl.Tape_out_name, std::ios::in | std::ios::out | std::ios::binary);

		sort(gen_data.begin(), gen_data.end());
		
		for (auto it : gen_data)
		{
			int test = cntrl.get_data(fout);
			int real = it;
			BOOST_REQUIRE_EQUAL(test, real);
			cntrl.move_right(fout);
		}

		std::cout << "    + sorting with random values approved" << std::endl;
		fout.close();
	}

	{
		std::fstream fout(cntrl.Tape_out_name, std::ios::in | std::ios::out | std::ios::binary);
		gen_data.clear();
		gen_data = { 112837,2,3,5,7,1,2,19,17,26,15,9,4,4,5,182,19,82,1137,2,33,5,72,1,42,19,17,26,113425,9,4,14,5,182,19,82,37,38,39,40 }; //40elems
		cntrl.N = 40;
		cntrl.M = 10;

		std::fstream file(cntrl.Tape_in_name, std::ios::out | std::ios::binary);
		for (int i = 0; i < cntrl.N; i++)
		{
			cntrl.set_data(file, gen_data[i]);
			cntrl.move_right(file);
		}
		file.close();

		TapeSorter.Sort(cntrl);

		sort(gen_data.begin(), gen_data.end());
		fout.seekg(0, fout.beg);
		for (auto it : gen_data)
		{
			int test = cntrl.get_data(fout);
			int real = it;
			BOOST_REQUIRE_EQUAL(test, real);
			cntrl.move_right(fout);
		}

		std::cout << "    + sorting with N = 40, M = 10 approved" << std::endl;
		fout.close();
	}

	{
		std::fstream fout(cntrl.Tape_out_name, std::ios::in | std::ios::out | std::ios::binary);
		gen_data.clear();
		gen_data = { 112837,2,3,5,7,1,2,19,17,26,15,9,4,4,5,182,19,82,1137,2,33,5,72,1,42,19,17,26,113425,9,4,14,5,182,19,82 }; //36
		cntrl.N = 36;
		cntrl.M = 10;

		std::fstream file(cntrl.Tape_in_name, std::ios::out | std::ios::binary);
		for (int i = 0; i < cntrl.N; i++)
		{
			cntrl.set_data(file, gen_data[i]);
			cntrl.move_right(file);
		}
		file.close();

		TapeSorter.Sort(cntrl);

		sort(gen_data.begin(), gen_data.end());
		fout.seekg(0, fout.beg);
		for (auto it : gen_data)
		{
			int test = cntrl.get_data(fout);
			int real = it;
			BOOST_REQUIRE_EQUAL(test, real);
			cntrl.move_right(fout);
		}

		std::cout << "    + sorting with N = 36, M = 10 approved" << std::endl;
		fout.close();
	}

	{
		std::fstream fout(cntrl.Tape_out_name, std::ios::in | std::ios::out | std::ios::binary);
		gen_data.clear();
		gen_data = { 112837 };
		cntrl.N = 1;
		cntrl.M = 1;

		std::fstream file(cntrl.Tape_in_name, std::ios::out | std::ios::binary);
		for (int i = 0; i < cntrl.N; i++)
		{
			cntrl.set_data(file, gen_data[i]);
			cntrl.move_right(file);
		}
		file.close();

		TapeSorter.Sort(cntrl);

		sort(gen_data.begin(), gen_data.end());
		fout.seekg(0, fout.beg);
		for (auto it : gen_data)
		{
			int test = cntrl.get_data(fout);
			int real = it;
			BOOST_REQUIRE_EQUAL(test, real);
			cntrl.move_right(fout);
		}

		std::cout << "    + sorting with N = 1, M = 1 approved" << std::endl;
		fout.close();
	}

	{
		std::fstream fout(cntrl.Tape_out_name, std::ios::in | std::ios::out | std::ios::binary);
		gen_data.clear();
		gen_data = {};
		cntrl.N = 0;
		cntrl.M = 0;

		std::fstream file(cntrl.Tape_in_name, std::ios::out | std::ios::binary);
		for (int i = 0; i < cntrl.N; i++)
		{
			cntrl.set_data(file, gen_data[i]);
			cntrl.move_right(file);
		}
		file.close();

		TapeSorter.Sort(cntrl);

		sort(gen_data.begin(), gen_data.end());
		fout.seekg(0, fout.beg);
		for (auto it : gen_data)
		{
			int test = cntrl.get_data(fout);
			int real = it;
			BOOST_REQUIRE_EQUAL(test, real);
			cntrl.move_right(fout);
		}

		std::cout << "    + sorting with N = 0, M = 0 approved" << std::endl;
		fout.close();
	}


	std::cout << "+ sorting approved" << std::endl;
}
