#pragma once
#include "parser.h"
#include "lexer.h"
#include "error.h"
#include "instr.h"
#include "program.h"
#include <string_view>
#include <fstream>
#include <filesystem>
#include <iostream>
inline void Compile(std::string_view filePath,Program& prog,std::vector<Instr>* bin) {
		std::filesystem::path p(filePath);
		if (p.extension().string() != ".nmath") {
			std::cout << "I can only open files with the .nmath extension" << std::endl;
			std::cin.get();
			std::exit(1);
		}
		std::ifstream file(p);
		if (!file.is_open()) {
			Error("I can't open the file");
		}
		std::string source((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
		Lexer lexer(source);
		lexer.Tokenize();
		Parser parser(lexer.tokens, prog);
		parser.Parse();
		*bin = std::move(parser.GetBin());
		if (parser.isDebug) {
			for (auto& instr : *bin) {
				std::cout << OpCodeToString(instr.op);
				std::cout << " " << instr.a;
				std::cout << " " << instr.b;
				std::cout << " " << instr.c << '\n';
			}
		}
	}