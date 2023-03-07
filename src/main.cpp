#include <fstream>
#include <iostream>
#include <istream>
#include <ostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <time.h>
#include <unordered_map>
#include <vector>
#include <unistd.h>
#include <stdint.h>
#define ISARR(a) a & 0x80



#ifndef DEBUG
	#define VERSION "KNL v0.1.0-alpha"
	#define DEBUG 0
#else
	#define VERSION "KNL v0.1.0-debug_alpha"
#endif

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
string strrep[] = {
	"VOID",
	"NUMBER",
	"UNSIGNED",
	"DOUBLE",
	"CHAR",
	"STRING",
	"LABEL"
};
// ARRAYS ARE VARIABLE TYPE & 0x80

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
  int64_t stackPos;
};
class Double : public Variable {
public:
  double data;
  Double(int type, double data) {
    this->type = type;
    this->data = data;
  }
};
class String : public Variable {
public:
  string data;
  String(int type, string data) {
    this->type = type;
    this->data = data;
  }
};
class Array : public Variable {
  void *ptr;
  int length;
  Array(int type, void *data, int length) {
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
    "KEYWORD", // done
    "END",
    "IDENTIFIER", // done
    "OPERATOR",   // done
    "STRING",     // done
    // "WHITESPACE",
    "ARRAY", // done
    // "FUNCTION",
    "NUMBER",   // done
    "COMMENT",  // done
    "CONSTANT", // done
};
typedef struct func {
  int type;
  int64_t token_offset;
} function;
typedef struct tok {
  int token_type;
  string data;
} token;

bool unsafe = false;
bool strong = false;
bool no_warn = false;
bool no_warn_string = false;

vector<string> keywords = {
    "void",   // 0
    "var",    // 1
    "string", // 2
    "func",   // 3
    "label",  // 4
    "module", // 5
    // IO Keywords
    "print", // 6
    "scan", // 7
    "write", // 8
    "read",  // 9
    // Control keywords
    "if",     // 10
    "elif",   // 11
    "else",   // 12
    "for",    // 13
    "while",  // 14
    "goto",   // 15
    "end",    // 16
    "then",   // 17
    "array",  // 18
    "sleep",  // 19
    "return", // 20
    "call",   // 21
};
string keywords_r[] = {
    "void",   // 0
    "var",    // 1
		"char",
    "string", // 2
    "func",   // 3
    "label",  // 4
    "module", // 5
    // IO Keywords
    "print", // 6
    "scan", // 7
    "write", // 8
    "read",  // 9
    // Control keywords
    "if",     // 10
    "elif",   // 11
    "else",   // 12
    "for",    // 13
    "while",  // 14
    "goto",   // 15
    "end",    // 16
    "then",   // 17
    "array",  // 18
    "sleep",  // 19
    "return", // 20
    "call",   // 21
};

const string CHARS =
    "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUBWXYZ1234567890_";
const string NUMS = "1234567890._";
const string HEX = "01234567890abcdef_x";
const string OPERATORS = "+-*/%&|^!~@$><=:";

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
  static int pow(int n, int exp) {
    if (exp == 0) {
      return 1;
    }
    int tmp = n;
    for (int i = 0; i < exp; i++) {
      tmp *= n;
    }
    return tmp;
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
				if (line[j] == '/') {
          if (j >= line.size()) {
            continue;
          }
          if (line[j + 1] == '*') {
            token tok = token{TokenType::COMMENT, ""};
            bool searching = true;
            while (searching) {
              if (i >= lines.size()) {
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
              if (j >= line.size()) {
                j = 0;
                i++;
              }
              if (searching == false) {
                tok.data += line[j];
              }
            }
            tokens.push_back(tok);
          } else if (line[j + 1] == '/') {

            break;
          }
        }
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
          if(line[j] == '0' && line[j+1] == 'x' || line[j+1] == 'X'){
						while (HEX.find(line[j]) != string::npos && j < line.size() ) {
            	num += line[j++];
          	}
					}
					else{
						while (NUMS.find(line[j]) != string::npos && j < line.size()) {
           		num += line[j++];
          	}
					}
          j--;
          // std::cout << num;
          tokens.push_back(token{TokenType::NUMBER, num});
        }  else if (OPERATORS.find(line[j]) != string::npos) {
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
          j--;
          token tok{};
          bool found = false;
          // hardcoded keywords.length, idk why its not working if it's not
          // hardcoded
          for (int k = 0; k < keywords.size(); k++) {
            string cmp = keywords[k];
            if (keywords[k] == str) {
              found = true;
              break;
            }
          }
          if (found) {
            tok.token_type = TokenType::KEYWORD;
            tok.data = str;
          } else if (constants.count(str)) {
            tok = tokenize_const(constants[str].data);
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
        } else if (line[j] == ';') {
          token tok = token{TokenType::END, ";"};
          tokens.push_back(tok);
        } else {
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
        if(line[j] == '0' && line[j+1] == 'x' || line[j+1] == 'X'){
						while (HEX.find(line[j]) != string::npos && j < line.size() ) {            	num += line[j++];
        	}
				}
				else{
					while (NUMS.find(line[j]) != string::npos && j < line.size()) {
        		num += line[j++];
         	}
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
        if (keywords_r->find(str) != string::npos) {
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
  // runner and runner functions

  void sleep(int ticks) {
    int r = ticks * 10000;
    for (int i = 0; i < r; i++)
      ;
  }
  int64_t *heap;
  int64_t stacklimit;
  int sp;
  int bp;
  int memsize;
  int runinit(int memsize) {
    this->memsize = memsize;
    stacklimit = (memsize * 3) / 4;
    heap = (int64_t *)malloc(memsize);
    return heap != nullptr;
  }
  void push(int value) {
    if (unsafe) {
      heap[(sp++)] = value;
    } else {
      if (sp >= memsize) {
        std::cout << "Error: Stack Call exceeds memory limit!\n";
        exit(-1);
      }
      if (sp >= stacklimit) {
        std::cout << "Error: Stack Overflow!\n";
        exit(-1);
      }
      heap[sp++] = value;
    }
  }
  int64_t pop() {
    if (unsafe) {
      return heap[(--sp)];
    } else {
      sp--;
      if (sp < 0) {
        std::cout << "Error: Operation on stack attempted with insufficient "
                     "arguments!\n";
        exit(-1);
      }
      if (sp < bp) {
        std::cout << "Error: Broken stack frame!\n";
        exit(-1);
      }
      return heap[sp];
    }
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

        Variable *intt = new Variable();
        intt->type = VarType::LABEL;
        intt->stackPos = sp;
        push(i);
        vars[0][tokens[i].data] = intt;
      }
    }
  }
  Variable *get_var(string varname) {
    for (int i = scope_num; i >= 0; i--) {
      if (i < 0) {
        break;
      }
      if (vars[i].count(varname)) {
        return vars[i][varname];
      }
    }
    return new Variable{-1};
  }

  void set(int pos) {
    // WARNING! POPS FROM STACK
    heap[pos] = pop();
  }
  int64_t get(int pos) { return heap[pos]; }

  int eval(vector<token> expr) {
    Variable *left_side = nullptr;
    Variable *right_side = nullptr;
    bool return_char = false;
    bool is_string = false;
    //                           012345678901234
    // const string OPERATORS = "+-*/%&|^!~@$><=:";
    for (int i = 0; i < expr.size(); i++) {
      if (expr[i].token_type == TokenType::NUMBER) {
        char *strt = (char *)expr[i].data.c_str();
        push(strtol(strt, &strt, 0));
      } else if (expr[i].token_type == TokenType::IDENTIFIER) {
      	push(get_var(expr[i].data)->stackPos);
				if(get_var(expr[i].data)->type == VarType::STRING){
					if(!no_warn && !no_warn_string){
						std::cout << "Warning: Performing operations on string does not modify!\n";
					}
				}
      } else if (expr[i].token_type == TokenType::OPERATOR) {
        int op = OPERATORS.find(expr[i].data);
        int oper0 = pop();
        int oper1 = 0;
        if (op < 8 || op > 10) {
          oper1 = pop();
        }
        switch (op) {
        case 0: // add
          push(oper1 + oper0);
          break;
        case 1: // sub
          push(oper0 - oper1);
          break;
        case 2:
          push(oper0 * oper1);
          break;
        case 3:
          if (oper1 == 0) {
            std::cout << "Error: Division by zero!\n";
            exit(-1);
          }
          push(oper0 / oper1);
          break;
        case 4:
          if (oper1 == 0) {
            std::cout << "Error: Division by zero!\n";
            exit(-1);
          }
          push(oper0 % oper1);
          break;
        case 5:
          push(oper0 & oper1);
          break;
        case 6:
          push(oper0 | oper1);
          break;
        case 7:
          push(oper0 ^ oper1);
          break;
        case 8:
          push(!oper0);
          break;
        case 9:
          push(~oper0);
          break;
        case 10:
          // std::cout << ""
					// printf("DEBUG: %lx", heap[oper0]);
          push(heap[oper0]);
          break;
        case 11:
          heap[oper0] = oper1;
          break;
        case 12:
          push(oper0 > oper1);
          break;
        case 13:
          push(oper0 < oper1);
          break;
        case 14:
          push(oper0 == oper1);
          break;
        case 15:
          push(oper1);
          push(oper0);
					push(oper0);
          break;
        }
      }
    }
    // std::cout << pop();
    return 0;
  }
  void run() {
    for (int i = 0; i < tokens.size(); i++) {
      if (tokens[i].token_type == TokenType::COMMENT) {
        continue;
      } else if (tokens[i].token_type == TokenType::NUMBER) {
        vector<token> expr = vector<token>();
        while (tokens[i].token_type != TokenType::END) {
          if (tokens[i].token_type == TokenType::END) {
            break;
            // I KNOW ALL OF THIS IS REDUNDANT BUT IDK WHY IT DOESNT WORK
            // WITHOUT IT
          }
          if (i > tokens.size()) {
            std::cout
                << "Error: Expected END Type token (;) after expression!\n";
            exit(-1);
          }
          expr.push_back(tokens[i++]);
        }
        i--;
        eval(expr);
      } else if (tokens[i].token_type == TokenType::OPERATOR) {
        vector<token> expr = vector<token>();
        while (tokens[i].token_type != TokenType::END) {
          if (tokens[i].token_type == TokenType::END) {
            break;
            // I KNOW ALL OF THIS IS REDUNDANT BUT IDK WHY IT DOESNT WORK
            // WITHOUT IT
          }
          if (i > tokens.size()) {
            std::cout
                << "Error: Expected END Type token (;) after expression!\n";
            exit(-1);
          }
          expr.push_back(tokens[i++]);
        }
        i--;
        eval(expr);
      } else if (tokens[i].token_type == TokenType::IDENTIFIER) {
        vector<token> expr = vector<token>();
        while (tokens[i].token_type != TokenType::END) {
          if (tokens[i].token_type == TokenType::END) {
            break;
            // I KNOW ALL OF THIS IS REDUNDANT BUT IDK WHY IT DOESNT WORK
            // WITHOUT IT
          }
          if (i > tokens.size()) {
            std::cout << "Expected END Type token (;) after expression!\n";
            exit(-1);
          }
          expr.push_back(tokens[i++]);
        }
        i--;
        eval(expr);
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
              // data = "Error";
              exit(-1);
            }
            if (ret->type == VarType::STRING || ISARR(ret->type)) {
							int pos = heap[ret->stackPos];
							std::cout << pos << " ";
							do{
								printf("%c", (char)((heap[pos++])));
								// std::cout << "next: " << heap[pos] << "is zero?" << std::to_string(heap[pos]);
							}while((heap[pos] != 0));
							std::cout << pos;
							// data = "GOOD";
            } else if (ret->type == VarType::INT64) {
              data = std::to_string(heap[ret->stackPos]);
            } else {
              // data = "hi";
              std::cout << "Error!";
            }
          } else if (tokens[i].token_type == TokenType::STRING ||
                     tokens[i].token_type == TokenType::NUMBER) {
            data = tokens[i].data;
          } else {
            int64_t popped = pop();
            data = std::to_string(popped);
            push(popped);
          }
          std::cout << data << std::endl;
        } else if (tokens[i].data == "goto") {
          token tmptok = tokens[i + 1];
          if (tmptok.token_type == TokenType::IDENTIFIER) {
            string label = tmptok.data;
            Variable *intt = get_var(label);
            i = heap[intt->stackPos];
          } else if (tmptok.token_type == TokenType::NUMBER) {
            char *strt = (char *)tmptok.data.c_str();

            // i = strtol(strt, &strt, 0);
          }
        } else if (tokens[i].data == "var") {
          if (i + 1 > tokens.size()) {
            std::cout
                << "Error: Expected identifier when declaring variable!\n";
            exit(-1);
          }
          vars[scope_num][tokens[++i].data] = new Variable{VarType::INT64, sp};
        } else if (tokens[i].data == "char") {
          if (i + 1 > tokens.size()) {
            std::cout
                << "Error: Expected identifier when declaring variable!\n";
            exit(-1);
          }
          vars[scope_num][tokens[++i].data] = new Variable{VarType::CHAR, sp};
        }else if (tokens[i].data == "array") {
          if (i + 2 > tokens.size()) {
            std::cout
                << "Error: Expected identifier when declaring variable!\n";
            exit(-1);
          }
					int type = 0;
          vars[scope_num][tokens[++i].data] = new Variable{type | 0x80, sp};
        }
				else if (tokens[i].data == "sleep") {
          if (i >= tokens.size()) {
            std::cout << "Error: Sleep keyword without any time!\n";
            exit(-1);
          }

          if (tokens[++i].token_type == TokenType::NUMBER) {
            char *start = (char *)tokens[i].data.c_str();
            int time = strtol(start, &start, 0);
            sleep(time);
          }
        } else if (tokens[i].data == "then") {
          scope_num++;
          if (scope_num > max_scope) {
            vars.push_back(unordered_map<string, Variable *>());
            max_scope++;
          }
        } else if (tokens[i].data == "end") {
          scope_num--;
          vars.pop_back();
        } else if (tokens[i].data == "if") {
          if (i >= tokens.size()) {
            std::cout << "Error: Unterminated If Statement!\n";
            exit(-1);
          }
          if (tokens[++i].data != "then" ||
              tokens[i].token_type != TokenType::END) {
            vector<token> expr = vector<token>();
            while (tokens[i].token_type != TokenType::KEYWORD &&
                   tokens[i].data != "then" &&
                   tokens[i].token_type != TokenType::END) {
              if (i > tokens.size()) {
                std::cout << "Unterminated If Statement!\n";
                exit(-1);
                break;
              }
              if (tokens[i].token_type == TokenType::KEYWORD &&
                      tokens[i].data == "then" ||
                  tokens[i].token_type == TokenType::END) {
                break;
              } else {
                // std::cout << tokens[i].data;
                expr.push_back(tokens[i++]);
              }
            }
            eval(expr);
            // i--;
          }
          i--;
          int jump = pop();
          // std::cout << jump << " " << tokens[i].data;
          if (jump) {
            continue;
          } else {
            while (tokens[i].data != "end" && i < tokens.size())
              i++;
          }
        }
        else if(tokens[i].data == "scan"){
          // char *str = (char *)malloc(4096);
					char *str;
          string input = "";
					std::cin >> input;
					str = (char *)input.c_str();
          if(i + 1 > tokens.size()){
            std::cout << "Warning: Scan at EOF!\n";
          }
          else{
            i++;
            if(tokens[i].token_type == TokenType::IDENTIFIER){
              //remember, when implementing arrays to check for overlap!!!
              Variable *v = get_var(tokens[i].data);
              // strcpy((char *)((heap+(heap[v->stackPos]))), str);
							for(int j = 0; j < input.size(); j++){
								heap[heap[v->stackPos] + j] = input[j];
							}
							heap[heap[v->stackPos] + input.size()] = 0;
							if(!(ISARR(v->type)) && v->type != VarType::STRING){
								if(!no_warn){
									std::cout << "Warning: Implicit convserion from type " + strrep[v->type] + " to " + strrep[v->type] + " Array";
								}
								v->type = v->type | 0x80;
								vars[scope_num][tokens[i].data] = v;
							}
            }
						else{
							std::cout << "Error: Not enough arguments in builtin call: scan\n";
							exit(-1);
						}
          }
          // free(str);
        }
      }
    }
  }
};

}; // namespace KN

