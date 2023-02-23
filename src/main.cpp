#include <fstream>
#include <iostream>
#include <istream>
#include <ostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <string>
#include <unordered_map>
#include <vector>

#define ISARR(a) a & 0x80

using std::fstream;
using std::ifstream;
using std::ofstream;
using std::string;
using std::unordered_map;
using std::vector;

class VarType {
public:
  enum types {
    VOID,
    INT64,
    // INT32,
    // INT16,
    // INT8,
    UNSIGNED64,
    // UNSIGNED32,
    // UNSIGNED16,
    // UNSIGNED8,
    DOUBLE,
    // FLOAT,
    CHAR,
    STRING,
    LABEL,
  };
};
// ARRAYS ARE VARIABLE TYPE & 0x80

// typedef union {
//   long long integer;
//   double double_ver;
// } data;

// typedef struct var {
//   int type;
//   data stored_value;
// } var;
// typedef struct arr {
//   int type;
//   void *ptr;
//   int length;
// } arr;

// typedef union {
//   var var_rep;
//   arr arr_rep;
// } variable;

class Variable {
public:
  int type;
};
class Integer : public Variable {
public:
  int data;
	Integer(){
		
	}
	Integer(int type, int data){
		this->type = type;
		this->data = data;
	}
};
class Double : public Variable {
public:
  double data;
	Double(int type, double data){
		this->type = type;
		this->data = data;
	}
};
class String : public Variable {
public:
  string data;
	String(int type, string data){
		this->type = type;
		this->data = data;
	}
};
class Array : public Variable {
  void *ptr;
  int length;
	Array(int type, void *data, int length){
		this->type = type;
		this->ptr = data;
		this->length = length;
	}
};

class TokenType {
public:
  enum types {
    KEYWORD,
		END,
    IDENTIFIER,
    OPERATOR,
    STRING,
    // WHITESPACE,
    ARRAY,
    // FUNCTION,
    NUMBER,
    COMMENT,
    CONSTANT,
  };
};
string token_type_str[] = {
    "KEYWORD", //done
		"END",
		"IDENTIFIER",//done
    "OPERATOR", // done
    "STRING",   // done
    // "WHITESPACE",
    "ARRAY", // done
    // "FUNCTION",
    "NUMBER",   // done
    "COMMENT",  // done
    "CONSTANT", // done
};
typedef struct func {
  int type;
  long long token_offset;
} function;
typedef struct tok {
  int token_type;
  string data;
} token;

bool unsafe = false;
bool strong = false;
bool no_warn = false;

string keywords[]{
    "void", // 0
    "num",  // 1
    // "int",
    // "short",
    // "byte",
    "full", // 2
    // "uint",
    // "ushort",
    // "ubyte",
    "double", // 3
    // "float",
    "char",   // 4
    "string", // 5
    "func",   // 6
    "label",  // 7
    "module", // 8
    // IO Keywords
    "print", // 9
    "input", // 10
    "write", // 11
    "read",  // 12
    // Control keywords
    "if",    // 13
    "elif",  // 14
    "else",  // 15
    "for",   // 16
    "while", // 17
    "goto",  // 18
    "end",   // 19
    "then",  // 20
    "array", // 21
		"sleep", // 22
};

const string CHARS =
    "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUBWXYZ1234567890_";
const string NUMS = "1234567890._";
const string OPERATORS = "+-*/%&|^!~$@><=";

