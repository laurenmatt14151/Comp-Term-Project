#include <cstdlib>
#include <string.h>
#include <iostream>
#include <fstream>
#include <iomanip>
using namespace std;

//----------------------------------------------------------------------------
// File name:    halix11.cpp
// Purpose:      HALIX emulator - (student version).
// Input file:   progname.hlx
// Output files: progname.hlt -- execution trace showing ACC, I/O, IP;  
//                               DM dump; IM dump. 
// Author:       cda3101joe Dr. Jones
// Date:         2010-18
//----------------------------------------------------------------------------
const int IMSIZE = 100;
const int DMSIZE = 100;
const int MAXOPCODE = 33;

string Instr[35] = {"NOOP  ","READ  ","WRITE ","LOAD  ","STORE ","ADD   ","SUB   ",
                    "MULT  ","DIV   ","MOD   ","BRANCH","BRT   ","BRF   ",
                    "CLEAR ","SET   ","DOUBLE","INCR  ","DECR  ","CLT   ",
                    "CLE   ","CEQ   ","CNE   ","CGE   ","CGT   ",
                    "SETI  ","ADDI  ","SUBI  ","MULTI ","DIVI  ","MODI  ",
                    "POW   ","SHACC ","BSUB  ","RET   "
                   };
// -----------------------------------------------------
//-| Decode instruction into opcode and operand.
// -----------------------------------------------------
void Decode (int inst, int & opcode, int & operand)
{
  opcode = inst / 100;
  operand = inst % 100;
}//Decode


// -----------------------------------------------------
//Display contents of instruction memory.
// -----------------------------------------------------
void Show_code(int IM[], int codeSize)
{
  int i, opcode, addr;       

  cout << endl << "ADDR  IMem" << endl;
  cout << "====  ====" << endl;
  for(i=0; i < codeSize; i++)
  {
      cout << "  " << setw(2) << setfill('0') << i 
           << "  " << setw(4) << setfill('0') << IM[i];

      opcode = IM[i] / 100;
      addr = IM[i] % 100;
      if (opcode == 99)
         cout << "  HALT" << endl;
      else if ((opcode >= 1) && (opcode <= MAXOPCODE))
         cout << "  " << Instr[opcode] << " " 
              << setw(2) << setfill('0') << addr << endl;
      else
         cout << "**ILLEGAL" << endl;
      

      //printf("%4.4d %5.4d \n", i, IM[i]);
  }

}//Show_code


// -----------------------------------------------------
//Display contents of data memory.
void Show_data(int DM[], int dataSize)
{
  int i;       
  
  cout << endl << "ADDR  DMem" << endl;
  cout << "====  ====" << endl;
  for(i=0; i < dataSize; i++)
  {
      cout << "  " << setw(2) << setfill('0') << i 
           << "  " << setw(4) << setfill('0') << DM[i] << endl;
      //cout << i << " : " << DM[i] << endl;
      //printf("%4.4d %5.4d \n", i, DM[i]);
  }
}//Show_data

// -----------------------------------------------------
//Display contents of instruction memory.
// -----------------------------------------------------
void Save_code(ostream & outF, int IM[], int codeSize)
{
  int i, opcode, addr;       

  outF << endl << "INSTRUCTION MEMORY DUMP" << endl;
  outF << endl << "iADR  IMem  OPRTN  OPND" << endl;
  outF <<         "====  ====  ====== ====" << endl;
  for(i=0; i < codeSize; i++)
  {
      outF << "  " << setw(2) << setfill('0') << i 
           << "  " << setw(4) << setfill('0') << IM[i];

      opcode = IM[i] / 100;
      addr = IM[i] % 100;
      if (opcode == 99)
         outF << "  HALT" << endl;
      else if ((opcode >= 1) && (opcode <= MAXOPCODE))
         outF << "  " << Instr[opcode] << " " 
              << setw(2) << setfill('0') << addr << endl;
      else
         outF << "**ILLEGAL" << endl;
      
      //fprintf(outF, "%4.4d %5.4d \n", i, IM[i]);
  }

}//Save_code


