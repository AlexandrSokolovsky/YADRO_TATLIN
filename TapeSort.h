#pragma once

#include <boost/filesystem/operations.hpp>

class TapeSort
{
public:
	int Sort(TapeController cntrl)
	{
		//div file by M bytes-----------------------------------------------------------------
		std::deque<std::string> file_temp_names;

		if (cntrl.M <= 0)
		{
			std::cerr << "Not enough RAM" << std::endl;
			return -1;
		}

		std::fstream fin(cntrl.Tape_in_name, std::ios::in | std::ios::out | std::ios::binary);


		int temp_file = 0;
		if (fin && !fin.eof())
		{
			while (fin)
			{
				std::vector<int> v_in_M_elems;

				//read M elements from Tape_in file
				int i;
				for (i = 0; fin && !fin.eof() && i < cntrl.M; i++)
				{
					v_in_M_elems.push_back(cntrl.get_data(fin));
					cntrl.move_right(fin);
				}

				if (i < cntrl.M || (temp_file * cntrl.M + i) > cntrl.N)
				{
					i--;
					v_in_M_elems.pop_back();
				}


				MergeSort(v_in_M_elems, 0, i - 1);

				
				boost::filesystem::path ftemp_name;
				if (i >= cntrl.N)
				{
					//write sorted data to Tape_out
					ftemp_name = cntrl.Tape_out_name;
				}
				else
				{
					//create Tape_tempX.txt in tmp dir if batches > 1
					ftemp_name = "tmp";
					ftemp_name = ftemp_name / ("Tape_temp" + std::to_string(temp_file) + ".txt");
					file_temp_names.push_back(ftemp_name.string());
				}

				std::fstream fout(ftemp_name.string(), std::ios::out | std::ios::binary);

				//write sorted batch to file
				for (int i = 0; i < v_in_M_elems.size(); i++)
				{
					cntrl.set_data(fout, v_in_M_elems[i]);
					cntrl.move_right(fout);
				}

				fout.close();

				if ((temp_file * cntrl.M + v_in_M_elems.size()) >= cntrl.N)
					break;
				temp_file++;
			}


			fin.close();
		}

		//div file end-----------------------------------------------------------------

		//merge files------------------------------------------------------------------

		while (file_temp_names.size() != 0)
		{
			temp_file = merge_files(file_temp_names, temp_file, cntrl);
		}
		//merge files end--------------------------------------------------------------
		return 0;
	}
private:
	int merge_files(std::deque<std::string>& file_temp_names, int temp_file, TapeController cntrl)
	{

		std::string f0, f1;
		boost::filesystem::path fout_name;
		f0 = file_temp_names.front();
		file_temp_names.pop_front();

		f1 = file_temp_names.front();
		file_temp_names.pop_front();

		std::fstream fin0(f0, std::ios::in | std::ios::out | std::ios::binary);
		std::fstream fin1(f1, std::ios::in | std::ios::out | std::ios::binary);





		//prepare output file name
		if (file_temp_names.size() == 0)
		{
			fout_name = cntrl.Tape_out_name;
		}
		else
		{
			temp_file++;
			fout_name = "tmp";
			fout_name = fout_name / ("Tape_temp" + std::to_string(temp_file) + ".txt");
			file_temp_names.push_back(fout_name.string());
		}


		std::fstream fout(fout_name.string(), std::ios::out | std::ios::binary);

		//merge files and write to output file elements 1 by 1
		int num0, num1;
		if (fin0 && !fin0.eof())
		{
			if (fin1 && !fin1.eof())
			{
				num0 = cntrl.get_data(fin0);
				num1 = cntrl.get_data(fin1);

				while (fin0 && fin1)
				{
					if (num0 <= num1)
					{
						cntrl.set_data(fout, num0);
						cntrl.move_right(fout);

						cntrl.move_right(fin0);
						num0 = cntrl.get_data(fin0);

					}
					else
					{
						cntrl.set_data(fout, num1);
						cntrl.move_right(fout);

						cntrl.move_right(fin1);
						num1 = cntrl.get_data(fin1);

					}

					if (!fin0 && fin1)
					{
						while (fin1)
						{
							cntrl.set_data(fout, num1);
							cntrl.move_right(fout);

							cntrl.move_right(fin1);
							num1 = cntrl.get_data(fin1);

						}
					}

					if (fin0 && !fin1)
					{
						while (fin0)
						{
							cntrl.set_data(fout, num0);
							cntrl.move_right(fout);

							cntrl.move_right(fin0);
							num0 = cntrl.get_data(fin0);

						}
					}

				}
				fout.close();
			}
		}


		fin0.close();
		fin1.close();




		return temp_file;
	}

	void MergeSortedIntervals(std::vector<int>& v, int s, int m, int e)
	{
		std::vector<int> temp;

		int i, j;
		i = s;
		j = m + 1;

		while (i <= m && j <= e) {

			if (v[i] <= v[j]) {
				temp.push_back(v[i]);
				++i;
			}
			else {
				temp.push_back(v[j]);
				++j;
			}

		}

		while (i <= m) {
			temp.push_back(v[i]);
			++i;
		}

		while (j <= e) {
			temp.push_back(v[j]);
			++j;
		}

		for (int i = s; i <= e; ++i)
			v[i] = temp[i - s];

	}


	void MergeSort(std::vector<int>& v, int s, int e)
	{
		if (s < e) {
			int m = (s + e) / 2;
			MergeSort(v, s, m);
			MergeSort(v, m + 1, e);
			MergeSortedIntervals(v, s, m, e);
		}
	}

};
