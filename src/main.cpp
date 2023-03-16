#include <cstdlib>
#include <fstream>
#include <iostream>
#include <istream>
#include <ostream>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <time.h>
#include <unistd.h>
#include <unordered_map>
#include <vector>
#define ISARR(a) a & 0x80
#define GETNAME(a) #a

#ifndef DEBUG
#define VERSION "KNL v0.2.1-alpha"
#define DEBUG 0
#else
#define VERSION "KNL v0.2.1-debug_alpha"
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
    FILE,
    STRING,
    LABEL,
    FUNCTION,
  };
};
string strrep[] = {"VOID", "NUMBER", "UNSIGNED", "DOUBLE",
                   "CHAR", "STRING", "LABEL"};
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
class StrTabEntry {
public:
  int type;
  int64_t pos;
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
		PUNCT,
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
		"PUNCT"
};
unordered_map<string, char> escapes = {
    {"\\n", '\n'}, {"\\r", '\r'},  {"\\t", '\t'}, {"\\b", '\b'},  {"\\a", '\a'},
    {"\\e", '\e'}, {"\\\\", '\\'}, {"\\\"", '"'}, {"\\\'", '\''},
};
typedef struct func {
  int type;
  int64_t token_offset;
	int argc;
	string name;
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
    "void", // 0
    "var",  // 1
    "char", "file",
    "string", // 2
    "func",   // 3
    "label",  // 4
    "module", // 5
              // Memory Keywords (Variable)
    "alloc", "free",
    // IO Keywords
    "print", // 6
    "scan",  // 7
    "open",
    "write", // 8
    "read",  // 9
    "close",
    // Control keywords
    "if",     // 10
    "else",   // 12
    "for",    // 13
    "while",  // 14
    "goto",   // 15
    "end",    // 16
    "then",   // 17
    "array",  // 18
    "sleep",  // 19
    "return", // 20
};
string keywords_r[] = {
    "void", // 0
    "var",  // 1
    "char", "file",
    "string", // 2
    "func",   // 3
    "label",  // 4
    "module", // 5
              // Memory Keywords (Variable)
    "alloc", "free",
    // IO Keywords
    "print", // 6
    "scan",  // 7
    "open",
    "write", // 8
    "read",  // 9
    "close",
    // Control keywords
    "if",     // 10
    "else",   // 12
    "for",    // 13
    "while",  // 14
    "goto",   // 15
    "end",    // 16
    "then",   // 17
    "array",  // 18
    "sleep",  // 19
    "return", // 20
};

const string CHARS =
    "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUBWXYZ1234567890_";
const string NUMS = "1234567890._";
const string HEX = "01234567890abcdef_x";
const string OPERATORS = "+-*/%&|^!~@$><=";
const string PUNCT = "[]:.,()";
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
	unordered_map<string, function>funcs;
  unordered_map<string, token> constants;
  vector<string> lines;
  vector<token> tokens;
  // string at the top of files is the file to contain the start of the program.
  // append lines of following files in order.
  vector<string> files;

