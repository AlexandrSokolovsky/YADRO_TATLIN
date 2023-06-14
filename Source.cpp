#define _CRT_SECURE_NO_WARNINGS

#include "TapeController.h"
#include "TapeSort.h"

#include <boost/program_options.hpp>

namespace opt = boost::program_options;



int main(int argc, char* argv[])
{
	//parse cmd
	std::string cfg_file_name, tape_in_filename, tape_out_filename;

	opt::options_description desc("All Options");
	desc.add_options()
		("in", opt::value<std::string>(), "tape_in_filename")
		("out", opt::value<std::string>(), "tape_out_filename")
		("cfg", opt::value<std::string>(), "(additional option) cfg_filename")
		("help", "produce help message");

	opt::variables_map vm;
	opt::store(opt::parse_command_line(argc, argv, desc), vm);
	opt::notify(vm);

	if (vm.count("-help"))
	{
		std::cout << desc << std::endl;
		return -1;
	}

	if (!vm.empty())
	{		
		if (!vm["in"].empty() && !vm["out"].empty())
		{
			tape_in_filename = vm["in"].as<std::string>();
			tape_out_filename = vm["out"].as<std::string>();
		}
		else
		{
			std::cout << desc << std::endl;
			return -1;
		}
			

		if(!vm["cfg"].empty())
			cfg_file_name = vm["cfg"].as<std::string>();
		else
			cfg_file_name = "cfg.json";
	}



	//read cfg and setup
	TapeController cntrl(cfg_file_name);

	cntrl.Tape_in_name = tape_in_filename;
	cntrl.Tape_out_name = tape_out_filename;

	//Sort input Tape and write data to the output Tape
	TapeSort TapeSorter;
	TapeSorter.Sort(cntrl);

	return 0;
}