// -----------------------------------------------------
//Display contents of data memory.
void Save_data(ostream & outF, int DM[], int dataSize)
{
  int i;       
  
  outF << endl;
  outF << endl << "DATA MEMORY DUMP" << endl;
  outF << endl << "ADDR  DMem" << endl;
  outF << "====  ====" << endl;
  for(i=0; i < dataSize; i++)
  {
      outF << "  " << setw(2) << setfill('0') << i << "  " 
           << setw(4) << setfill('0') << DM[i] << endl;
      //outF << i << " : " << DM[i] << endl;
      //printf("%4.4d %5.4d \n", i, DM[i]);
  }
}//Save_data

// -----------------------------------------------------
// Load instructions from code file into Instruction Memory. 
// -----------------------------------------------------
void Load_code(istream & codeF, int IM[], int & codeSize)
{
  int i = 0;       // Address in IM.
   
  codeF >> codeSize;
  if (codeF.fail()) 
  {
     codeSize = 0;
     cout << endl << "WARNING: EMPTY code file." << endl;
  }
  while ( (i < codeSize)  && (codeF >> IM[i]) )
  {
     i++; 
  }
  for (i = codeSize; i < 50; i++)
      IM[i] = 9900;
}//Load_code

// -----------------------------------------------------
// Load data from code file into Data Memory. Set default
//      data segment to 10 words.
// -----------------------------------------------------
void Load_data(istream & dataF, int DM[], int & dataSize)
{
  int i = 0;       // Address in DM.
   
  dataF >> dataSize;
  if (dataF.fail()) 
  {
     dataSize = 10;
     for (i = 0; i < dataSize; i++) 
         DM[i] = 0;
     cout << endl << "WARNING: EMPTY data file." << endl;
  }
  else
  while ( (i < dataSize) && (dataF >> DM[i]) )
  {
     i++; 
  }
}//Load_data