int main(int argc, char **argv) {
	if(DEBUG){
		std::cout << "KNL v0.1.0 DEBUG BUILD. USE AT YOUR OWN RISK!\n";
		std::cout << "PID: " + std::to_string(getpid()) + "\n";
	}
  if (argc == 1) {
    std::cout << "KNL: No Input File Specified!\n";
  } else {
    KN::Lang *runner = new KN::Lang();
    int initmem = 0x400 * sizeof(int64_t);
    for (int i = 1; i < argc; i++) {
      string arg = argv[i];
      if (arg == "-u") {
        unsafe = true;
      } else if (arg == "-w") {
        if(i+1 > argc){
					no_warn = true;
				}
				else{
					i++;
					string arg1 = argv[i];
					no_warn = arg1 == "all" ? true : no_warn;
					no_warn_string = arg1 == "string" ? true : no_warn_string;
				}
      } else if (arg == "-t") {
        strong = true;
      } else if (arg == "-m") {
				if(!no_warn){
					std::cout << "Warning: Memory switch allocates memory in terms of int64 (0x400 will not allocate one kilobyte, but instead 8)";
				}
        char *ret = (char *)malloc(512);
        initmem = strtol(argv[++i], &ret, 0);
      } else if(arg == "-h"){
				std::cout << VERSION << "\n";
				std::cout << "https://github.com/notsomeidiot123/knl\n";
				std::cout << "USEAGE (Represented as RegEx): knl ^(\\S+)[\\-m|\\-t|\\-w|\\-u]*";
				std::cout << "HELP:\n";
				std::cout << "Anything that does not follow a switch that does not require an immediate value is treated as the file to execute\n";
				std::cout << "-m:\n\tSpecify amount of memory to be use. KNL allocates this * 8\n";
				std::cout << "-u:\n\tAllow unsafe code. Not reccomended unless being used in a freestanding enviornment where direct memory access is important\n";
				std::cout << "-w:\n\tDisable all warnings.\n";
				std::cout << "-t:\n\tEnforce strong typing (May be removed)\n";
				exit(-1);
			}
			else {
        runner->add_file(arg);
      }
    }
    runner->read_files();
    runner->tokenize();
    if (DEBUG)
      runner->debug_print_toks();
    if (!runner->runinit(initmem)) {
      std::cout << "Error initializing memory!\n";
      exit(-1);
    }
    runner->preprocess();
    if (initmem < 0x400) {
      std::cout << "Specified heap size less than minimum. Defaulting to 0x400 "
                   "(1K)\n";
      initmem = 0x400;
    }

    runner->run();
    if (DEBUG) {
      std::cout << "End of program! Unwinding stack!\n";
      while (runner->sp > runner->bp) {
        std::cout << "SP:" << runner->sp - 1 << "| DATA: " << runner->pop()
                  << std::endl;
      }
			ofstream dump = ofstream((string)"core" + std::to_string(getpid()) + ".out");
			char *towrite = (char *)malloc(runner->memsize);
			memcpy(towrite, runner->heap, runner->memsize);
			dump.write(towrite, runner->memsize);
    }
		if(DEBUG){
			std::cout << "KNL v0.1.0 DEBUG BUILD EXIT\n";
			std::cout << "PID: " + std::to_string(getpid()) + "\n";
		}
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
