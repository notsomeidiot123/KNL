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
        LABEL,
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
        // WHITESPACE,
        ARRAY,
        FUNCTION,
        NUMBER,
        COMMENT,
        CONSTANT,
    };
    
};
string token_type_str[] = {
    "KEYWORD",
    "IDENTIFIER",
    "OPERATOR",
    "STRING",
    // "WHITESPACE",
    "ARRAY",
    "FUNCTION",
    "NUMBER",
    "COMMENT",
    "CONSTANT",
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
    "func",
    "label",
    //IO Keywords
    "print",
    "input",
    "write",
    "read",
    //Control keywords
    "if",
    "elif",
    "else",
    "for",
    "while",
    "goto",
};

const string CHARS ="abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUBWXYZ1234567890_";
const string NUMS = "1234567890._";
const string OPERATORS  = "+-*/%&|^!~$@><=";

namespace KN{
    class Shell{
        public:
            static void cli_shell(){
                std::cout << "SHELL_ERROR: SHELL HAS NOT BEEN IMPLEMENTED!\n";
            }
            static void cli_shell(vector<string> files){
                std::cout << "SHELL_ERROR: SHELL HAS NOT BEEN IMPLEMENTED!\n";
            }
    };
    class Lang{
        private:
            unordered_map<string, variable> vars;
            unordered_map<string, function> funcs;
            unordered_map<string, token> constants;
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
                constants = unordered_map<string, token>();
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
                        file_to_read.getline(line, 512, '\n');
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
                vector<string> errors = vector<string>();
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
                            while(line[j] != '\"'){
                                if(line[j] == '\n' || j >= line.size()){
                                    string err = "KNL: Error: Unterminated String on line ";
                                    err += std::to_string(i+1);
                                    err += "HERE: " + line;
                                    errors.push_back(err);
                                    break;
                                }
                                topush += line[j++];
                            }
                            tokens.push_back(token{TokenType::STRING, topush});
                            // std::cout << topush;
                        }
                        else if(line[j] =='#'){
                            //directive specifier
                            // token t = token{};
                            j++;
                            if(j > line.size()){
                                string err = "KNL: Error: Directive token with no Directive specified on line ";
                                err += std::to_string(i+1);
                                err += "\nHERE: " + line;
                                errors.push_back(err);
                                break;
                            }
                            string possible_dir = "";
                            while(CHARS.find(line[j]) != string::npos && j < line.size()){
                                possible_dir += line[j++];
                            }
                            if(possible_dir == "strong"){
                                strong = true;
                                // std::cout << "Strong file" << std::endl;
                            }
                            else if(possible_dir == "shell"){
                                // KN::Shell::cli_shell(files);
                            }
                            else if(possible_dir == "unsafe"){
                                unsafe = true;
                            }
                            else if(possible_dir == "important"){
                                no_warn = true;
                            }
                            else if(possible_dir == "endstrong"){
                                strong = false;
                            }
                            else if(possible_dir == "endimportant"){
                                no_warn = false;
                            }
                            else if(possible_dir == "safe"){
                                unsafe = false;
                            }
                            else if(possible_dir == "const"){
                                token t = {};
                                j++;
                                if(j >= line.size()){
                                    string err = "KNL: Error: #define Directive with no definition on line ";
                                    err += std::to_string(i+1);
                                    err += "\nHERE: " + line;
                                    errors.push_back(err);
                                }
                                string id = "";
                                while(CHARS.find(line[j]) != string::npos && j < line.size()){
                                    id += line[j++];
                                }
                                constants[id] = tokenize_const((line.c_str()+j));
                            }
                            else{
                                string err = "KNL: Error: Unknown Directive on line ";
                                err += std::to_string(i+1) + "\nHERE: " + line;
                                errors.push_back(err);
                            }
                        }
                        else if(NUMS.find(line[j]) != string::npos){
                            string num = "";
                            while(NUMS.find(line[j])!= string::npos && j < line.size()){
                                num += line[j++];
                            }
                            tokens.push_back(token{TokenType::NUMBER, num});
                        }
                        else if(OPERATORS.find(line[j]) != string::npos){
                            token tok = {TokenType::OPERATOR, ""};
                            if(j <= line.size()-1){
                                j = j <= 0 ? 0 : j-1;
                                string sub = line.substr(j, 2);
                                if(sub == "--" || sub == "++" || sub == "==" || sub == ">=" || sub == "<="){
                                    tok.data = sub;
                                    // j++;
                                    tokens.push_back(tok);
                                    j+=2;
                                    continue;
                                }
                                
                            }
                            j = j == 0 ? j : j + 1;
                            tok.data += line[j];
                            tokens.push_back(tok);
                            j++;
                            
                        }
                        
                    }
                }
                if(errors.size() > 0){
                    for(int i = 0; i < errors.size(); i++){
                        std::cout << errors[i] << "\n";
                    }
                    exit(-1);
                }
                
            }
            void debug_print_toks(){
                for(int i = 0; i < tokens.size(); i++){
                    std::cout << "{ " << token_type_str[tokens[i].token_type] << " : \"" << tokens[i].data << "\" }\n";
                }
            }
            token tokenize_const(string str){
                vector<string> errors = vector<string>();
                string line = str;
                for(int j = 0; j < line.size(); j++){
                    if(line[j] == ' ' || line[j] == '\n'){
                        //yes i know it's redundant
                        continue;
                    }
                    else if(line[j] == '\"'){
                        // std::cout << "String";
                        //parse for string
                        string topush = "";
                        j++;
                        while(line[j] != '\"'){
                            if(line[j] == '\n' || j >= line.size()){
                                return token{-1, ""};
                            }
                            topush += line[j++];
                        }
                        // tokens.push_back(token{TokenType::STRING, topush});
                        return token{TokenType::STRING, topush};
                        std::cout << topush;
                    }
                    if(NUMS.find(line[j]) != string::npos){
                        string num = "";
                        while(NUMS.find(line[j])!= string::npos && j < str.size()){
                            num += line[j++];
                        }
                        return token{TokenType::NUMBER, num};
                    }
                    else {
                        return token{-1, 0};
                    }
                }
                return token {TokenType::NUMBER, "1"};
            }
    };
    class Nums{

    };
};



int main(int argc, char **argv){
    if(argc == 1){
        std::cout << "KNL: No Input File Specified!\n";
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