int main(int argc, char *argv[])
//int main()
{
  // Declarations
  int IM[200];  // Instruction Memory.
  int DM[50];   // Data Memory.
  int IP;       // Instruction Pointer.
  int IR;       // Instruction Register.
  int RR;       // Subroutine Return Register.
  int ACC;      // Accumulator.
  int opcode;   // Operation Code.
  int addr;     // Address of Value.
  int codeSize; // Number of instructions.
  int dataSize; // Number of data items.

  int MAX_INSTRUCTIONS = 2000; // Guard against infinite loops.
  int Instr_Count = 0;         // Count each instruction.

  char codeFname[40];   // Name of file containing machine code.
  ifstream codeF;       // Executable code input file.
  char traceFname[40];  // Name of file containing execution trace.
  ofstream traceF;      // Executable trace output file.
  char UNIXcmd[40];     // Unix system call argument.

  cout << endl;
  cout << "+----------------------------------------+" << endl;
  cout << "| HALIX version 1.7, (c) E. Jones, 2011. |" << endl;
  cout << "+----------------------------------------+" << endl;

  //-| -----------------------------------------------------------------
  //-| Loader: Load executable program into the computer memory (IM+DM).
  //-| -----------------------------------------------------------------
  if(argc == 2)
     strcpy (codeFname, argv[1]);
  else
  {
     cout << "Enter name of code file: ";
     cin >> codeFname;
  }
  strcpy (traceFname, codeFname);
  traceFname[strlen(codeFname)-1] = 't';

  codeF.open(codeFname);
  if (codeF.fail())
  {
     cout << "Code file '" << codeFname << "' NOT FOUND - FATAL." << endl;
     exit (1);
  }

  //-| -----------------------------------------------------------------
  //-| Loader: Load executable program into the computer memory (IM+DM).
  //-| -----------------------------------------------------------------
  Load_code(codeF, IM, codeSize);
  Load_data(codeF, DM, dataSize);
  codeF.close();

  //-| -----------------------------------------------------------------
  //-| Display executable program in the computer memory (IM+DM).
  //-| -----------------------------------------------------------------
  Show_code(IM, codeSize);
  Show_data(DM, dataSize);
 

  //-| -----------------------------------------------------------------
  //-| EXECUTION CYCLE: Fetch ==> Decode ==> Execute 
  //-| -----------------------------------------------------------------
  traceF.open(traceFname);
  Save_code(traceF, IM, codeSize);
  Save_data(traceF, DM, dataSize);
  traceF << endl << endl;
  traceF << "*********************" << endl;
  traceF << "HALIX EXECUTION TRACE" << endl;
  traceF << "(c) 2019, yourlogin First Last" << endl;
  traceF << "Program: " << codeFname << endl;
  traceF << "*********************" << endl << endl;
  traceF << "iADR  INST  OPRTN   OPND  RESULT" << endl; 
  traceF << "----  ----  -----   ----  ------" << endl; 
  IP = 0;
  traceF << "  " << setw(2) << IP << "  ";

  IR = IM[IP++];
  Decode(IR, opcode, addr);

  while (opcode != 99 && Instr_Count < MAX_INSTRUCTIONS)
  {
     traceF << setw(4) << setfill('0') << IR << "  ";

     Instr_Count++;

     //-| ---------------------------------
     //-| Execute decoded instruction. 
     //-| ---------------------------------
     switch (opcode)
     {
       case 01 : // READ
              cout << "ENTER NUMBER: ";
              cin >> DM[addr];
              traceF << Instr[01] << "    " << setw(2) << addr 
                     << " <==" << setw(4) << DM[addr] << endl;
              break;
     
       case 02 : // WRITE
              cout << "Data Value: " << DM[addr] << endl;
              traceF << Instr[02] << "    " << setw(2) << addr << " ==>"
                     << setw(4) << DM[addr] << endl;
              break;
   
       case 03 : // LOAD
              ACC = DM[addr];
              traceF << Instr[03] << "    " << setw(2) << addr << "    "
                     << setw(4) << ACC << endl;
              break;
   
       case 04 : // STORE
              DM[addr] = ACC;
              traceF << Instr[04] << "    " << setw(2) << addr << "    "
                     << setw(4) << DM[addr] << endl;
              break;
   
       case 05 : // ADD
              ACC = ACC + DM[addr];
              traceF << Instr[05] << "    " << setw(2) << addr << "    "
                     << setw(4) << ACC << endl;
              break;
      
       case 10 : // BRANCH
              traceF << Instr[10] << "    " << setw(2) << addr << "    ";
              IP = addr;
              traceF << setw(2) << IP << endl;
              break;
   
   
       case 23 : // CGT - Compare for greater-than  
              ACC = (ACC > DM[addr]);
              traceF << Instr[23] << "    " << setw(2) << addr << "    "
                     << setw(4) << ACC << endl;
              break;
   

       case 25 : // ADDI - add immediate value to ACC.
              ACC = ACC + addr;
              traceF << Instr[25] << "    " << setw(2) << addr << "    "
                     << setw(4) << ACC << endl;
              break;
   
       default: 
              traceF << "BADOP   " << setw(2) << addr << "  " 
                     <<  " ILLEGAL opcode " << endl;
              cout << "ILLEGAL opcode (" << opcode << ")." << endl; 
              break;
   
 
     }//switch    

     traceF << "  " << setw(2) << IP << "  ";

     //-| ---------------------------------
     //-| Fetch and decode next instruction.
     //-| ---------------------------------
     IR = IM[IP++];
     Decode(IR, opcode, addr);

  }//cycle 

  cout << endl << endl << "EXECUTION TERMINATING at address " 
       << IP << "... " << endl;
  cout << Instr_Count << " INSTRUCTIONS EXECUTED." << endl;

  traceF << setw(2) << IR << "  ";
  traceF << "HALT " << endl;
  traceF << endl;
  traceF << "----  ----  -----   ----  ------" << endl; 
  traceF << "ADDR  INST  OPRTN   OPND  RESULT" << endl; 
  traceF << endl;
  traceF << "***************************" << endl;
  traceF << "END - HALIX EXECUTION TRACE" << endl;
  traceF << "Program: " << codeFname << endl;
  traceF << Instr_Count << " INSTRUCTIONS EXECUTED. " << endl;
  traceF << "***************************" << endl;

  //Save_code(traceF, IM, codeSize);
  Save_data(traceF, DM, dataSize);

  traceF.close();

  cout << endl << endl << endl;
  cout << "-------------------------------------------" << endl;
  cout << "HALIX EXECUTION TRACE FILE - " << traceFname << endl;
  cout << "-------------------------------------------" << endl;


  strcpy(UNIXcmd,"cat ");
  strcat(UNIXcmd,traceFname);
  system(UNIXcmd);

  //  cout << endl << endl;
  //  cout << "--------------------------------------------------------------" << endl;
  //  cout << "Be sure to submit both files, " << codeFname << " and " 
  //       << traceFname << "." << endl;
  //  cout << "--------------------------------------------------------------" << endl;
  //  cout << endl;
  
  return 0;
}
