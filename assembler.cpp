#include <iostream>
#include <fstream>
#include <map>
#include <bitset>
#include <string>

std::string get_destination(const std::string& instruction){
	int position = instruction.find('=');
	if (position != std::string::npos) {
		return instruction.substr(0, position);
	}
	return "";
}

std::string get_computation(const std::string& instruction) {
	int start_position = instruction.find("=");
	int end_position = instruction.find(";");
	if (start_position != std::string::npos && end_position != std::string::npos) {
		return instruction.substr(start_position + 1, end_position - start_position - 1);
	}
	if (start_position == std::string::npos) {
		return instruction.substr(0, end_position);
	}
	if (end_position == std::string::npos) {
		return instruction.substr(start_position + 1, instruction.size() - start_position - 1);
	}
	return "ERROR";
}

std::string get_jump(const std::string& instruction){
	int start_position = instruction.find(";") + 1;
	if (start_position != std::string::npos) {
		return instruction.substr(start_position, 3);
	}
	return "";
}

int main() {
	//Edit to be path to your files from working directory
	const std::string path = "./Desktop/proj_euler/";
	std::cout << "Assuming path is " << path << std::endl << "Enter file name to assemble: ";
	std::string inFileName;
	std::cin >> inFileName;

	std::ifstream file(path + inFileName);

	if(!file.is_open()) {
		std::cerr << "Error, could not find file; ensure you have read the readme.md" << std::endl;
		return 1;		
	}

	//First sweep to check for tokens
	std::map<std::string, int> tokens;
	std::pair<std::string, int> data_tokens[23] = {
		std::make_pair("SP", 0),
		std::make_pair("LCL", 1),
		std::make_pair("ARG", 2),
		std::make_pair("THIS", 3),
		std::make_pair("THAT", 4),
		std::make_pair("SCREEN", 16384),
		std::make_pair("KBD", 24576)
	};

	for (int i = 0; i<16; ++i){
		data_tokens[i + 7] = std::make_pair("R" + std::to_string(i), i);
	}

	for (int i = 0; i<23; ++i){
		tokens.insert(data_tokens[i]);
	}

	std::string line;
	int line_number = 0;
	while(file >> line){
		//if A-instruction
		if(line.at(0) == '(' && !isdigit(line.at(1))){
			//if token
			line.erase(0, 1);
			line.erase(line.size() - 1);

			tokens.insert(make_pair(line, line_number));
			line_number--;
		}
		if(line.at(0) == '/'){
			line_number--;
			file.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
		}
		line_number++;
	}

	//Second sweep to translate instructions
	std::map<std::string, std::string> jumps;
	std::pair<std::string, std::string> data_jumps[] = {
		std::make_pair("JGT", "001"),
		std::make_pair("JEQ", "010"),
		std::make_pair("JGE", "011"),
		std::make_pair("JLT", "100"),
		std::make_pair("JNE", "101"),
		std::make_pair("JLE", "110"),
		std::make_pair("JMP", "111")
	};

	for(int i=0; i<7; ++i){
		jumps.insert(data_jumps[i]);
	}

	std::map<std::string, std::string> computations;
	std::pair<std::string, std::string> data[] = {
		std::make_pair("0", "0101010"),
		std::make_pair("1", "0111111"),
		std::make_pair("-1", "0111010"),
		std::make_pair("D", "0001100"),
		std::make_pair("A", "0110000"),
		std::make_pair("M", "1110000"),
		std::make_pair("!D", "0001101"),
		std::make_pair("!A", "0110001"),
		std::make_pair("!M", "1110001"),
		std::make_pair("-D", "0001111"),
		std::make_pair("-A", "0110011"),
		std::make_pair("-M", "1110011"),
		std::make_pair("D+1", "0011111"),
		std::make_pair("A+1", "0110111"),
		std::make_pair("M+1", "1110111"),
		std::make_pair("D-1", "0001110"),
		std::make_pair("A-1", "0110010"),
		std::make_pair("M-1", "1110010"),
		std::make_pair("D+A", "0000010"),
		std::make_pair("D+M", "1000010"),
		std::make_pair("D-A", "0010011"),
		std::make_pair("D-M", "1010011"),
		std::make_pair("A-D", "0000111"),
		std::make_pair("M-D", "1000111"),
		std::make_pair("D&A", "0000000"),
		std::make_pair("D&M", "1000000"),
		std::make_pair("D|A", "0010101"),
		std::make_pair("D|M", "1010101")
	};

	for (int i = 0; i < 28; ++i) {
    		computations.insert(data[i]);
	}

	file.clear();
	file.seekg(0);
	int minimum_variable_memory_address = 16;
	while(file >> line){
		if(line.at(0) == '/') {
			file.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
		} else {

		if(line.at(0) == '@'){
			//if A-instruction
			line.erase(0,1);
			if(isdigit(line.at(0))){
				std::cout << '0' << std::bitset<15>(std::stoi(line)) << std::endl;	
			} else {
				if(tokens.count(line) > 0){
					std::cout << '0' << std::bitset<15>(tokens[line]) << std::endl;
				} else {
					std::cout << '0' << std::bitset<15>(minimum_variable_memory_address) << std::endl;
					tokens.insert(make_pair(line, minimum_variable_memory_address));
					minimum_variable_memory_address++;
				}
			}
		} else if (line.at(0) != '(') {
			std::string instruction = "111";			
			const std::string computation = get_computation(line);
			instruction += computations[computation];

			const std::string destination = get_destination(line);
			instruction += destination.find('A') != std::string::npos ? '1' : '0';
			instruction += destination.find('D') != std::string::npos ? '1' : '0';
			instruction += destination.find('M') != std::string::npos ? '1' : '0';

			if(line.find(";") != std::string::npos){
				const std::string jump = get_jump(line);
				instruction += jumps[jump];
			} else {
				instruction += "000";
			}

			std::cout << instruction << std::endl;	
		}}
	}

	return 0;
}
