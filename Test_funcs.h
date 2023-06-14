#pragma once

#include <time.h>


std::vector<int> generate_fin(TapeController cntrl)
{

	srand(time(0));
	std::vector<int> gen_data;

	for (int i = 0; i < cntrl.N; i++)
	{
		gen_data.push_back(200 - rand() % 250);
	}

	//create file--------------------------------------------------
	std::fstream file(cntrl.Tape_in_name, std::ios::out | std::ios::binary);
	for (int i = 0; i < cntrl.N; i++)
	{
		cntrl.set_data(file, gen_data[i]);
		cntrl.move_right(file);
	}

	file.close();

	return gen_data;
}