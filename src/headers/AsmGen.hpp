#pragma once
#include "IR.hpp"
#include <cstdio>
#include <fstream>
#include <ostream>
#include <string>
#include <vector>

class AsmGen {
public:
  AsmGen(std::vector<IRInst> &instructions) {

    for (IRInst &inst : instructions) {
      if (inst.s_Type == "INT_DECL" || inst.s_Type == "FLT_DECL" ||
          inst.s_Type == "STR_DECL" || inst.s_Type == "BUL_DECL") {
        Decs.push_back(inst);
      } else if (inst.s_Type == "RET") {
        Rets.push_back(inst);
      }
    }
  }

  void generateASM() {
    std::string define_section = "SYS_EXIT equ 1\nSYS_READ equ 3\nSYS_WRITE "
                                 "equ 4\nSTDIN equ 0\nSTDOUT equ 1\n\n";
    std::string text_section =
        "section .text\n\tglobal _start\n\n_start:\n\tjmp main\n\n\n";
    std::string exit_section = "exit:\n";

    std::string print_section =
        "%macro put 2\n\tmov eax, SYS_WRITE\n\tmov ebx, STDOUT\n\tmov ecx, "
        "%1\n\tmov edx, %2\n\tint 0x80\n%endmacro\n\n";

    std::string data_section = "section .data\n";

    std::string main_section = "main:\n";

    for (IRInst &inst : Decs) {
      if (inst.s_Type == "STR_DECL") {
        std::string sect = "\t" + inst.s_Operand + " db '" + inst.s_Operation +
                           "', 0xA\n\t" + inst.s_Operand + "_len equ $ - " +
                           inst.s_Operand + "\n";
        std::string sect_m =
            "\tput " + inst.s_Operand + ", " + inst.s_Operand + "_len\n";
        main_section += sect_m;
        data_section += sect;
      }
      if (inst.s_Type == "INT_DECL") {
        std::string sect =
            "\t" + inst.s_Operand + " db '" + inst.s_Operation + "'\n";
        std::string sect_m =
            "\tput " + inst.s_Operand + ", 2\n"; // auto mactic print
        main_section += sect_m;

        data_section += sect;
      }
      if (inst.s_Type == "BUL_DECL") {
        std::string sect =
            "\t" + inst.s_Operand + " db '" + inst.s_Operation + "'\n";

        std::string sect_m =
            "\tput " + inst.s_Operand + ", " + "1\n"; // auto mactic print
                                                      //
        main_section += sect_m;
        data_section += sect;
      }
      if (inst.s_Type == "FLT_DECL") {
        std::string sect =
            "\t" + inst.s_Operand + " db '" + inst.s_Operation + "'\n";

        std::string sect_m =
            "\tput " + inst.s_Operand + ", 2\n"; // auto mactic print
        main_section += sect_m;

        data_section += sect;
      }
    }

    for (IRInst &inst : Rets) {
      if (inst.s_Type == "RET") {
        std::string sect = "\tmov eax, " + inst.s_Operation + "\n\tint 0x80\n\n";
        exit_section += sect;
      }
    }
    main_section += "\tjmp exit\n";

    std::ofstream file("Assembly.asm");
    printf("%s\n", define_section.c_str());

    file << define_section;
    printf("%s\n", print_section.c_str());
    file << print_section;
    printf("%s\n", text_section.c_str());
    file << text_section;
    printf("%s\n", main_section.c_str());
    file << main_section;
    printf("%s\n", exit_section.c_str());
    file << exit_section;
    printf("%s\n", data_section.c_str());
    file << data_section;
  }

private:
  std::vector<IRInst> Decs;
  std::vector<IRInst> Rets;
};
