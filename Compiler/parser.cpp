#include "stdc++.h"
#include "lexer.cpp"
#include "tokens.cpp"
#include "emitter.cpp"



using namespace std;


#ifndef PARSER
#define PARSER

class Parser{
    public:
    lexer lex;
    Emitter emit;
    Token curToken;
    Token peekToken;
    set<string> symbols;
    set<string> labelsDeclared;
    set<string> labelsGoTo;

    Parser(lexer lex, Emitter &emitt){
        this->lex = lex;
        emit = emitt;
        symbols.clear();
        labelsDeclared.clear();
        labelsGoTo.clear();
        nextToken();
        nextToken();   
    }

    // Return true if the current token matches.
    bool checkToken(Tokens kind){
        return kind == curToken.kind;
    }

    // # Return true if the next token matches.
    bool checkPeek(Tokens kind){
        return kind == peekToken.kind;
    }

    // # Try to match current token. If not, error. Advances the current token.
    void match(Tokens kind){
        if(!checkToken(kind)){
            string str;
            str = "Expexted ";
            str+=kind;
            str+=" got ";
            str+=curToken.kind;
            abort(str);
        }
        nextToken();
    }

    // # Advances the current token.
    void nextToken(){
        curToken = peekToken;
        peekToken = lex.getToken();
    }

    void abort(string message){
        cout << "Error. "<< message;
        exit(1);
    }


    // production rules
    // program ::= {statement}
    void program(){
        cout << "PROGRAM\n";
        emit.headerLine("#include <stdio.h>\n");
        emit.headerLine("int main(){\n");

        while(checkToken(NEWLINE)){
            nextToken();
        }
        while(!checkToken(EEOF)){
            statement();
        }
        for (string val: labelsGoTo){
            if(labelsDeclared.find(val)==labelsDeclared.end()){
                abort("Label not declared: "+val);
            }
        }
        emit.emitLine("return 0;");
        emit.emitLine("}");
        emit.writeFile();
    }

    void statement(){
        // 1. PRINT (expression|string)
        if(checkToken(PRINT)){
            // cout << "STATEMENT-PRINT\n";
            nextToken();
            if(checkToken(STRING)){
                // printf("text\n");
                emit.emitLine("printf(\""+curToken.text+"\\n\");");
                nextToken();
            }
            else{
                //  printf("%.2f\n", (float)(a));
                emit.emit("printf(\"%.2f\\n\",(float)(");
                expression();
                emit.emitLine("));");
            }
        }
        

        // 2. "IF" comparison "THEN" nl {statement} "ENDIF" nl
        else if(checkToken(IF)){
            // cout << "STATEMENT-IF\n";
            nextToken();
            emit.emit("if(");
            comparison();
            match(THEN);
            nl();
            emit.emit("){");
            while(!checkToken(ENDIF)){
                statement();
            }
            match(ENDIF);
            emit.emitLine("}");

        }

        // 3. "WHILE" comparison "REPEAT" {statement} "ENDWHILE"
        else if(checkToken(WHILE)){
            // cout << "STATEMENT-WHILE\n";
            nextToken();
            emit.emit("while(");
            comparison();
            match(REPEAT);
            nl();
            emit.emitLine("){");
            while(!checkToken(ENDWHILE)){
                statement();
            }
            match(ENDWHILE);
            emit.emitLine("}");
        }
        // 4. "LABEL" ident nl
        else if(checkToken(LABEL)){
            // cout << "STATEMENT-LABEL\n";
            nextToken();
            if(labelsDeclared.find(curToken.text)!=labelsDeclared.end()){
                string message;
                message = "Already declared label with the given name "+ curToken.text;
                abort(message);
            }
            labelsDeclared.insert(curToken.text);

            emit.emitLine(curToken.text + ":");
            match(IDENT);
        }
        // 5. "GOTO" ident nl
        else if(checkToken(GOTO)){
            // cout << "STATEMENT-GOTO\n";
            nextToken();
            labelsGoTo.insert(curToken.text);
            emit.emitLine("goto " + curToken.text+';');
            match(IDENT);

        }
        // 6. "LET" ident "=" expression nl
        else if(checkToken(LET)){
            // cout << "STATEMENT-LET\n";
            nextToken();
            if(symbols.find(curToken.text)==symbols.end()){
                symbols.insert(curToken.text);
                emit.headerLine("float "+ curToken.text+";");
            }
            emit.emit(curToken.text + " = ");
            match(IDENT);
            match(EQ);
            expression();
            emit.emitLine(";");
        }

        // 7. "INPUT" ident nl
        else if(checkToken(INPUT)){
            // cout << "STATEMENT-INPUT\n";
            nextToken();
            if(symbols.find(curToken.text)==symbols.end()){
                symbols.insert(curToken.text);
                emit.headerLine("float "+ curToken.text+";");
            }
            emit.emitLine("if(0==scanf(\"%f\",&"+curToken.text+")){");
            emit.emitLine(curToken.text+" = 0;");
            emit.emit("scanf(\"%");
            emit.emitLine("*s\");");
            emit.emitLine("}");
            match(IDENT);
        }

        else{
            string message = "Invalid stmt at " + curToken.text;
            abort(message);
        }
        nl();
    }

    void comparison(){
        // comparison ::= expression (("==" | "!=" | ">" | ">=" | "<" | "<=") expression)+
        // cout << "COMPARISON\n";
        expression();
        // Must be at least one comparison operator and another expression.
        if(comparisonOperator()){
            emit.emit(curToken.text);
            nextToken();
            expression();
        }
        else{
            string message = "Expected comparison operator got " + curToken.text;
            abort(message);
        }
        while(comparisonOperator()){
            emit.emit(curToken.text);
            nextToken();
            expression();
        }

    }

    bool comparisonOperator(){
        // comparisonOperator ::= "==" | "!=" | ">" | ">=" | "<" | "<  
        return checkToken(EQEQ)||checkToken(NOTEQ)||checkToken(GT)||checkToken(LT)||checkToken(GTEQ)||checkToken(LTEQ);
    }

    void expression(){
        // expression ::= term {( "-" | "+" ) term}
        // cout << "EXPRESSION\n";
        term();
        while(checkToken(PLUS)||checkToken(MINUS)){
            emit.emit(curToken.text);
            nextToken();
            term();
        }
    }

    void term(){
        // term ::= unary {( "/" | "*" ) unary}
        // cout << "TERM\n";
        unary();
        while(checkToken(SLASH)||checkToken(ASTERISK)){
            emit.emit(curToken.text);
            nextToken();
            unary();
        }
    }

    void unary(){
        // unary ::= ["+" | "-"] primary
        // cout <<"UNARY\n";
        if(checkToken(PLUS) || checkToken(MINUS)){
            emit.emit(curToken.text);
            nextToken();
        }
        primary();
    }

    void primary(){
        // primary ::= number | ident
        // cout << "PRIMARY " << curToken.text<<"\n";
        if(checkToken(NUMBER)){
            emit.emit(curToken.text);
            nextToken();
        }
        else if(checkToken(IDENT)){
            if(symbols.find(curToken.text)==symbols.end()){
                abort("Symbol not declared "+curToken.text);
            }
            emit.emit(curToken.text);
            nextToken();
        }
        else{
            string message;
            message = "Error: " + curToken.text + " is not a valid primary";
            abort(message);
        }

    }
    void nl(){
        // cout << "NEWLINE\n";
        if(checkToken(NEWLINE)){
            nextToken();
        }
        while(checkToken(NEWLINE)){
            nextToken();
        }
    }

    void debug(){
        cout << curToken.text << " "<<curToken.kind<<"\n";
    }

};
#endif