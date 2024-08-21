#include "stdc++.h"
#include "tokens.cpp"
using namespace std;
#ifndef LEXER
#define LEXER
class lexer{
    public:
    string source;
    char curChar;
    int curPos;

    lexer(){
        
    }

    lexer(string source){
        this->source = source + '\n';
        curPos = -1;
        nextChar();
    }


    // Process the next character.
    void nextChar(){
        curPos++;
        if(curPos >= source.size()) curChar = '\0'; //EOF
        else curChar = source[curPos];
    }

    // Return the lookahead character.
    char peek(){
        if(curPos+1>=source.size()) return '\0';
        return source[curPos+1];
    }

    // Invalid token found, print error message and exit.
    void abort(string message){
        cout << "Lexing error "<< message<<curChar<<"\n";
        exit(0);
    }

    // Skip whitespace except newlines, which we will use to indicate the end of a statement.
    void skipWhitespace(){
        while(curChar==' '||curChar=='\t'||curChar=='\r'){
            nextChar();
        }
    }

    // Skip comments in the code.
    void skipComment(){
        if(curChar == '#'){
            while(curChar!='\n'){
                nextChar();
            }
        }
    }

    bool isDigit(char chr){
        if(chr-'0'>=0 and chr-'0'<=9) return true;
        return false;
    }

    Tokens checkIfKeyword(string tokenText){
        if(tokenText=="LABEL") return LABEL;
        if(tokenText=="GOTO") return GOTO;
        if(tokenText=="PRINT") return PRINT;
        if(tokenText=="INPUT") return INPUT;
        if(tokenText=="LET") return LET;
        if(tokenText=="IF") return IF;
        if(tokenText=="THEN") return THEN;
        if(tokenText=="ENDIF") return ENDIF;
        if(tokenText=="WHILE") return WHILE;
        if(tokenText=="REPEAT") return REPEAT;
        if(tokenText=="ENDWHILE") return ENDWHILE;
        return IDENT;
    }

    bool isLowerAlpha(char chr){
        if(chr-'a'>=0 and chr-'a'<=26) return true;
        return false;
    }

    bool isUpperAlpha(char chr){
        if(chr-'A'>=0 and chr-'A'<=26) return true;
        return false;
    }

    bool isAlpha(char chr){
        if(isLowerAlpha(chr)||isUpperAlpha(chr)) return true;
        return false;
    }

    bool isAlphaNum(char chr){
        if(isDigit(chr) or isAlpha(chr)) return true;
        return false;
    }
    // Return the next token.
    Token getToken(){
        // Check the first character of this token to see if we can decide what it is.
        // If it is a multiple character operator (e.g., !=), number, identifier, or keyword then we will process the rest.
        skipWhitespace();
        skipComment();
        Token token;
        string str;
        if(curChar == '+')
            token = Token("+",PLUS);
        else if(curChar == '-')
            token = Token("-",MINUS);
        else if (curChar == '*')
            token =  Token("*",ASTERISK);
        else if (curChar == '/')
            token =  Token("/",SLASH);
        else if (curChar == '\n')\
            token = Token("\n",NEWLINE);
        else if (curChar == '\0')
            token =  Token("\0", EEOF);
        else if(curChar=='='){
            if(peek()=='='){
                nextChar();
                token = Token("==",EQEQ);
            }else{
                token = Token("=",EQ);
            }
        }
        else if(curChar=='<'){
            if(peek()=='='){
                nextChar();
                token = Token("<=",LTEQ);
            }else{
                token = Token("<",LT);
            }
        }
        else if(curChar=='>'){
            if(peek()=='='){
                nextChar();
                token = Token(">=",GTEQ);
            }else{
                token = Token(">",GT);
            }
        }
        else if(curChar=='!'){
            if(peek()=='='){
                token = Token("!=",NOTEQ);
            }else{
                abort("expecting != but got ! ");
            }
        }
        else if(curChar =='\"'){
            str.pop_back();
            nextChar();
            int start = curPos;
            while(curChar != '\"'){
                if(curChar == '\n' || curChar == '\r' || curChar=='\t' || curChar=='\\'){
                    abort("invalid character in string");
                }
                str.push_back(curChar);
                nextChar();
            }
            token = Token(str,STRING);
        }
        // 203
        // 203.44
        // 203.4.5
        // 9.332
        else if(isDigit(curChar)){
            while(isDigit(peek())){
                str.push_back(curChar);
                nextChar();
            }
            if(peek()=='.'){
                str.push_back(curChar);
                nextChar();
                while(isDigit(peek())){
                    str.push_back(curChar);
                    nextChar();
                }
            }
            str.push_back(curChar);
            token = Token(str,NUMBER);
        }
        // PRINT LET
        // a num bandzo3d
        else if(isAlpha(curChar)){
            while(isAlphaNum(peek())){
                str.push_back(curChar);
                nextChar();
            }
            str.push_back(curChar);
            token = Token(str,checkIfKeyword(str));
        }
        else{
            abort("Unknown token ");
        }
        nextChar();
        return token;
    }

    //
};
#endif