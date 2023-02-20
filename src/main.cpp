#include <istream>
#include <ostream>
#include <fstream>
#include <iostream>
#include <unordered_map>
#include <string>
#include <string.h>
#include <vector>
#include <stdio.h>
#include <stdlib.h>

#define ISARR(a) a & 0x80


using std::vector; using std::unordered_map;
using std::string; using std::fstream;
using std::ifstream; using std::ofstream;

class VarType{
    public:
    enum types{
        VOID,
        INT64,
        INT32,
        INT16,
        INT8,
        UNSIGNED64,
        UNSIGNED32,
        UNSIGNED16,
        UNSIGNED8,
        DOUBLE,
        FLOAT,
        CHAR,
        STRING,
    };
};
//ARRAYS ARE VARIABLE TYPE & 0x80

typedef union{
    long long integer;
    double double_ver;
}data;

typedef struct var{
    int type;
    data stored_value;
}var;
typedef struct arr{
    int type;
    void *ptr;
    int length;
}arr;

typedef union{
    var var_rep;
    arr arr_rep;
}variable;


class TokenType{
    public:
    enum types{
        KEYWORD,
        IDENTIFIER,
        OPERATOR,
        STRING,
        WHITESPACE,
        ARRAY,
        FUNCTION,
        NUMBER,
        COMMENT
    };
    
};
string token_type_str[] = {
    "KEYWORD",
    "IDENTIFIER",
    "OPERATOR",
    "STRING",
    "WHITESPACE",
    "ARRAY",
    "FUNCTION",
    "NUMBER",
    "COMMENT",
};
typedef struct func{
    int type;
    long long line_offset;
}function;
typedef struct tok{
    int token_type;
    string data;
}token;


bool unsafe = false;
bool strong = false;
bool no_warn = false;

string keywords[]{
    "void",
    "long",
    "int",
    "short",
    "byte",
    "ulong",
    "uint",
    "ushort",
    "ubyte",
    "double",
    "float",
    "char",
    "string",
    //IO Keywords
    "print",
    "input",
    "write",
    "read",
};

const string CHARS ="abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUBWXYZ1234567890_";


namespace KN{
    class Shell{
        public:
            static void cli_shell(){
                std::cout << "ERROR: SHELL HAS NOT BEEN IMPLEMENTED!\n";
            }
    };
    class Lang{
        private:
            unordered_map<string, variable> vars;
            unordered_map<string, function> funcs;
            vector<string> lines;
            vector<token> tokens;
            //string at the top of files is the file to contain the start of the program. append lines of following files in order.
            vector<string> files;
        public:
            Lang(){
                vars = unordered_map<string, variable>();
                funcs = unordered_map<string, function>();
                lines = vector<string>();
                tokens = vector<token>();
                files = vector<string>();
            }
            void read_files(){
                if(files.size() == 0){
                    std::cout << "KNL: No Input File Specified!\n";
                    exit(-1);
                }
                for(int i = 0; i < files.size(); i++){
                    ifstream file_to_read(files[i]);
                    if(!file_to_read.is_open()){
                        std::cout << "KNL: Invalid Input File \"" << files[i] << "\"!\n";
                    }
                    while(file_to_read){
                        char *line = (char *)malloc(512);
                        file_to_read.getline(line, 512, ';');
                        if(!strcmp(line, "")){
                            continue;
                        }
                        lines.push_back(line);
                        free(line);
                    }
                }
            }
            void add_file(string file){
                files.push_back(file);
            }
            void tokenize(){
                for(int i = 0; i < lines.size(); i++){
                    string line  = lines[i];
                    for(int j = 0; j < line.size(); j++){
                        if(line[j] == ' ' || line[j] == '\n'){
                            //yes i know it's redundant
                            continue;
                        }
                        else if(line[j] == '\"'){
                            //parse for string
                            string topush = "";
                            j++;
                            while(line[j] != '\"' && j < line.size() && line[j] != '\n'){
                                topush += line[j++];
                            }
                            tokens.push_back(token{TokenType::STRING, topush});
                        }
                    }
                }
                
            }
            void debug_print_toks(){
                for(int i = 0; i < tokens.size(); i++){
                    std::cout << "{ " << token_type_str[tokens[i].token_type] << " : " << tokens[i].data << " }\n";
                }
            }
    };
};



int main(int argc, char **argv){
    if(argc == 0){
        std::cout << "KNL Shell v0.1.0 START\n";
        KN::Shell::cli_shell();
    }else{
        KN::Lang *runner = new KN::Lang();
        for(int i = 1; i < argc; i++){
            string arg = argv[i];
            if(arg == "-u"){
                unsafe = true;
            }
            else if(arg == "-w"){
                no_warn = true;
            }else if(arg == "-t"){
                strong = true;
            }
            else{
                runner->add_file(arg);
            }
        }
        runner->read_files();
        runner->tokenize();
        runner->debug_print_toks();
    }
}