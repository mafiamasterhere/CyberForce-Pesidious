#include <iostream>
#include <fstream>
#include <cstdlib>

#include "../portable-executable-library2/pe_lib/pe_bliss.h"
//#ifdef PE_BLISS_WINDOWS
#include "../portable-executable-library2/samples/lib.h"
//#endif

using namespace pe_bliss;

//??????, ????????????, ??? ???????? ????? ?????? ? ??????? ??????? PE-?????
int main(int argc, char* argv[])
{
	if(argc < 2)
	{
		std::cout << "Usage: import_adder.exe PE_FILE" << std::endl;
		return 0;
	}

	//????????? ????
	std::ifstream pe_file(argv[1], std::ios::in | std::ios::binary);
	if(!pe_file)
	{
		std::cout << "Cannot open " << argv[1] << std::endl;
		return -1;
	}

	std::ifstream infile(argv[2]);
	//,mbnnnnnnint x = atoi(argv[4]);

	try
	{
		//??????? ????????? PE ??? PE+ ?????? ? ??????? ???????
		pe_base image(pe_factory::create_pe(pe_file));

		//??????? ?????? ????????????? ????????? ? ???????
		imported_functions_list imports(get_imported_functions(image));

		//???????? ????? ??????????, ?? ??????? ????? ????????????? ???????
		import_library new_lib;
		 //????? ??? ????? kaimi_dx.dll

		//??????? ? ??? ???? ???????? ???????

		std::string line;
		int iat = 0x1;
		int i = 0;
		std::getline(infile, line);
		new_lib.set_name(line);
		int total_dll = 0;
		while(std::getline(infile, line) && total_dll < 25)
		{
            if(line.find(".dll") != std::string::npos)
            {
                imports.push_back(new_lib);
                new_lib.clear_imports();
                new_lib.set_name(line);
                iat = 0x1;
                total_dll++;


            }

            else
            {
                imported_function func;
                func.set_name(line); //???? ?????? - ?? ????? Tralala
                func.set_iat_va(iat); //??????? ????????? ?????????? ????? ? import address table
                iat++;
                new_lib.add_import(func);
            }

		}


		imports.push_back(new_lib); //??????? ??????????????? ?????????? ? ????????

		//????? ????????????? ? ???????????? ???????

		//?? ?? ?????? ??????????? ??????? ????????
		//??? ????? ????? ??????? ??????, ??? ?? ?????? ??????????????,
		//??????? ??????? ?? ? ????? ??????, ????? ??? ???????????
		//(?? ?? ????? ????????? ???????????? ??????, ???? ?????? ?????? ?? ? ????? ????? ?????)
		section new_imports;
		new_imports.get_raw_data().resize(1); //?? ?? ????? ????????? ?????? ??????, ??????? ????? ? ??? ????? ????????? ?????? ?????? 1
		new_imports.set_name("new_imp"); //??? ??????
		new_imports.readable(true).writeable(true); //???????? ?? ?????? ? ??????
		section& attached_section = image.add_section(new_imports); //??????? ?????? ? ??????? ?????? ?? ??????????? ?????? ? ????????????? ?????????

		//?????????, ?????????? ?? ????????? ???????????? ????????
		import_rebuilder_settings settings(true, false); //???????????? ????????? PE ? ?? ??????? ???? IMAGE_DIRECTORY_ENTRY_IAT
		rebuild_imports(image, imports, attached_section, settings); //???????????? ???????

		//??????? ????? PE-????
		std::string base_file_name(argv[1]);
		std::string::size_type slash_pos;
		if((slash_pos = base_file_name.find_last_of("/\\")) != std::string::npos)
			base_file_name = base_file_name.substr(slash_pos + 1);

        std::string modified(argv[3]);

		base_file_name = modified;
		std::ofstream new_pe_file(base_file_name.c_str(), std::ios::out | std::ios::binary | std::ios::trunc);
		if(!new_pe_file)
		{
			std::cout << "Cannot create " << base_file_name << std::endl;
			return -1;
		}

		//???????????? PE-????
		rebuild_pe(image, new_pe_file, true, true);

		//std::cout << "PE was rebuilt and saved to " << base_file_name << std::endl;
	}
	catch(const pe_exception& e)
	{
		//???? ???????? ??????
		std::cout << "Error: " << e.what() << std::endl;
		return -1;
	}

	return 0;
}
