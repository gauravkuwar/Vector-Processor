// unordered_map<string, array<char, 3>> all_instrs_set = {
// 	{"PACKLO", {'V','V','V'}},
// 	{"SUBVV", {'V','V','V'}},
// 	{"MULVV", {'V','V','V'}},
// 	{"ADDVV", {'V','V','V'}},
// 	{"PACKHI", {'V','V','V'}},
// 	{"UNPACKHI", {'V','V','V'}},
// 	{"UNPACKLO", {'V','V','V'}},
// 	{"DIVVV", {'V','V','V'}},
// 	{"DIVVS", {'V','V','S'}},
// 	{"MULVS", {'V','V','S'}},
// 	{"SUBVS", {'V','V','S'}},
// 	{"ADDVS", {'V','V','S'}},
// 	{"SLEVV", {'V','V','N'}},
// 	{"SGTVV", {'V','V','N'}},
// 	{"SEQVV", {'V','V','N'}},
// 	{"SLTVV", {'V','V','N'}},
// 	{"SGEVV", {'V','V','N'}},
// 	{"SNEVV", {'V','V','N'}},
// 	{"SGTVS", {'V','S','N'}},
// 	{"SLEVS", {'V','S','N'}},
// 	{"LV", {'V','S','N'}},
// 	{"SNEVS", {'V','S','N'}},
// 	{"SLTVS", {'V','S','N'}},
// 	{"SEQVS", {'V','S','N'}},
// 	{"SV", {'V','S','N'}},
// 	{"SGEVS", {'V','S','N'}},
// 	{"POP", {'S','N','N'}},
// 	{"MFCL", {'S','N','N'}},
// 	{"MTCL", {'S','N','N'}},
// 	{"SVWS", {'V','S','S'}},
// 	{"LVWS", {'V','S','S'}},
// 	{"LVI", {'V','S','V'}},
// 	{"SVI", {'V','S','V'}},
// 	{"BGT", {'S','S','I'}},
// 	{"BLE", {'S','S','I'}},
// 	{"BLT", {'S','S','I'}},
// 	{"LS", {'S','S','I'}},
// 	{"BGE", {'S','S','I'}},
// 	{"BNE", {'S','S','I'}},
// 	{"SS", {'S','S','I'}},
// 	{"BEQ", {'S','S','I'}},
// 	{"SUB", {'S','S','S'}},
// 	{"OR", {'S','S','S'}},
// 	{"AND", {'S','S','S'}},
// 	{"SRA", {'S','S','S'}},
// 	{"ADD", {'S','S','S'}},
// 	{"SRL", {'S','S','S'}},
// 	{"SLL", {'S','S','S'}},
// 	{"XOR", {'S','S','S'}},
// 	{"CVM", {'N','N','N'}},
// 	{"HALT", {'N','N','N'}},
// };

// void verify(vector<string> instrs) {
// 	for (const string& instr : instrs) {
// 		// instr exists in set of all instructions
// 		cout << instr << ":" << all_instrs_set.count(instr) << endl;
// 		if (all_instrs_set.count(instr) < 1) {
// 			cout << "Instr not valid" << endl;
// 		}
// 	}
// 	// instr exists in all
// 	// correct num of parameters
// 	// correct type of parameter (vector/scalar)
// }
