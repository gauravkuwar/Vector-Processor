#include <iostream> 
#include <array>
#include <unordered_map> 
#include <fstream>
using namespace std; 

// 32 bit words
#define MAX_VEC_LEN 64
#define REG_BITS 32
#define REG_COUNT 8
#define VDMEM_SIZE 128000 // 512 KB -> 512000 bytes -> 4096000 bits / 32 bits -> 128000 words
#define SDMEM_SIZE 8000 // 32 KB -> 32000 bytes -> 256000 bits / 32 bits -> 8000 words
#define MAX_INSTR_PARTS 4

#define ASM_FN "Code.asm"
#define SDMEM_FN "SDMEM.txt"
#define VDMEM_FN "VDMEM.txt"

// trim from end (in place)
inline void rtrim(std::string &s) {
    s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
        return !std::isspace(ch);
    }).base(), s.end());
}

struct Instruction {
	string instr_name;
	string op1;
	string op2;
	string op3;
};


void get_instrs(string filepath, vector<Instruction>* instrs) {
	string tmp_line;
	ifstream asm_file(filepath);

	string comments_removed;
	string instr_parts[MAX_INSTR_PARTS];
	int instr_parts_idx;
	int idx;
	int prev_idx;

	while (getline (asm_file, tmp_line)) {
		// filter out lines which are empty or have comments
		if (tmp_line[0] != '#' && tmp_line != "") {
			comments_removed = tmp_line.substr(0, tmp_line.find("#")); // remove inline comments
			rtrim(comments_removed); // remove whitespace from  end of string
			
			instr_parts_idx = 0;
			prev_idx = 0;
			idx = 0;
			memset(instr_parts, 0, MAX_INSTR_PARTS * sizeof(instr_parts[0])); 
			
			// separate each instruction part into instr_parts array
			while (idx < (int)comments_removed.size() && instr_parts_idx < 4) {
				if (comments_removed[idx] == ' ') {
					instr_parts[instr_parts_idx++] = comments_removed.substr(prev_idx, idx-prev_idx);
					prev_idx = idx + 1;
				}

				idx++;
			}

			instr_parts[instr_parts_idx] = comments_removed.substr(prev_idx, idx-prev_idx);

			Instruction tmp_instr = {
				instr_parts[0],
				instr_parts[1],
				instr_parts[2],
				instr_parts[3]
			};

			instrs->push_back(tmp_instr);
		}
	}
		
	asm_file.close();
}

void init_mem(string filepath, int mem_array[]) {
	// load memory data from file into memory arrays
	string tmp_line;
	ifstream mem_file(filepath);
	int idx = 0;

	while (getline(mem_file, tmp_line))
		mem_array[idx++] = stoi(tmp_line);
}

int reg_int(string op) {
	return stoi(op.substr(2));
}

class FuncSimulator {
	private:
		vector<Instruction> instrs;

		int sdmem[SDMEM_SIZE]; // scalar memory
		int vdmem[VDMEM_SIZE]; // vector memory

		int vreg[REG_COUNT][MAX_VEC_LEN]; // vector registers
		int sreg[REG_COUNT]; // scalar registers

		int vec_len = MAX_VEC_LEN; // current vector length register
		int vec_mask[MAX_VEC_LEN]; // vector mask register - consists of MAX_VEC_LEN 1 bit data

		void cvm() {
			// clear vector mask
			fill_n(vec_mask, MAX_VEC_LEN, 1); // set all bits of vector mask to 1
		}

		int pop() {
			// count num of 1 bits in vector mask
			int count = 0;

			for (int i = 0; i < MAX_VEC_LEN; i++)
				count += vec_mask[i];
				
			return count;
		}

		void alu_op() {

		}

	public:
		FuncSimulator(string base_fp) {
			cvm();
			get_instrs(base_fp + ASM_FN, &instrs); // get and format instructions from Code.asm file
		}

		void run() {
			int PC = 0; // program counter
			
			// process instructions
			while (instrs[PC].instr_name != "HALT") {
				cout << instrs[PC].instr_name << " " << instrs[PC].op1 << " " << instrs[PC].op2 << " " << instrs[PC].op3 << endl;

				// --- Vector Mask Register Operations ---
				// Clear the Vector Mask Register - set all bits to 1.
				if (instrs[PC].instr_name == "CVM")
					cvm();
				
				// Count the number of 1s in the Vector Mask Register and store the scalar value in SR1.
				if (instrs[PC].instr_name == "POP")
					sreg[reg_int(instrs[PC].op1)] = pop();


				// --- Vector Length Register Operations ---
				// // Move the contents of the Scalar Register SR1 into the Vector Length Register.
				if (instrs[PC].instr_name == "MTCL")
					vec_len = sreg[reg_int(instrs[PC].op1)];

				// Move the contents of the Vector Length Register into the Scalar Register SR1.
				if (instrs[PC].instr_name == "MFCL")
					sreg[reg_int(instrs[PC].op1)] = vec_len;

				// --- Vector Operations ---
				// ADD/SUBVV, ADD/SUBVS, MUL/DIVVV, MUL/DIVVS

				// --- Scalar Operations ---
				// ADD/SUB, AND/OR/XOR, SLL, SRL, SRA
				
				// --- Memory Access Operations ---
				// LV, SV, LVWS, SVWS, LVI, SVI, LS, SS

				// --- Control ---
				// B__

				// --- Register-Register Shuffle ---
				// UNPACKLO, UNPACKHI, PACKLO, PACKHI

				PC++;
			}
		}
};


int main(int argc, char *argv[]) {
	string base_fp = argv[1]; // "../../dot_product/";
	FuncSimulator func_sim(base_fp);
	func_sim.run();
	return 0;
}
