#include "stdc++.h"
#ifndef EMITTER
#define EMITTER
#include "tokens.cpp"
using namespace std;

class Emitter{
    
    string fullPath;
    string header;
    string code;
    public:
    Emitter(){

    }
    Emitter(string path){
        fullPath = path;
        header.clear();
        code.clear();
    }  

    void emit(string codes){
        code += codes;
    }

    void emitLine(string codes){
        code+=codes+"\n";
    }

    void headerLine(string codes){
        // cout <<"a";
        header+=codes+"\n";
        // cout << header;
    }

    void writeFile(){
        // cout << header <<" " << code;
        ofstream MyFile(fullPath);
        MyFile << header + code;
        MyFile.close();
    }

};



#endif
