#pragma once
#include <iostream>

#include <string>
#include <vector>
#include <deque>

#include <fstream>
#include <bitset>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

class TapeController
{
public:
	int M;
	int N;

	float delay_read;
	float delay_write;
	float delay_move;

	std::string Tape_in_name = "Tape_in.txt";
	std::string Tape_out_name = "Tape_out.txt";

	TapeController(std::string cfg_name)
	{
		namespace pt = boost::property_tree;
		pt::ptree root;
		pt::read_json(cfg_name, root);

		std::vector< std::pair<std::string, std::string> > results;
		// Iterator over all results
		for (pt::ptree::value_type& result : root.get_child("Tape"))
			results.push_back(std::make_pair(result.first, result.second.data()));

		for (auto& p : results)
		{
			if (p.first == "M")
				M = stoi(p.second);
			if (p.first == "N")
				N = stoi(p.second);
			if (p.first == "delay_read")
				delay_read = stof(p.second);
			if (p.first == "delay_write")
				delay_write = stof(p.second);
			if (p.first == "delay_move")
				delay_move = stof(p.second);
		}
	}

	int move_left(std::fstream& file)
	{
		long pos_g = file.tellg();
		long pos_p = file.tellp();
		file.seekp(pos_p - sizeof(int32_t));
		file.seekg(pos_g - sizeof(int32_t));

		return 0;
	}

	int move_right(std::fstream& file)
	{
		long pos_g = file.tellg();
		long pos_p = file.tellp();
		file.seekp(pos_p + sizeof(int32_t));
		file.seekg(pos_g + sizeof(int32_t));

		return 0;
	}

	int get_data(std::fstream& file)
	{
		int number;

		file.read((char*)&number, sizeof(number));

		//move seek back 4 bytes back
		long pos_g = file.tellg();
		file.seekg(pos_g - sizeof(int32_t));


		return number;
	}


	int set_data(std::fstream& file, int number)
	{

		file.write((char*)&number, sizeof(number));

		//move seek back 4 bytes back
		long pos_p = file.tellp();
		file.seekp(pos_p - sizeof(int32_t));

		return 0;
	}

	~TapeController()
	{

	}
};