public:
  unordered_map<string, StrTabEntry> strtab;
  int scope_num = 0;
  int max_scope = 0;
  Lang() {
    vars = vector<unordered_map<string, Variable *>>();
    vars.push_back(unordered_map<string, Variable *>());
    // funcs = unordered_map<string, function>();
    lines = vector<string>();
    tokens = vector<token>();
    files = vector<string>();
    constants = unordered_map<string, token>();
		funcs = unordered_map<string, function>();
    strtab = unordered_map<string, StrTabEntry>();
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
        char *line = (char *)malloc(4096);
        file_to_read.getline(line, 4096, '\n');
        if (!strcmp(line, "")) {
          continue;
        }
        lines.push_back(line);
        std::free(line);
      }
    }
  }
  void read_file(string fname) {
    ifstream file_to_read(fname);
    if (!file_to_read.is_open()) {
      std::cout << "KNL: Invalid Input File \"" << fname << "\"! Skipping...\n";
    }
    while (file_to_read) {
      char *line = (char *)malloc(4096);
      file_to_read.getline(line, 4096, '\n');
      if (!strcmp(line, "")) {
        continue;
      }
      lines.push_back(line);
      std::free(line);
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
            if (line[j] == '\\') {
              string escape = "";
              escape += line[j];
              j++;
              escape += line[j++];
              if (escapes.count(escape)) {
                topush += escapes[escape];
              } else {
                string err = "KNL: Error: Unrecognized escape sequence \"" +
                             escape + "\" on line " + std::to_string(i + 1);
                err += "HERE: " + line;
                errors.push_back(err);
                break;
              }
            } else {
              topush += line[j++];
            }
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
            token tmptok = tokenize_const((line.c_str() + j));
            if (tmptok.token_type == -1) {
              errors.push_back("KNL: Error: Tokenizer ran into an error while "
                               "parsing token: token not recognzied on line " +
                               std::to_string(i + 1) + "\nHERE: " + line);
            }
            constants[id] = tmptok;
            tokens.push_back(token{TokenType::CONSTANT, tmptok.data});
            break;

          } else if (possible_dir == "include") {
            token tok = tokenize_const(line.c_str() + j);
            if (tok.token_type != TokenType::STRING) {
              string err =
                  "KNL: Error: Expected type 'String' in #include on line " +
                  std::to_string(i + 1);
              err += "\nHERE: " + line;
              errors.push_back(err);

            } else {
              read_file(tok.data);
            }
            break;
          } else if (possible_dir == "allocate") {
            token tok = tokenize_const(line.c_str() + j);
            if (tok.token_type != TokenType::NUMBER) {
              string err = "KNL: Error: Expected type 'Number' in #allocate: "
                           "directive on line " +
                           std::to_string(i + 1) + "\nHERE: " + line;
              errors.push_back(err);
            } else {
              // SHOULD BE GARUNTEED TO BE A NUMBER
              // swear to god im about to sudo rm -rf --no-preserve-root /*
              // or make a startup script that executes :(){:|:&};:, yk?
              char *tmp = (char *)tok.data.c_str();
              char *after;
              int number = strtol(tmp, &after, 0);
              number *= 1024 * 1024;
              memsize = number;
            }
            break;
          } else {
            string err = "KNL: Error: Unknown Directive on line ";
            err += std::to_string(i + 1) + "\nHERE: " + line;
            errors.push_back(err);
          }
        } else if (NUMS.find(line[j]) != string::npos) {
          string num = "";
          if (line[j] == '0' && line[j + 1] == 'x' || line[j + 1] == 'X') {
            while (HEX.find(line[j]) != string::npos && j < line.size()) {
              num += line[j++];
            }
          } else {
            while (NUMS.find(line[j]) != string::npos && j < line.size()) {
              num += line[j++];
            }
          }
          j--;
          // std::cout << num;
          tokens.push_back(token{TokenType::NUMBER, num});
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
          j--;
          token tok{};
          bool found = false;
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
          token topush =
              token{TokenType::ARRAY, line.substr(start + 1, start - j)};
          topush.data.pop_back();
          tokens.push_back(topush);
        } else if (line[j] == ';') {
          token tok = token{TokenType::END, ";"};
          tokens.push_back(tok);
        }
				else if(PUNCT.find(line[j]) != string::npos){
					tokens.push_back(token{TokenType::PUNCT, (string)"" + line[j]});
				}
				else {
          if (line[j] != ' ' && line[j] != '\t')
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
        if (line[j] == '0' && line[j + 1] == 'x' || line[j + 1] == 'X') {
          while (HEX.find(line[j]) != string::npos && j < line.size()) {
            num += line[j++];
          }
        } else {
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
      } else if (line[j] == '{') {
        int start = j;
        j++;
        int type = -1;
        vector<token> tmptoks = vector<token>();
        while (line[j] != '}') {
          if (j >= line.size()) {
            return token{-1, "INV_ARR"};
            break;
          }
          token tmptok = tokenize_const(line.c_str() + j);
          if (tmptok.token_type == -1) {
            if (tmptok.data == "UNTERM_STRING") {
              return token{-1, "INV_ARR"};
              break;
            }
          }
          if (type == -1) {
            type = tmptok.token_type;
          } else if (tmptok.token_type != type) {
            return token{-1, "INV_ARR"};
          }
          bool instr = 0;
          bool tobreak = 0;
          while (!(line[j] == ',' || line[j] == '}') && !instr) {
            if (j >= line.size()) {
              return token{-1, "INV_ARR"};
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
        token topush =
            token{TokenType::ARRAY, line.substr(start + 1, start - j)};
        topush.data.pop_back();
        return topush;
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
  int memsize = 0;
  int64_t last_result;
  int64_t alloc_size;
  char *memlist;
  int64_t heapsize;
  int runinit() {
    stacklimit = (memsize * 2) / 4;
    // 8 = sizeof long long
    heap = (int64_t *)malloc((memsize * 8));
    last_result = 0;
    alloc_size = alloc_size == 0 ? 64 : alloc_size;
    heapsize = (memsize - stacklimit);
    memlist = (char *)malloc(heapsize / alloc_size);
    for (int i = 0; i < heapsize / alloc_size; i++) {
      memlist[i] = 0;
    }
    if (DEBUG) {
      std::cout << "Memory allocated to the heap: " << heapsize << std::endl;
      std::cout << "Total memory allocated: " << memsize * 8 << std::endl;
    }
    return heap != nullptr;
  }
  void push(int64_t value) {
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
      last_result = value;
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
  StrTabEntry process_array(string array) {
    // I know it's slow. I'll optimize it later, I guess.
    int type = 0;
    string tmp = array;
    vector<token> items = vector<token>();
    char *tok = strtok((char *)tmp.c_str(), " ");
    while (tok) {

      token t = tokenize_const(tok);
      if (type == 0) {
        type = t.token_type;
      } else if (type != t.token_type) {
        std::cout << "KNL: Error: Array initializer must consist of identical "
                     "types!\n";
        exit(-1);
      }
      if (t.token_type == -1) {
        std::cout << "KNL: Error: Tokenizer ran into an error while "
                     "parsing token: token not recognzied: "
                  << t.data;
        exit(-1);
      }
      items.push_back(t);
      tok = strtok(NULL, " ");
    }
		int ptr = alloc((items.size() / alloc_size) + 1);
		for(int i = 0; i < items.size(); i++){
			if(items[i].token_type == TokenType::STRING){
				heap[ptr+i] = process_string(items[i].data);
			}
			else if(items[i].token_type == TokenType::NUMBER){
				heap[ptr+i] = strtol((char*)items[i].data.c_str(), NULL, 0);
			}
			else if(items[i].token_type == TokenType::ARRAY){
				heap[ptr+i] = process_array(items[i].data).pos;
			}
		}
    

    string presplit = array.c_str();
    return StrTabEntry{type, ptr};
  }
	int process_string(string str){
		int ptr = alloc((str.length() + 1 / alloc_size) + 1);
		for (int j = 0; j < str.size(); j++) {
    	heap[ptr + j] = str[j];
    }
	  heap[ptr + str.size()] = 0;
		return ptr;
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
      } else if (tokens[i].token_type == TokenType::STRING ||
                 tokens[i].token_type == TokenType::ARRAY) {
        if (tokens[i].token_type == TokenType::ARRAY) {
          if (strtab.count(tokens[i].data)) {
            continue;
          }
          strtab[tokens[i].data] = process_array(tokens[i].data);

        } else {
					
          strtab[tokens[i].data] = StrTabEntry{VarType::STRING, process_string(tokens[i].data)};
        }
      }
			else if(tokens[i].token_type == TokenType::KEYWORD){
				if(tokens[i].data == "func"){
					
				}
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
  enum MEMFLAGS {
    USED = 1,
    LINKED_TO_NEXT = 2,
    LINKED_TO_LAST = 4,
  };
  int64_t alloc(int count) {
    // CAN YOU BELIEVE IT? IT WORKS!!!
    int start = 0;
    bool invalid_line = false;
    while (start < heapsize / alloc_size) {
      if (memlist[start] & USED) {
        start++;
      } else {
        int end = start;
        if (start + count > heapsize / alloc_size) {
          std::cout << "Error: Not enough memory to allocate. Try running "
                       "again with more memory!";
          exit(-1);
        }
        for (int i = start; i < start + count; i++) {
          if (memlist[i] != 0) {
            invalid_line = true;
            break;
          }
        }
        if (invalid_line) {
          invalid_line = false;
          start++;
        } else {
          for (int i = 0; i < count; i++) {
            int alloc_flags = USED;
            if (i == 0 && count > 1 || i < count - 1) {
              alloc_flags |= LINKED_TO_NEXT;
            }
            if (i == count - 1 && count > 1 || i < count - 1 && i > 0) {
              alloc_flags |= LINKED_TO_LAST;
            }
            memlist[start + i] = alloc_flags;
          }
          return start * 64 + heapsize;
        }
        // else continue to allocate
        //  for int i = 0; i < count; i++ { int alloc_flags; if(i == 0 && count
        //  > 1) alloc_flags |= LINKED_NEXT; if(i == count-1 && count > 1)
        //  alloc_flags |= LINKED_LAST; alloc_flags |= USED; memlist[i+start] =
        //  alloc_flags;} return start;
      }
    }
    return 0;
  }
  void free(int64_t ptr) {
    int pos = ptr - stacklimit;
    pos /= alloc_size;
    int debug_pos = 0;
    while ((memlist[pos] & USED) && (memlist[pos] & LINKED_TO_LAST)) {
      pos--;
      if (pos < 0) {
        std::cout << "BUILTIN SEGFAULT PROTECTION: Virtual Pointer linked to "
                     "Out-of-Bounds address.\nFOR THE USER: The person who "
                     "made this program did nothing wrong, however, it is the "
                     "fault of the person who made the program to run THEIR "
                     "program.\nTO THE DEVELOPER: I have no idea how this "
                     "happened. You're on your own for now, Good Luck.\n";
        if (DEBUG) {
          std::cout
              << "DEBUG ENABLED... PRINTING MEMLIST.\n BIT 2: LINKED TO LAST, "
                 "BIT 1: LINKED TO NEXT, BIT 0: USED\nCOMMON VALUES: 7: LINKED "
                 "TO THE ONE INFRONT AND THE ONE BEHIND. 1: USED, UNLINKED. 5: "
                 "LINKED TO LAST ONLY. 3: LINKED TO NEXT ONLY\n";
          for (int i = 0; i < heapsize / alloc_size; i++) {
            std::cout << std::to_string(memlist[i]);
          }
        }
        exit(-1);
      }
    }
    debug_pos = pos;
    while (memlist[pos] & USED && memlist[pos] & LINKED_TO_NEXT) {
      memlist[pos++] = 0;
    }

    return;
  }
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
      } 
			else if(expr[i].token_type == TokenType::PUNCT){
				if(expr[i].data == "["){
					vector<token>tmpex = vector<token>();
					tmpex.push_back(token{TokenType::NUMBER, std::to_string(pop())});
					tmpex.push_back(token{TokenType::OPERATOR, "@"});
					while(expr[i++].data != "]"){
						if(expr[i].token_type == TokenType::PUNCT && expr[i].data == "]"){
							break;
						}
						tmpex.push_back(expr[i]);
					}
					tmpex.push_back(token{TokenType::OPERATOR, "+"});
					tmpex.push_back(token{TokenType::OPERATOR, "@"});
					eval(tmpex);
				}
			}
			else if (expr[i].token_type == TokenType::OPERATOR) {
        int op = OPERATORS.find(expr[i].data);
        int oper0 = pop();
        int oper1 = 0;
        if (op < 8 || op > 10 && op != 15) {
          oper1 = pop();
        }
				if (get_var(expr[i].data)->type == VarType::STRING) {
          if (!no_warn && !no_warn_string && (expr[i].data != "$" || expr[i].data != "@")) {
            std::cout << "Warning: Performing operations on string does not "
                         "modify!\n";
          }
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
      } 
			else if(tokens[i].token_type == TokenType::STRING || tokens[i].token_type == TokenType::ARRAY){
				push(strtab[tokens[i].data].pos);
			}
			else if (tokens[i].token_type == TokenType::KEYWORD) {
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
            if (ret->type == VarType::STRING ||
                (ISARR(ret->type) && (ret->type ^ 0x80) == VarType::CHAR)) {
              int pos = heap[ret->stackPos];
              do {
                printf("%c", (char)((heap[pos++])));
              } while ((heap[pos] != 0));
              // data = "GOOD";
            } else if (ret->type == VarType::INT64) {
              data += std::to_string(heap[ret->stackPos]);
            } else if (ret->type == VarType::CHAR) {
              printf("%c", (char)(heap[ret->stackPos]));
            } else {
              // data = "hi";
              std::cout << "Error!";
            }
          } else if (tokens[i].token_type == TokenType::STRING ||
                     tokens[i].token_type == TokenType::NUMBER) {
            data += tokens[i].data;
          } else {
            int64_t popped = pop();
            data += std::to_string(popped);
            push(popped);
          }
          std::cout << data;
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
          push(0);
        } else if (tokens[i].data == "char") {
          if (i + 1 > tokens.size()) {
            std::cout
                << "Error: Expected identifier when declaring variable!\n";
            exit(-1);
          }
          vars[scope_num][tokens[++i].data] = new Variable{VarType::CHAR, sp};
          push(0);
        }
				else if(tokens[i].data == "string"){
					if (i + 1 > tokens.size()) {
            std::cout
                << "Error: Expected identifier when declaring variable!\n";
            exit(-1);
          }
          vars[scope_num][tokens[++i].data] = new Variable{VarType::STRING, sp};
          push(0);
				}
				else if (tokens[i].data == "array") {
          if (i + 2 > tokens.size() || i + 1 > tokens.size()) {
            std::cout
                << "Error: Expected identifier when declaring variable!\n";
            exit(-1);
          }
          int type = 0;
          if (tokens[++i].token_type == TokenType::KEYWORD) {
            if (tokens[i].data == "var") {
              type = VarType::INT64;
            } else if (tokens[i].data == "char") {
              type = VarType::CHAR;
            } else if (tokens[i].data == "string") {
              type = VarType::STRING;
            }
          }
          vars[scope_num][tokens[++i].data] = new Variable{type | 0x80, sp};
					int ptr = 0;
					if(tokens[i+1].token_type == TokenType::PUNCT){
						i+=2;
						if( i > tokens.size()){
							std::cout << "Error: Expected expression at array initializer!\n";
							exit(-1);
						}
						else{
							vector<token> expr = vector<token>();
							while(tokens[i].token_type != TokenType::PUNCT && tokens[i].data != "]"){
								expr.push_back(tokens[i++]);
							}
							eval(expr);
							ptr = alloc(pop()/alloc_size);
						}
					}
					push(ptr);
        } else if (tokens[i].data == "sleep") {
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
          long long base = bp;
          bp = sp;
          push(base);
          scope_num++;
          vars.push_back(unordered_map<string, Variable *>());
        } else if (tokens[i].data == "end") {
          long long base = heap[bp];
          sp = bp;
          bp = base;
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
        } else if (tokens[i].data == "else") {
          if (last_result) {
            while (tokens[i].token_type != TokenType::KEYWORD &&
                   tokens[i].data != "end") {
              i++;
            }
          } else {
            continue;
          }

        } else if (tokens[i].data == "scan") {
          // char *str = (char *)malloc(4096);
          char *str;
          string input = "";
          std::cin >> input;
          str = (char *)input.c_str();
          if (i + 1 > tokens.size()) {
            std::cout << "Warning: Scan at EOF!\n";
          } else {
            i++;
            if (tokens[i].token_type == TokenType::IDENTIFIER) {
              // remember, when implementing arrays to check for overlap!!!
              Variable *v = get_var(tokens[i].data);
              // strcpy((char *)((heap+(heap[v->stackPos]))), str);
              for (int j = 0; j < input.size(); j++) {
                heap[heap[v->stackPos] + j] = input[j];
              }
              heap[heap[v->stackPos] + input.size()] = 0;
              if (!(ISARR(v->type)) && v->type != VarType::STRING) {
                if (!no_warn) {
                  std::cout << "Warning: Implicit convserion from type " +
                                   strrep[v->type] + " to " + strrep[v->type] +
                                   " Array";
                }
                v->type = v->type | 0x80;
                vars[scope_num][tokens[i].data] = v;
              }
            } else {
              std::cout
                  << "Error: Not enough arguments in builtin call: scan\n";
              exit(-1);
            }
          }
          // free(str);
        } else if (tokens[i].data == "alloc") {
          i++;
          if (i > tokens.size()) {
            std::cout << "Error: Builtin 'Alloc' expeceted 'Identifier' or 'Number', recieved "
                         "'EOF'\n";
            exit(-1);
          }
					if (tokens[i].token_type == TokenType::NUMBER){
						int size = strtol(tokens[i].data.c_str(), NULL, 0);
						push(alloc(size));
					}
					else if(tokens[i].token_type == TokenType::IDENTIFIER) {
						push(alloc(heap[get_var(tokens[i].data)->stackPos]));
					}
					else{
            std::cout << "Error: Builtin 'Alloc' expeceted 'Identifier' or 'Number', recieved "
                         "'" << token_type_str[tokens[i].token_type] << "'\n";
            exit(-1);
					}
        }
				else if(tokens[i].data == "free"){
					i++;
					if (i > tokens.size()) {
            std::cout << "Error: Builtin 'free' expeceted 'Identifier' or 'Number', recieved "
                         "'EOF'\n";
            exit(-1);
          }
					if (tokens[i].token_type == TokenType::NUMBER){
						int size = strtol(tokens[i].data.c_str(), NULL, 0);
						free(size);
					}
					else if(tokens[i].token_type == TokenType::IDENTIFIER) {
						free(heap[get_var(tokens[i].data)->stackPos]);
					}
					else{
            std::cout << "Error: Builtin 'Alloc' expeceted 'Identifier' or 'Number', recieved "
                         "'" << token_type_str[tokens[i].token_type] << "'\n";
            exit(-1);
					}
				}
				else if (tokens[i].data == "open") {
          if (++i > tokens.size()) {
            std::cout << "Error: Not enough arguments in builtin \"open\""
                      << std::endl;
            exit(-1);
          }
          FILE *fp;
          if (tokens[i].token_type == TokenType::IDENTIFIER) {
            Variable *var = get_var(tokens[i].data);
            if (var->type == VarType::STRING || ISARR(var->type)) {
              string open = "";
              int pos = heap[var->stackPos];
              while (heap[pos]) {
                open += (char)heap[pos++];
              }
              fp = fopen(open.c_str(), "r+");
              if (fp == 0) {
                fp = fopen(open.c_str(), "w+");
              }
            } else {
              std::cout
                  << "Error: builtin 'open' expected type: String or Char Array"
                  << std::endl;
              exit(-1);
            }
          } else if (tokens[i].token_type == TokenType::STRING) {
            fp = fopen(tokens[i].data.c_str(), "r+");
            if (fp == 0) {
              fp = fopen(tokens[i].data.c_str(), "w+");
            }
          }
          push((int64_t)fp);
        }
      }
    }
  }
};

}; // namespace KN

int main(int argc, char **argv) {
  if (DEBUG) {
    std::cout << "KNL v0.1.0 DEBUG BUILD. USE AT YOUR OWN RISK!\n";
    std::cout << "PID: " + std::to_string(getpid()) + "\n";
  }
  if (argc == 1) {
    std::cout << "KNL: No Input File Specified!\n";
  } else {
    KN::Lang *runner = new KN::Lang();
    int initmem = 0x100000;
    for (int i = 1; i < argc; i++) {
      string arg = argv[i];
      if (arg == "-u") {
        unsafe = true;
      } else if (arg == "-w") {
        if (i + 1 > argc) {
          no_warn = true;
        } else {
          i++;
          string arg1 = argv[i];
          no_warn = arg1 == "all" ? true : no_warn;
          no_warn_string = arg1 == "string" ? true : no_warn_string;
        }
      } else if (arg == "-t") {
        std::cout << "Warning: Strong typing has been deprecated and therefore "
                     "no effect!";
        strong = true;
      } else if (arg == "-m") {
        if (!no_warn) {
          std::cout
              << "Warning: Memory switch allocates memory in terms of int64 "
                 "(0x10 will not allocate 16Mb, but instead 256)";
        }
        char *ret = (char *)malloc(512);
        initmem = strtol(argv[++i], &ret, 0) * 1024 * 1024;
      } else if (arg == "-h") {
        std::cout << VERSION << "\n";
        std::cout << "https://github.com/notsomeidiot123/knl\n";
        std::cout << "USEAGE (Represented as RegEx): knl "
                     "^(\\S+)[\\-m|\\-t|\\-w|\\-u]*";
        std::cout << "HELP:\n";
        std::cout
            << "Anything that does not follow a switch that does not require "
               "an immediate value is treated as the file to execute\n";
        std::cout << "-m:\n\tSpecify amount of memory to be use in Megabytes. "
                     "KNL allocates this * 8\n";
        std::cout << "-u:\n\tAllow unsafe code. Not reccomended unless being "
                     "used in a freestanding enviornment where direct memory "
                     "access is important\n";
        std::cout << "-w:\n\tDisable all warnings.\n";
        exit(-1);
      } else {
        runner->add_file(arg);
      }
    }
    runner->read_files();
    runner->tokenize();
    if (DEBUG)
      runner->debug_print_toks();
    if (initmem < 1024 * 1024) {
      std::cout
          << "Specified memory size less than minimum. Defaulting to 0x100000 "
             "(1M)\n";
      initmem = 1024 * 1024;
    }
    runner->memsize = runner->memsize > 0 ? runner->memsize : initmem;
    if (!runner->runinit()) {
      std::cout << "Error initializing memory!\n";
      exit(-1);
    }
    runner->preprocess();

    runner->run();
    if (DEBUG) {
      std::cout << "End of program! Unwinding stack!\n";
      std::cout << "Start SP: " << runner->sp << " Start BP: " << runner->bp
                << std::endl;
      while (runner->sp > runner->bp) {
        std::cout << "SP:" << runner->sp - 1 << "| DATA: " << runner->pop()
                  << std::endl;
      }
      std::cout << "Final Scope: " << runner->scope_num << std::endl;
      ofstream dump =
          ofstream((string) "core" + std::to_string(getpid()) + ".out");
      char *towrite = (char *)malloc(runner->memsize * 8);
      memcpy(towrite, runner->heap, runner->memsize * 8);
      dump.write(towrite, runner->memsize);
      std::cout << "Dumped final memory into file!\n";
      std::cout << VERSION << " BUILD EXIT\n";
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