namespace KN {
class Shell {
public:
  static void cli_shell() {
    std::cout << "SHELL_ERROR: SHELL HAS NOT BEEN IMPLEMENTED!\n";
  }
  static void cli_shell(vector<string> files) {
    std::cout << "SHELL_ERROR: SHELL HAS NOT BEEN IMPLEMENTED!\n";
  }
};
class Nums {
public:
	static int pow(int n, int exp){
		if(exp == 0){
			return 1;
		}
		int tmp = n;
		for(int i = 0; i < exp; i++){
			tmp *= n;
		}
		return tmp;
	}
	static int ATOI(string str){
		int res = 0;
		int mul = 1;
		for(int i = str.length(); i > -1 ; i--){
			if(i < 0){
				break;
			}
			res += str[i] * mul;
			mul *= 10;
		}
		return res;
	}	
};
class Lang {
private:
  vector<unordered_map<string, Variable *>> vars;
  int scope_num = 0;
  int max_scope = 0;
  unordered_map<string, function> funcs;
  unordered_map<string, token> constants;
  vector<string> lines;
  vector<token> tokens;
  unordered_map<string, Integer> ints();
  // string at the top of files is the file to contain the start of the program.
  // append lines of following files in order.
  vector<string> files;

public:
  Lang() {
    vars = vector<unordered_map<string, Variable *>>();
    vars.push_back(unordered_map<string, Variable *>());
    funcs = unordered_map<string, function>();
    lines = vector<string>();
    tokens = vector<token>();
    files = vector<string>();
    constants = unordered_map<string, token>();
  }
  void read_files() {
    if (files.size() == 0) {
      std::cout << "KNL: No Input File Specified!\n";
      exit(-1);
    }
    for (int i = 0; i < files.size(); i++) {
      ifstream file_to_read(files[i]);
      if (!file_to_read.is_open()) {
        std::cout << "KNL: Invalid Input File \"" << files[i] << "\"!\n";
      }
      while (file_to_read) {
        char *line = (char *)malloc(512);
        file_to_read.getline(line, 512, '\n');
        if (!strcmp(line, "")) {
          continue;
        }
        lines.push_back(line);
        free(line);
      }
    }
  }
  void add_file(string file) { files.push_back(file); }
  void tokenize() {
    vector<string> errors = vector<string>();
    for (int i = 0; i < lines.size(); i++) {
      string line = lines[i];
      for (int j = 0; j < line.size(); j++) {
        if (line[j] == '\n' || line[j] == '\t') {
          // yes i know it's redundant
          continue;
        } else if (line[j] == '\"') {
          // parse for string
          string topush = "";
          j++;
          while (line[j] != '\"') {
            if (line[j] == '\n' || j >= line.size()) {
              string err = "KNL: Error: Unterminated String on line ";
              err += std::to_string(i + 1);
              err += "HERE: " + line;
              errors.push_back(err);
              break;
            }
            topush += line[j++];
          }
          // j--;
          tokens.push_back(token{TokenType::STRING, topush});
          // std::cout << topush;
        } else if (line[j] == '#') {
          // directive specifier
          //  token t = token{};
          j++;
          if (j > line.size()) {
            string err = "KNL: Error: Directive token with no Directive "
                         "specified on line ";
            err += std::to_string(i + 1);
            err += "\nHERE: " + line;
            errors.push_back(err);
            break;
          }
          string possible_dir = "";
          while (CHARS.find(line[j]) != string::npos && j < line.size()) {
            possible_dir += line[j++];
          }
          if (possible_dir == "strong") {
            strong = true;
            // std::cout << "Strong file" << std::endl;
          } else if (possible_dir == "shell") {
            // KN::Shell::cli_shell(files);
          } else if (possible_dir == "unsafe") {
            unsafe = true;
          } else if (possible_dir == "important") {
            no_warn = true;
          } else if (possible_dir == "endstrong") {
            strong = false;
          } else if (possible_dir == "endimportant") {
            no_warn = false;
          } else if (possible_dir == "safe") {
            unsafe = false;
          } else if (possible_dir == "const") {
            token t = {};
            j++;
            if (j >= line.size()) {
              string err =
                  "KNL: Error: #define Directive with no definition on line ";
              err += std::to_string(i + 1);
              err += "\nHERE: " + line;
              errors.push_back(err);
            }
            string id = "";
            while (CHARS.find(line[j]) != string::npos && j < line.size()) {
              id += line[j++];
            }
            j--;
            token tmptok = tokenize_const((line.c_str() + j));
            if (tmptok.token_type == -1) {
              errors.push_back("KNL: Error: Tokenizer ran into an error while "
                               "parsing token: token not recognzied on line " +
                               std::to_string(i + 1) + "\nHERE: " + line);
            }
            constants[id] = tmptok;
            tokens.push_back(token{TokenType::CONSTANT, tmptok.data});
            break;

          } else {
            string err = "KNL: Error: Unknown Directive on line ";
            err += std::to_string(i + 1) + "\nHERE: " + line;
            errors.push_back(err);
          }
        } else if (NUMS.find(line[j]) != string::npos) {
          string num = "";
          while (NUMS.find(line[j]) != string::npos && j < line.size()) {
            num += line[j++];
          }
          j--;
          // std::cout << num;
          tokens.push_back(token{TokenType::NUMBER, num});
        } else if (line[j] == '/') {
          if (j >= line.size()) {
            continue;
          }
          if (line[j + 1] == '*') {
            token tok = token{TokenType::COMMENT, ""};
            bool searching = true;
            while (searching) {
              if (j >= line.size()) {
                string err = "KNL: Error: Unterminated comment on line " +
                             std::to_string(i + 1) + "\nHERE: " + line;
                errors.push_back(err);
                break;
              }
              if (line[j] == '*') {
                if (line[j + 1] == '/') {
                  searching = false;
                  // break;
                }
              }
              tok.data += line[j];

              j++;
              if (searching == false) {
                tok.data += line[j];
              }
            }
            tokens.push_back(tok);
          }
        } else if (OPERATORS.find(line[j]) != string::npos) {
          token tok = {TokenType::OPERATOR, ""};
          tok.data += line[j];
          tokens.push_back(tok);
        } else if (CHARS.find(line[j]) != string::npos) {
          string str = (string) "";
          if (j > line.size()) {
            continue;
          }
          while ((CHARS.find(line[j] != string::npos) && (j < line.size()))) {

            if (j >= line.size() || line[j] == ' ' || line[j] == '\n' ||
                line[j] == '\t' || CHARS.find(line[j]) == string::npos) {
              // yes, it's redunant. no, it doenst work without it :/
              break;
            }
            str += line[j++];
          }
          // j--;
          token tok{};
          bool found = false;
          // hardcoded keywords.length, idk why its not working if it's not
          // hardcoded
          for (int k = 0; k < 23; k++) {
            string cmp = keywords[k];
            if (keywords[k] == str) {
              found = true;
            }
          }
          if (found) {
            tok.token_type = TokenType::KEYWORD;
            tok.data = str;
          } else {
            tok.token_type = TokenType::IDENTIFIER;
            tok.data = str;
          }
          tokens.push_back(tok);
        } else if (line[j] == '{') {
          int start = j;
          j++;
          int type = -1;
          vector<token> tmptoks = vector<token>();
          while (line[j] != '}') {
            if (j >= line.size()) {
              string err =
                  "KNL: Error: Unterminated Array Initializer on line " +
                  std::to_string(i + 1) + "\nHERE: " + line;
              errors.push_back(err);
              break;
            }
            token tmptok = tokenize_const(line.c_str() + j);
            if (tmptok.token_type == -1) {
              if (tmptok.data == "UNTERM_STRING") {
                string err = "KNL: Error: Unterminated String on line " +
                             std::to_string(i + 1) + "\nHERE: " + line;
                errors.push_back(err);
                break;
              }
            }
            if (type == -1) {
              type = tmptok.token_type;
            } else if (tmptok.token_type != type) {
              errors.push_back("KNL: Error: Incompatible types in array "
                               "initializer on line " +
                               std::to_string(i + 1) + "\nHERE: " + line);
            }
            bool instr = 0;
            bool tobreak = 0;
            while (!(line[j] == ',' || line[j] == '}') && !instr) {
              if (j >= line.size()) {
                string err =
                    "KNL: Error: Missing ',' in Array Initializer on line " +
                    std::to_string(i + 1) + "\nHERE: " + line;
                errors.push_back(err);
                break;
              }
              if (j == '"') {
                instr = !instr;
              }
              if (line[j] == '}') {
                tobreak = true;
                continue;
              } else {
                j++;
              }
            }
            if (line[j] == '}') {
              break;
            } else {
              j++;
            }
          }
          // for(int k = 0; k < tmptoks.size();k++);
          token topush = token{TokenType::ARRAY, line.substr(start, start - j)};
          tokens.push_back(topush);
        }
				else if(line[j] == ';'){
					token tok = token{TokenType::END, ";"};
				}
				else {
          if (line[j] != ' ')
            errors.push_back((string) "KNL: Error: Unexpected token \'" +
                             line[j] + "\' on line " + std::to_string(i + 1) +
                             "\nHERE: " + line);
        }
      }
    }
    if (errors.size() > 0) {
      for (int i = 0; i < errors.size(); i++) {
        std::cout << errors[i] << "\n";
      }
      exit(-1);
    }
  }
  void debug_print_toks() {
    std::cout << "{" << std::endl;
    for (int i = 0; i < tokens.size(); i++) {
      std::cout << "\t{ " << token_type_str[tokens[i].token_type] << " : "
                << std::to_string(tokens[i].token_type) << " : \""
                << tokens[i].data << "\" },\n";
    }
    std::cout << "}\n";
  }
  token tokenize_const(string str) {
    vector<string> errors = vector<string>();
    string line = str;
    for (int j = 0; j < line.size(); j++) {
      if (line[j] == ' ' || line[j] == '\n') {
        // yes i know it's redundant
        continue;
      } else if (line[j] == '\"') {
        // std::cout << "String";
        // parse for string
        string topush = "";
        j++;
        while (line[j] != '\"') {
          if (line[j] == '\n' || j >= line.size()) {
            return token{-1, "UNTERM_STRING"};
          }
          topush += line[j++];
        }
        // tokens.push_back(token{TokenType::STRING, topush});
        return token{TokenType::STRING, topush};
        std::cout << topush;
      }
      if (NUMS.find(line[j]) != string::npos) {
        string num = "";
        while (NUMS.find(line[j]) != string::npos && j < str.size()) {
          num += line[j++];
        }
        return token{TokenType::NUMBER, num};
      }

      else if (CHARS.find(line[j]) != string::npos) {
        string str = (string) "";
        if (j > line.size()) {
          continue;
        }
        while ((CHARS.find(line[j] != string::npos) && (j < line.size()))) {
          str += line[j++];
          if (j >= line.size()) {
            // yes, it's redunant. no, it doenst work without it :/
            break;
          }
        }
        token tok{};
        if (keywords->find(str) != string::npos) {
          tok.token_type = TokenType::KEYWORD;
          tok.data = str;
        } else {
          tok.token_type = TokenType::IDENTIFIER;
          tok.data = str;
        }
        return tok;
      } else {
        return token{-1, "INV_TOKEN"};
      }
    }
    return token{TokenType::NUMBER, "1"};
  }
	//runner and runner functions
	
	
	void sleep(int ticks){
		int r = ticks * 10000;
		for(int i = 0; i < r; i++);
	}
  void preprocess() {
    for (int i = 0; i < tokens.size(); i++) {
      if (tokens[i].token_type == TokenType::KEYWORD &&
          tokens[i].data == "label") {
        i++;
        if (i > tokens.size()) {
          std::cout << "Error: Label keyword with no label. Expected: "
                       "TokenType::IDENTIFIER\n";
        }

        Integer *intt = new Integer();
        intt->type = VarType::LABEL;
        intt->data = i;

        vars[0][tokens[i].data] = intt;
        Integer *t = (Integer *)(vars[0][tokens[i].data]);
        std::cout << "Label Data: " << vars[0][tokens[i].data]->type << " : "
                  << t->data << " " << tokens[i].data << std::endl;
      }
    }
  }
  Variable *get_var(string varname) {
    for (int i = scope_num; i >= 0; i--) {
			if(i < 0){
				break;
			}
      if (vars[i].count(varname)) {
        return vars[i][varname];
      }
    }
    return new Variable{-1};
  }
	int eval(vector<token> expr){
		int op1 = 0;
		int op2 = 0;
		int op = 0;
		//const string OPERATORS = "+-*/%&|^!~$@><=";
		for(int i = 0; i < expr.size(); i++){
			
			if(expr[i].token_type == TokenType::IDENTIFIER){
				// op1 = 
			}
			else if(expr[i].token_type == TokenType::NUMBER){
				
			}
			else if(expr[i].token_type == TokenType::OPERATOR){
				op = OPERATORS.find(expr[i].data);
				std::cout << op;
			}
		}
	}
  void run() {
    for (int i = 0; i < tokens.size(); i++) {
      if (tokens[i].token_type == TokenType::COMMENT) {
        continue;
      } else if (tokens[i].token_type == TokenType::KEYWORD) {
        if (tokens[i].data == "print") {
          if (i >= tokens.size()) {
            std::cout << "Error: builtin 'print' called at EOF!\n";
            exit(-1);
          }
          string data = "";
          if (tokens[++i].token_type == TokenType::IDENTIFIER) {
            Variable *ret = get_var(tokens[i].data);
            if (ret->type == -1) {
              std::cout << "Error: Undeclared Identifier '" << tokens[i].data
                        << "'!\n";
							data = "Error";
              // exit(-1);
            }
            if(ret->type == VarType::STRING){
							data = "string";
						}
						else{
							data = "hi";
							std::cout << "Error!";
						}
          } else if (tokens[i].token_type == TokenType::STRING ||
                     tokens[i].token_type == TokenType::NUMBER) {
            data = tokens[i].data;
          }
          std::cout << data << std::endl;
        }
        if (tokens[i].data == "goto") {
          token tmptok = tokens[i + 1];
					if( tmptok.token_type == TokenType::IDENTIFIER){
						string label = tmptok.data;
						Integer *intt = (Integer *)get_var(label);
						i = intt->data;
					}
          else if(tmptok.token_type == TokenType::NUMBER){
						i = Nums::ATOI(tmptok.data);
					}
        }
				else if(tokens[i].data == "num"){
					vars[scope_num][tokens[++i].data] = new Integer(VarType::INT64, 0);
					vector<token>expr = vector<token>();
					while(tokens[i].token_type != TokenType::END){
						if(tokens[i].token_type == TokenType::END){
							break;
							//I KNOW ALL OF THIS IS REDUNDANT BUT IDK WHY IT DOESNT WORK WITHOUT IT
						}
						expr.push_back(tokens[i++]);
					}
					i--;
					eval(expr);
				}
				else if(tokens[i].data == "sleep"){
					if(i > tokens.size()){
						std::cout << "Error! Sleep keyword without any time!\n";
						exit(-1);
					}
					if(tokens[++i].token_type == TokenType::NUMBER){
						int time = Nums::ATOI(tokens[++i].data);
						sleep(time);
					}
				}
				else if(tokens[i].data == "if"){
					if(tokens[i].data != "if"){
						std::cout << "Not equal" << std::endl;
						exit(-1);
					}
					std::cout << tokens[i].data << std::endl;
					vector<token> expr = vector<token>();
					i++;
					while(!(tokens[i].data == "then")){
						expr.push_back(tokens[i++]);
					}
					
					i--;
				}
      }
    }
  }
};

}; // namespace KN

int main(int argc, char **argv) {
  if (argc == 1) {
    std::cout << "KNL: No Input File Specified!\n";
  } else {
    KN::Lang *runner = new KN::Lang();
    for (int i = 1; i < argc; i++) {
      string arg = argv[i];
      if (arg == "-u") {
        unsafe = true;
      } else if (arg == "-w") {
        no_warn = true;
      } else if (arg == "-t") {
        strong = true;
      } else {
        runner->add_file(arg);
      }
    }
    runner->read_files();
    runner->tokenize();
    runner->debug_print_toks();
    runner->preprocess();
    runner->run();
    // std::cout << "KEYWORD: ";
    // for(int i= 0; i < 20; i++){
    // 	std::cout << keywords[i] << " DATA: ";
    // 	string str = keywords[i];
    // 	for(int j = 0; j < str.size(); j++){
    // 		printf("%d ", str[j]);
    // 	}
    // 	std::cout << std::endl;
    // }
  }
}