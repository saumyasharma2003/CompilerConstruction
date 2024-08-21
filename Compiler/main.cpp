#include "stdc++.h"
using namespace std;
#include "lexer.cpp"
#include "parser.cpp"
#include "emitter.cpp"
#include "tokens.cpp"

int main(int argc, char *argv[]){

    
    if(argc!=2){
        cout << "Needs 1 file name to read content";
        return 0;
    }
    // get the filepath
    string filePath = argv[1];
    // Open the file using ifstream
    ifstream file(filePath);
    // confirm file opening
    if (!file.is_open()) {
        // print error message and return
        cerr << "Failed to open file: " << filePath << endl;
        return 1;
    }
    // Read the file line by line into a string
    string code;
    string line;
    while (getline(file, line)) {
        // cout << line << endl;
        code+=line;
        code+="\n";
    }
    
    // Close the file
    // cout << code <<"\n";
    cout << "TEENY_TINY COMPILER\n\n\n";
    file.close();




    lexer lex = lexer(code);
    Emitter emit = Emitter("out.c");
    Parser parse = Parser(lex,emit);
    parse.program();
    cout << "\n\n\nParsing Complete\n";
}