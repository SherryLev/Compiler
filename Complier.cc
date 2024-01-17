#include <iostream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <cstring>
#include <map>

std::map<std::string, int > paramCount;
std::map<std::string,std::map< std::string, int>> procedureTable;

int value = 4;
char uniqueOne = 65;
char uniqueTwo = 65;
char uniqueThree = 65;
int n;

class Tree{
    public:
    std::string rule;
    std::string type;
    std::string proc;
    std::vector< Tree *> children;

    Tree(std::string cRule);
    ~Tree();
};

Tree::Tree(std::string cRule){
    rule = cRule;
}

Tree::~Tree(){
    for(int i = 0; i< children.size(); i++){
        delete children[i];
    }
}

Tree *buildTree(){
    // do smth before visiting children 
    std::istream& in = std::cin;
    std::string s;
    std::getline(in, s);
    std::string currentRule;
    std::istringstream line(s);

    line >> currentRule;
    Tree *tempTree = new Tree(currentRule);
    bool isUpper = true;
    //check upper case
    if(currentRule != ""){
        int size = currentRule.length();
        for(int i = 0; i < size; i++){
            if(!isupper(currentRule[i])){
                isUpper = false;
            }
        }
    }
    // if not leaf
    if(isUpper == false){
        while(line >> currentRule){
            if(currentRule != ".EMPTY"){

                if(currentRule == ":"){
                    std::string ruleType;
                    line >> ruleType;
                    tempTree->type = ruleType;
                } else{
                    Tree *tempChild = buildTree();
                    tempTree->children.push_back(tempChild);
                }
            }
        }
    } else {
        std::string secondWord;
        line >> secondWord;
        
        Tree *tempChild = new Tree(secondWord);
        if(currentRule == "ID" || currentRule == "NUM"){
            std::string thirdWord;
            std::string ruleType;
            line >> thirdWord;
            if(thirdWord == ":"){
                line >> ruleType;
                tempTree->type = ruleType;
            }
        }
        tempTree->children.push_back(tempChild);
        //std::cout<< "HERE 1" << std::endl; 
    }
    return tempTree;
}

void addParam(Tree *t){
    std::vector<std::string> currentLine;
    currentLine.push_back(t->rule);
    for(Tree *child: t->children){
        currentLine.push_back(child->rule);
    }

    if((currentLine[0] == "dcls" && t->children.size() < 1) || (currentLine[0] == "statements" && t->children.size() < 1) ||
        (currentLine[0] == "params" && t->children.size() < 1) ){
        return;
    }

    if (currentLine[0] == "main" && currentLine[1] == "INT" && currentLine[2] == "WAIN" && currentLine[3] == "LPAREN"
       && currentLine[4] == "dcl" && currentLine[5] == "COMMA" && currentLine[6] == "dcl"
       && currentLine[7] == "RPAREN" && currentLine[8] == "LBRACE" && currentLine[9] == "dcls"
       && currentLine[10] == "statements" && currentLine[11] == "RETURN"
        && currentLine[12] == "expr" && currentLine[13] == "SEMI" && currentLine[14] == "RBRACE"){
         t->proc = "wain";

    } else if(currentLine[0] == "procedure" && currentLine[1] == "INT" && currentLine[2] == "ID"
            && currentLine[3] == "LPAREN" && currentLine[4] == "params" && currentLine[5] == "RPAREN"
            && currentLine[6] == "LBRACE" && currentLine[7] == "dcls" && currentLine[8] == "statements"
            && currentLine[9] == "RETURN" && currentLine[10] == "expr" && currentLine[11] == "SEMI"
            && currentLine[12] == "RBRACE"){
                t->proc = t->children[1]->children[0]->rule;
    }


    for(int i = 0; i< currentLine.size() - 1; i++){
        t->children[i]->proc = t->proc;
    }

    for(int i = 0; i< currentLine.size() - 1; i++){
       addParam(t->children[i]);
    }

}

int countParam(const Tree *t){
    //int total = 0;
    std::vector<std::string> currentLine;
    currentLine.push_back(t->rule);
    for(Tree *child: t->children){
        currentLine.push_back(child->rule);
    }
    //std::cout << "the current line is" << currentLine[0] << std::endl;
    if (currentLine[0] == "params" && t->children.size() < 1 ){
        return 0;
    }
    if (currentLine[0] == "params" && currentLine[1] == "paramlist"){
        return countParam(t->children[0]);
    } else if (currentLine[0] == "paramlist" && currentLine[1] == "dcl" && (currentLine.size()== 2)){
       return 1;
    } else if (currentLine[0] == "paramlist" && currentLine[1] == "dcl" && currentLine[2] == "COMMA"
        && currentLine[3] == "paramlist"){
        return countParam(t->children[2])+ 1;
    }
}

void translateTree(const Tree *t){
    std::vector<std::string> currentLine;
    currentLine.push_back(t->rule);

    for(Tree *child: t->children){
        currentLine.push_back(child->rule);
    }
    if((currentLine[0] == "dcls" && t->children.size() < 1) || (currentLine[0] == "statements" && t->children.size() < 1) ||
        (currentLine[0] == "params" && t->children.size() < 1)){
            return;
    }
    if(currentLine[0] == "start" && currentLine[1] == "BOF" && currentLine[2] == "procedures" && currentLine[3] == "EOF"){
        //std::cout<< "MAYBE 16" << std::endl;
        translateTree(t->children[1]);
    } else if(currentLine[0] == "procedures" && currentLine[1] == "main"){
        //std::cout<< "MAYBE 17" << std::endl;
        translateTree(t->children[0]);
    } else if(currentLine[0] == "main" && currentLine[1] == "INT" && currentLine[2] == "WAIN" && currentLine[3] == "LPAREN"
       && currentLine[4] == "dcl" && currentLine[5] == "COMMA" && currentLine[6] == "dcl"
       && currentLine[7] == "RPAREN" && currentLine[8] == "LBRACE" && currentLine[9] == "dcls"
       && currentLine[10] == "statements" && currentLine[11] == "RETURN"
        && currentLine[12] == "expr" && currentLine[13] == "SEMI" && currentLine[14] == "RBRACE"){
        std::string keyA;
        keyA = t->children[3]->children[1]->children[0]->rule;
        std::string keyB;
        keyB = t->children[5]->children[1]->children[0]->rule;
        std::string param = t->children[3]->children[0]->proc;
        int offset;
        offset = procedureTable[param].size() * (-4);
        procedureTable[param][keyA] = offset;
        offset = procedureTable[param].size() * (-4);
        procedureTable[param][keyB] = offset;
        std::cout << ".import init" << std::endl;
        std::cout << ".import new" << std::endl;
        std::cout << ".import delete" << std::endl;
        std::cout << ".import print" << std::endl;
        std::cout << "lis $11" << std::endl;
        std::cout << ".word 1" << std::endl;
        std::cout << "lis $4" << std::endl;
        std::cout << ".word 4" << std::endl;
        std::cout << "sub $29, $30, $4" << std::endl;
        std::cout << "sw $1, 0($29)" << std::endl;
        std::cout << "sub $30, $30, $4" << std::endl;
        std::cout << "sw $2, -4($29)" << std::endl;
        std::cout << "sub $30, $30, $4" << std::endl;
        //loading init 
        if(t->children[3]->children[1]->type == "int"){
            std::cout << "add $2, $0, $0" << std::endl;
            }
        std::cout << "sw $31, -4($30)" << std::endl;
        std::cout << "sub $30, $30, $4" << std::endl;
        std::cout << "lis $5" << std::endl;
        std::cout << ".word init" << std::endl;
        std::cout << "jalr $5" << std::endl;
        std::cout << "add $30, $30, $4" << std::endl;
        std::cout << "lw $31, -4($30)" << std::endl;
        //finish loading init
        translateTree(t->children[8]);
        translateTree(t->children[9]);
        translateTree(t->children[11]);
        for(int i = 0; i < procedureTable["wain"].size(); i++){
            std::cout << "add $30, $30, $4" << std::endl;
        }
        std::cout << "jr $31" << std::endl;
    } else if(currentLine[0] == "dcl" && currentLine[1] == "type" && currentLine[2] == "ID" ){
        std::string param = t->proc;
        std::string key;
        key = t->children[1]->children[0]->rule;
        int offset = procedureTable[param].size() * (-4);
        //offsetTable.insert(std::pair<std::string, int>(key,value));
        procedureTable[param][key] = offset;
        translateTree(t->children[0]);
    } else if (currentLine[0] == "expr" && currentLine[1] == "term"){
        translateTree(t->children[0]);
    } else if(currentLine[0] == "term" && currentLine[1] == "factor"){
        translateTree(t->children[0]);
    } else if(currentLine[0] == "factor" && currentLine[1] == "NUM"){
        std::cout << "lis $9" << std::endl;
        std::cout << ".word " << t->children[0]->children[0]->rule << std::endl;
        std::cout << "add $3, $9, $0" << std::endl;
    } else if(currentLine[0] == "factor" && currentLine[1] == "ID" && (currentLine.size() == 2)){
        int displayNum;
        std::string param = t->proc;
        displayNum = procedureTable[param][t->children[0]->children[0]->rule];
        /*if (offsetTable.find(t->children[0]->children[0]->rule) !=  offsetTable.end()){
            displayNum = offsetTable[t->children[0]->children[0]->rule];
        }*/
        //std::cout << "HERE LW $3" << std::endl;
        std::cout << "lw $3, "<< displayNum << "($29)"<< std::endl;
    } else if(currentLine[0] == "factor" && currentLine[1] == "LPAREN" && currentLine[2] == "expr" && currentLine[3] == "RPAREN"){
        translateTree(t->children[1]);
    } else if (currentLine[0] == "dcls" && currentLine[1] == "dcls" && currentLine[2] == "dcl" && currentLine[3] == "BECOMES"
        && currentLine[4] == "NUM" && currentLine[5] == "SEMI"){
            translateTree(t->children[0]);
            //value -= 4;
            std::string keyC = t->children[1]->children[1]->children[0]->rule;
            std::string param = t->children[1]->proc;
            int offset = (procedureTable[param].size() - paramCount[param]) * (-4);
            // std::cout << "the offset is: " << offset << std::endl;
            //offsetTable.insert(std::pair<std::string, int>(keyC, value));
            procedureTable[param][keyC] = offset;
            /*if (offsetTable.find(t->children[3]->children[0]->rule) !=  offsetTable.end()){
                offset = offsetTable[t->children[3]->children[0]->rule];
            }*/
            std::cout << "lis $13" << std::endl;
            std::cout << ".word " << t->children[3]->children[0]->rule << std::endl;
            std::cout << "add $3, $13, $0" << std::endl;
            std::cout << "sw $3, " << offset << "($29)" << std::endl;
            std::cout << "sub $30, $30, $4" << std::endl;

    } else if(currentLine[0] == "statements" && currentLine[1] == "statements" && currentLine[2] == "statement"){
        translateTree(t->children[0]);
        translateTree(t->children[1]);
    } else if(currentLine[0] == "statement" && currentLine[1] == "lvalue" && currentLine[2] == "BECOMES" &&
        currentLine[3] == "expr" && currentLine[4] == "SEMI"){
            if(t->children[0]->children[0]->rule == "ID"){
                translateTree(t->children[2]);
                int displayNumA;
                std::string param = t->children[2]->proc;
                displayNumA = procedureTable[param][t->children[0]->children[0]->children[0]->rule];
                /*if (offsetTable.find(t->children[0]->children[0]->children[0]->rule) !=  offsetTable.end()){
                    displayNumA = offsetTable[t->children[0]->children[0]->children[0]->rule];
                }*/
                std::cout<< "sw $3, " << displayNumA << "($29)"<< std::endl;
            } else if (t->children[0]->children[0]->rule == "STAR"){
                //std::cout << "WENT INTO STAR" << std::endl;
                translateTree(t->children[2]);
                std::cout << "sw $3, -4($30)" << std::endl;
                std::cout << "sub $30, $30, $4" << std::endl;
                translateTree(t->children[0]->children[1]);
                std::cout << "add $30, $30, $4" << std::endl;
                std::cout << "lw $5, -4($30)" << std::endl;
                std::cout << "sw $5, 0($3)" << std::endl;
            }
    } else if(currentLine[0] == "lvalue" && currentLine[1] == "LPAREN" && currentLine[2] == "lvalue" && currentLine[3]== "RPAREN"){
        translateTree(t->children[0]);
         } else if (currentLine[0] == "expr" && currentLine[1] == "expr" && currentLine[2] == "PLUS" && currentLine[3]== "term"){
        if(t->children[0]->children[0]->type == "int" && t->children[2]->children[0]->type == "int"){
            translateTree(t->children[0]);
            //push ($3)
            std::cout<< "sw $3, -4($30)"  << std::endl;
            std::cout << "sub $30, $30, $4" << std::endl;
            translateTree(t->children[2]);
            //pop($5)
            std::cout << "add $30, $30, $4" << std::endl;
            std::cout << "lw $5, -4($30)" << std::endl;  //maybe 29
            std::cout << "add $3, $5, $3" << std::endl;
        } else if (t->children[0]->children[0]->type == "int*" && t->children[2]->children[0]->type == "int"){
            translateTree(t->children[0]);
            //push ($3)
            std::cout<< "sw $3, -4($30)"  << std::endl;
            std::cout << "sub $30, $30, $4" << std::endl;
            translateTree(t->children[2]);
            std::cout << "mult $3, $4" << std::endl;
            std::cout << "mflo $3" << std::endl;
            //pop($5)
            std::cout << "add $30, $30, $4" << std::endl;
            std::cout << "lw $5, -4($30)" << std::endl;
            std::cout << "add $3, $5, $3" << std::endl;
        } else if(t->children[0]->children[0]->type == "int" && t->children[2]->children[0]->type == "int*"){
            translateTree(t->children[2]);
            //push ($3)
            std::cout<< "sw $3, -4($30)"  << std::endl;
            std::cout << "sub $30, $30, $4" << std::endl;
            translateTree(t->children[0]);
            std::cout << "mult $3, $4" << std::endl;
            std::cout << "mflo $3" << std::endl;
            //pop($5)
            std::cout << "add $30, $30, $4" << std::endl;
            std::cout << "lw $5, -4($30)" << std::endl;
            std::cout << "add $3, $5, $3" << std::endl;
        }
    } else if (currentLine[0] == "expr" && currentLine[1] == "expr" && currentLine[2] == "MINUS" && currentLine[3]== "term"){
        if(t->children[0]->children[0]->type == "int" && t->children[2]->children[0]->type == "int"){
            translateTree(t->children[0]);
            //push ($3)
            std::cout<< "sw $3, -4($30)"  << std::endl;
            std::cout << "sub $30, $30, $4" << std::endl;
            translateTree(t->children[2]);
            //pop($5)
            std::cout << "add $30, $30, $4" << std::endl;
            std::cout << "lw $5, -4($30)" << std::endl;  //maybe 29
            std::cout << "add $3, $5, $3" << std::endl;
        } else if (t->children[0]->children[0]->type == "int*" && t->children[2]->children[0]->type == "int"){
            translateTree(t->children[0]);
            //push ($3)
            std::cout<< "sw $3, -4($30)"  << std::endl;
            std::cout << "sub $30, $30, $4" << std::endl;
            translateTree(t->children[2]);
            std::cout << "mult $3, $4" << std::endl;
            std::cout << "mflo $3" << std::endl;
            //pop($5)
            std::cout << "add $30, $30, $4" << std::endl;
            std::cout << "lw $5, -4($30)" << std::endl;
            std::cout << "add $3, $5, $3" << std::endl;
        } else if(t->children[0]->children[0]->type == "int" && t->children[2]->children[0]->type == "int*"){
            translateTree(t->children[2]);
            //push ($3)
            std::cout<< "sw $3, -4($30)"  << std::endl;
            std::cout << "sub $30, $30, $4" << std::endl;
            translateTree(t->children[0]);
            std::cout << "mult $3, $4" << std::endl;
            std::cout << "mflo $3" << std::endl;
            //pop($5)
            std::cout << "add $30, $30, $4" << std::endl;
            std::cout << "lw $5, -4($30)" << std::endl;
            std::cout << "add $3, $5, $3" << std::endl;
        }
    } else if (currentLine[0] == "expr" && currentLine[1] == "expr" && currentLine[2] == "MINUS" && currentLine[3]== "term"){
        if(t->children[0]->children[0]->type == "int" && t->children[2]->children[0]->type == "int"){
            translateTree(t->children[0]);
            //push ($3)
            std::cout<< "sw $3, -4($30)"  << std::endl;
            std::cout << "sub $30, $30, $4" << std::endl;
            translateTree(t->children[2]);
            //pop($5)
            std::cout << "add $30, $30, $4" << std::endl;
            std::cout << "lw $5, -4($30)" << std::endl; //maybe 29
            std::cout << "sub $3, $5, $3" << std::endl;
        } else if (t->children[0]->children[0]->type == "int*" && t->children[2]->children[0]->type == "int"){
            translateTree(t->children[0]);
            //push ($3)
            std::cout<< "sw $3, -4($30)"  << std::endl;
            std::cout << "sub $30, $30, $4" << std::endl;
            translateTree(t->children[2]);
            std::cout << "mult $3, $4" << std::endl;
            std::cout << "mflo $3" << std::endl;
            //pop($5)
            std::cout << "add $30, $30, $4" << std::endl;
            std::cout << "lw $5, -4($30)" << std::endl;
            std::cout << "sub $3, $5, $3" << std::endl;
        } else if(t->children[0]->children[0]->type == "int*" && t->children[2]->children[0]->type == "int*"){
            translateTree(t->children[0]);
             //push ($3)
            std::cout<< "sw $3, -4($30)"  << std::endl;
            std::cout << "sub $30, $30, $4" << std::endl;
            translateTree(t->children[2]);
            //pop($5)
            std::cout << "add $30, $30, $4" << std::endl;
            std::cout << "lw $5, -4($30)" << std::endl;
            std::cout << "sub $3, $5, $3" << std::endl;
            std::cout << "div $3, $4" << std::endl;
            std::cout << "mflo $3" << std::endl;
        }
    } else if (currentLine[0] == "term" && currentLine[1] == "term" && currentLine[2] == "STAR" && currentLine[3]== "factor"){
        translateTree(t->children[0]);
        //push ($3)
        std::cout<< "sw $3, -4($30)"  << std::endl;    // maybe 29 
        std::cout << "sub $30, $30, $4" << std::endl;
        translateTree(t->children[2]);
        //pop($5)
        std::cout << "add $30, $30, $4" << std::endl;
        std::cout << "lw $5, -4($30)" << std::endl; //maybe 29
        std::cout << "div $5, $3" << std::endl;
        std::cout << "mfhi $3" << std::endl;
    } else if(currentLine[0] == "statement" && currentLine[1] == "IF" && currentLine[2] == "LPAREN"
        && currentLine[3] == "test" && currentLine[4] == "RPAREN" && currentLine[5] == "LBRACE"
        && currentLine[6] == "statements" && currentLine[7] == "RBRACE" && currentLine[8] == "ELSE"
        && currentLine[9] == "LBRACE" && currentLine[10] == "statements" && currentLine[11] == "RBRACE"){
        // save the unique else at the top here so that the same one is used through out that entire if statement 
        char c = uniqueOne;
        uniqueOne++;
        translateTree(t->children[2]);
        std::cout<< "beq $3, $0, else" << c << std::endl;
        translateTree(t->children[5]);
        std::cout<< "beq $0, $0, endif" << c << std::endl;
        std::cout << "else" << c << ":" << std::endl;
        translateTree(t->children[9]);
        std::cout << "endif" << c << ":" << std::endl;

    } else if (currentLine[0] == "statement" && currentLine[1] == "WHILE" && currentLine[2] == "LPAREN"
        && currentLine[3] == "test" && currentLine[4] == "RPAREN" && currentLine[5] == "LBRACE" && currentLine[6] == "statements"
        && currentLine[7] == "RBRACE"){
        char c = uniqueTwo;
        uniqueTwo++;
        std::cout << "loop" << c << ":" << std::endl;
        translateTree(t->children[2]);
        std::cout << "beq $3, $0, endWhile" << c << std::endl;
        translateTree(t->children[5]);
        std::cout << "beq $0, $0, loop" << c << std::endl;
        std::cout << "endWhile" << c << ":" << std::endl;

    } else if (currentLine[0] == "test" && currentLine[1] == "expr" && currentLine[2] == "EQ" && currentLine[3]== "expr"){
        if(t->children[0]->type == "int"){
            translateTree(t->children[0]);
            //push ($3)
            std::cout<< "sw $3, -4($30)"  << std::endl;    // maybe 29 
            std::cout << "sub $30, $30, $4" << std::endl;
            translateTree(t->children[2]);
            //pop($5)
            std::cout << "add $30, $30, $4" << std::endl;
            std::cout << "lw $5, -4($30)" << std::endl; //maybe 29
            std::cout << "slt $6, $3, $5" << std::endl;
            std::cout << "slt $7, $5, $3" << std::endl;
            std::cout << "add $3, $6, $7" << std::endl;
            std::cout << "sub $3, $11, $3" << std::endl;
        } else if (t->children[0]->type == "int*"){
            translateTree(t->children[0]);
            //push ($3)
            std::cout<< "sw $3, -4($30)"  << std::endl;    // maybe 29 
            std::cout << "sub $30, $30, $4" << std::endl;
            translateTree(t->children[2]);
            //pop($5)
            std::cout << "add $30, $30, $4" << std::endl;
            std::cout << "lw $5, -4($30)" << std::endl; //maybe 29
            std::cout << "sltu $6, $3, $5" << std::endl;
            std::cout << "sltu $7, $5, $3" << std::endl;
            std::cout << "add $3, $6, $7" << std::endl;
            std::cout << "sub $3, $11, $3" << std::endl;
        }
    } else if (currentLine[0] == "test" && currentLine[1] == "expr" && currentLine[2] == "NE" && currentLine[3]== "expr"){
        if(t->children[0]->type == "int"){
            translateTree(t->children[0]);
            //push ($3)
            std::cout<< "sw $3, -4($30)"  << std::endl;    // maybe 29 
            std::cout << "sub $30, $30, $4" << std::endl;
            translateTree(t->children[2]);
            //pop($5)
            std::cout << "add $30, $30, $4" << std::endl;
            std::cout << "lw $5, -4($30)" << std::endl; //maybe 29
            std::cout << "slt $6, $3, $5" << std::endl;
            std::cout << "slt $7, $5, $3" << std::endl;
            std::cout << "add $3, $6, $7" << std::endl;
        } else if (t->children[0]->type == "int*"){
            translateTree(t->children[0]);
            //push ($3)
            std::cout<< "sw $3, -4($30)"  << std::endl;    // maybe 29 
            std::cout << "sub $30, $30, $4" << std::endl;
            translateTree(t->children[2]);
            //pop($5)
            std::cout << "add $30, $30, $4" << std::endl;
            std::cout << "lw $5, -4($30)" << std::endl; //maybe 29
            std::cout << "sltu $6, $3, $5" << std::endl;
            std::cout << "sltu $7, $5, $3" << std::endl;
            std::cout << "add $3, $6, $7" << std::endl;
        }
    } else if (currentLine[0] == "test" && currentLine[1] == "expr" && currentLine[2] == "LT" && currentLine[3]== "expr"){
        if(t->children[0]->type == "int"){
            translateTree(t->children[0]);
            std::cout<< "sw $3, -4($30)"  << std::endl;    // maybe 29 
            std::cout << "sub $30, $30, $4" << std::endl;
            translateTree(t->children[2]);
            //pop($5)
            std::cout << "add $30, $30, $4" << std::endl;
            std::cout << "lw $5, -4($30)" << std::endl; //maybe 29
            std::cout << "slt $3, $5, $3" << std::endl;
        } else if (t->children[0]->type == "int*"){
            translateTree(t->children[0]);
            std::cout<< "sw $3, -4($30)"  << std::endl;    // maybe 29 
            std::cout << "sub $30, $30, $4" << std::endl;
            translateTree(t->children[2]);
            //pop($5)
            std::cout << "add $30, $30, $4" << std::endl;
            std::cout << "lw $5, -4($30)" << std::endl; //maybe 29
            std::cout << "sltu $3, $5, $3" << std::endl;
        }
    } else if (currentLine[0] == "test" && currentLine[1] == "expr" && currentLine[2] == "LE" && currentLine[3]== "expr"){
        if(t->children[0]->type == "int"){
            translateTree(t->children[0]);
            std::cout<< "sw $3, -4($30)"  << std::endl;    // maybe 29 
            std::cout << "sub $30, $30, $4" << std::endl;
            translateTree(t->children[2]);
            //pop($5)
            std::cout << "add $30, $30, $4" << std::endl;
            std::cout << "lw $5, -4($30)" << std::endl; //maybe 29
            std::cout << "slt $3, $3, $5" << std::endl;
            std::cout << "sub $3, $11, $3" << std::endl;  // !(a > b)
        }else if (t->children[0]->type == "int*"){
            translateTree(t->children[0]);
            std::cout<< "sw $3, -4($30)"  << std::endl;    // maybe 29 
            std::cout << "sub $30, $30, $4" << std::endl;
            translateTree(t->children[2]);
            //pop($5)
            std::cout << "add $30, $30, $4" << std::endl;
            std::cout << "lw $5, -4($30)" << std::endl; //maybe 29
            std::cout << "sltu $3, $3, $5" << std::endl;
            std::cout << "sub $3, $11, $3" << std::endl;  // !(a > b)
        }
        } else if (currentLine[0] == "test" && currentLine[1] == "expr" && currentLine[2] == "GE" && currentLine[3]== "expr"){
        if(t->children[0]->type == "int"){
            translateTree(t->children[0]);
            std::cout<< "sw $3, -4($30)"  << std::endl;    // maybe 29 
            std::cout << "sub $30, $30, $4" << std::endl;
            translateTree(t->children[2]);
            //pop($5)
            std::cout << "add $30, $30, $4" << std::endl;
            std::cout << "lw $5, -4($30)" << std::endl; //maybe 29
            std::cout << "slt $3, $5, $3" << std::endl;
            std::cout << "sub $3, $11, $3" << std::endl; // !(a < b)
        }else if (t->children[0]->type == "int*"){
            translateTree(t->children[0]);
            std::cout<< "sw $3, -4($30)"  << std::endl;    // maybe 29 
            std::cout << "sub $30, $30, $4" << std::endl;
            translateTree(t->children[2]);
            //pop($5)
            std::cout << "add $30, $30, $4" << std::endl;
            std::cout << "lw $5, -4($30)" << std::endl; //maybe 29
            std::cout << "sltu $3, $5, $3" << std::endl;
            std::cout << "sub $3, $11, $3" << std::endl; // !(a < b)
        }
    } else if (currentLine[0] == "test" && currentLine[1] == "expr" && currentLine[2] == "GT" && currentLine[3]== "expr") {
       if(t->children[0]->type == "int"){
        translateTree(t->children[0]);
        std::cout<< "sw $3, -4($30)"  << std::endl;    // maybe 29 
        std::cout << "sub $30, $30, $4" << std::endl;
        translateTree(t->children[2]);
         //pop($5)
        std::cout << "add $30, $30, $4" << std::endl;
        std::cout << "lw $5, -4($30)" << std::endl; //maybe 29
        std::cout << "slt $3, $3, $5" << std::endl;
       } else if (t->children[0]->type == "int*"){
           translateTree(t->children[0]);
            std::cout<< "sw $3, -4($30)"  << std::endl;    // maybe 29 
            std::cout << "sub $30, $30, $4" << std::endl;
            translateTree(t->children[2]);
            //pop($5)
            std::cout << "add $30, $30, $4" << std::endl;
            std::cout << "lw $5, -4($30)" << std::endl; //maybe 29
            std::cout << "sltu $3, $3, $5" << std::endl;
       }
    } else if (currentLine[0] == "statement" && currentLine[1] == "PRINTLN" && currentLine[2] == "LPAREN"
        && currentLine[3] == "expr" && currentLine[4] == "RPAREN" && currentLine[5] == "SEMI"){
        std::cout<< "sw $1, -4($30)"  << std::endl;    // maybe 29 
        std::cout << "sub $30, $30, $4" << std::endl;
        translateTree(t->children[2]);
        std::cout << "add $1, $3, $0" << std::endl;
        std::cout << "sw $31, -4($30)"  << std::endl;    // maybe 29 
        std::cout << "sub $30, $30, $4" << std::endl;
        std::cout << "lis $5" << std::endl;
        std::cout << ".word print" << std::endl;
        std::cout << "jalr $5" << std::endl;
        std::cout << "add $30, $30, $4" << std::endl;
        std::cout << "lw $31, -4($30)" << std::endl;
        std::cout << "add $30, $30, $4" << std::endl;
        std::cout << "lw $1, -4($30)" << std::endl;
    } else if (currentLine[0] == "procedures" && currentLine[1] == "procedure" && currentLine[2] == "procedures"){
        //countNumParam(t->children[0]->children[3], n);
        n = countParam(t->children[0]->children[3]);
        //std::cout << "the value of n is: " << n << std::endl;
        //std::cout <<  "the param is" <<t->children[0]->children[1]->children[0]->rule << std::endl;
        paramCount[t->children[0]->children[1]->children[0]->rule]= n;
        translateTree(t->children[1]);
        translateTree(t->children[0]);
    } else if (currentLine[0] == "params" && currentLine[1] == "paramlist"){
        translateTree(t->children[0]);
    } else if (currentLine[0] == "procedure" && currentLine[1] == "INT" && currentLine[2] == "ID"
        && currentLine[3] == "LPAREN" && currentLine[4] == "params" && currentLine[5] == "RPAREN"
        && currentLine[6] == "LBRACE" && currentLine[7] == "dcls" && currentLine[8] == "statements"
        && currentLine[9] == "RETURN" && currentLine[10] == "expr" && currentLine[11] == "SEMI"
        && currentLine[12] == "RBRACE"){
        std::cout << "S" << t->children[1]->children[0]->rule <<":" << std::endl;
        std::cout << "sub $29, $30, $4" << std::endl; // maybe take out
        translateTree(t->children[3]);
        translateTree(t->children[6]);
        translateTree(t->children[7]);
        translateTree(t->children[9]);
        std::string param = t->children[1]->children[0]->rule;
        int value = procedureTable[t->children[1]->children[0]->rule].size() - paramCount[param];
        //std::cout << "the value that needs to be outputted is: " << value << std::endl;
        for( int  i = 0; i < value; i++){
            std::cout << "add $30, $30, $4" << std::endl;
            std::cout << "lw $5, -4($30)" << std::endl;
        }
        std::cout << "jr $31" << std::endl;
    } else if (currentLine[0] == "paramlist" && currentLine[1] == "dcl" && (currentLine.size() < 3)){
        std::string keyB = t->children[0]->children[1]->children[0]->rule;
        std::string param = t->proc;
        int offset;
        offset = 4 * (paramCount[param] - procedureTable[param].size());
        //std::cout << "the offset is in paramlist :" <<offset << std::endl;
        //std::cout << "the offset is: " << offset << std::endl;
        procedureTable[param][keyB] = offset;
    } else if (currentLine[0] == "paramlist" && currentLine[1] == "dcl" && currentLine[2] == "COMMA"
        && currentLine[3] == "paramlist"){
        // build map in here for paramlist 
        std::string keyB = t->children[0]->children[1]->children[0]->rule;
        std::string param = t->proc;
        int offset;
        offset = 4 * (paramCount[param] - procedureTable[param].size()) ;
        //std::cout << "the offset is: " << offset << std::endl;
        procedureTable[param][keyB] = offset;
        translateTree(t->children[2]);
    } else if (currentLine[0] == "factor" && currentLine[1] == "ID" && currentLine[2] == "LPAREN"
        && currentLine[3] == "RPAREN"){
        //std::cout << "HERE 4" << std::endl;
        std::cout << "sw $29, -4($30)" << std::endl;
        std::cout << "sub $30, $30, $4" << std::endl;
        std::cout << "sw $31, -4($30)" << std::endl;
        std::cout << "sub $30, $30, $4" << std::endl;
        std::cout << "lis $5" << std::endl;
        std::cout << ".word S" << t->children[0]->children[0]->rule << std::endl;
        std::cout << "jalr $5" << std::endl;
        std::cout << "add $30, $30, $4" << std::endl;
        std::cout << "lw $31, -4($30)" << std::endl;
        std::cout << "add $30, $30, $4" << std::endl;
        std::cout << "lw $29, -4($30)" << std::endl;
    } else if (currentLine[0] == "factor" && currentLine[1] == "ID" && currentLine[2] == "LPAREN"
        && currentLine[3] == "arglist" && currentLine[4] == "RPAREN"){
         //std::cout << "HERE 5" << std::endl;
        std::cout << "sw $29, -4($30)" << std::endl;
        std::cout << "sub $30, $30, $4" << std::endl;
        std::cout << "sw $31, -4($30)" << std::endl;
        std::cout << "sub $30, $30, $4" << std::endl;
        translateTree(t->children[2]);
        std::cout << "lis $5" << std::endl;
        std::cout << ".word S" << t->children[0]->children[0]->rule << std::endl;
        std::cout << "jalr $5" << std::endl;
        n = paramCount[t->children[0]->children[0]->rule];
        //std::cout << "the n value is:" << n << std::endl;
        for(int i = 0; i < n; i++){
           std::cout << "add $30, $30, $4" << std::endl;
           std::cout << "lw $5, -4($30)" << std::endl;
        }
        std::cout << "add $30, $30, $4" << std::endl;
        std::cout << "lw $31, -4($30)" << std::endl;
        std::cout << "add $30, $30, $4" << std::endl;
        std::cout << "lw $29, -4($30)" << std::endl;

    } else if (currentLine[0] == "arglist" && currentLine[1] == "expr" && (currentLine.size() == 2)){
        translateTree(t->children[0]);
        //std::cout << "add $30, $30, $4"<< std::endl; // maybe not
        std::cout << "sw $3, -4($30)" << std::endl;
        std::cout << "sub $30, $30, $4" << std::endl;
    } else if (currentLine[0] == "arglist" && currentLine[1] == "expr" && currentLine[2] == "COMMA"
        && currentLine[3] == "arglist"){
        translateTree(t->children[0]);
        std::cout << "sw $3, -4($30)" << std::endl;
        std::cout << "sub $30, $30, $4" << std::endl;
        translateTree(t->children[2]);
    } else if (currentLine[0] == "dcls" && currentLine[1] == "dcls" && currentLine[2] == "dcl" && currentLine[3] == "BECOMES"
        && currentLine[4] == "NULL" && currentLine[5] == "SEMI"){
            translateTree(t->children[0]);
            //value -= 4;
            std::string keyC = t->children[1]->children[1]->children[0]->rule;
            std::string param = t->children[1]->proc;
            int offset = (procedureTable[param].size()) * (-4);
            //std::cout << "the offset is: " << offset << std::endl;
            //offsetTable.insert(std::pair<std::string, int>(keyC, value));
            procedureTable[param][keyC] = offset;
            /*if (offsetTable.find(t->children[3]->children[0]->rule) !=  offsetTable.end()){
                offset = offsetTable[t->children[3]->children[0]->rule];
            }*/
            std::cout << "lis $13" << std::endl;
            std::cout << ".word 1" << std::endl;
            std::cout << "add $3, $13, $0" << std::endl;
            std::cout << "sw $3, " << offset << "($29)" << std::endl;
            std::cout << "sub $30, $30, $4" << std::endl;
    } else if (currentLine[0] == "factor" && currentLine[1] == "NULL"){
        std::cout << "add $3, $0, $11" << std::endl;
    } else if(currentLine[0] == "factor" && currentLine[1]== "AMP" && currentLine[2] == "lvalue"){
        //std::cout << "the type is " << t->children[1]->children[0]->rule<< std::endl;
        //std::cout << "the second type is " << t->children[1]->children[0]->rule<< std::endl;
        if(t->children[1]->children[0]->rule == "ID"){
            std::string param = t->proc;
            int offset = procedureTable[param][t->children[1]->children[0]->children[0]->rule];
            std::cout << "lis $3" << std::endl;
            std::cout << ".word " << offset << std::endl;
            std::cout << "add $3, $3, $29" << std::endl;
        } else if(t->children[1]->children[0]->rule == "STAR"){
            translateTree(t->children[1]->children[1]);
        }
    } else if(currentLine[0] == "factor" && currentLine[1]== "STAR" && currentLine[2] == "factor"){
        translateTree(t->children[1]);
        std::cout << "lw $3, 0($3)" << std::endl;
    } else if(currentLine[0] == "factor" && currentLine[1]== "NEW" && currentLine[2] == "INT"
        && currentLine[3] == "LBRACK" && currentLine[4] == "expr" && currentLine[5] == "RBRACK"){
        translateTree(t->children[3]);
        std::cout << "add $1, $3, $0" << std::endl;
        std::cout<< "sw $31, -4($30)"  << std::endl;    // maybe 29 
        std::cout << "sub $30, $30, $4" << std::endl;
        std::cout << "lis $5" << std::endl;
        std::cout << ".word new" << std::endl;
        std::cout << "jalr $5" << std::endl;
        //pop($31)
        std::cout << "add $30, $30, $4" << std::endl;
        std::cout << "lw $31, -4($30)" << std::endl;
        std::cout << "bne $3, $0, 1" << std::endl;
        std::cout << "add $3, $11, $0" << std::endl;
    } else if(currentLine[0] == "statement" && currentLine[1] == "DELETE" && currentLine[2] == "LBRACK"
        && currentLine[3] == "RBRACK" && currentLine[4] == "expr" && currentLine[5] == "SEMI"){
        char c = uniqueOne;
        uniqueOne++;
        translateTree(t->children[3]);
        std::cout << "beq $3, $11, skipDelete" << c << std::endl;
        std::cout << "add $1, $3, $0" << std::endl;
        std::cout << "sw $31, -4($30)"  << std::endl;    // maybe 29 
        std::cout << "sub $30, $30, $4" << std::endl;
        std::cout << "lis $5" << std::endl;
        std::cout << ".word delete" << std::endl;
        std::cout << "jalr $5" << std::endl;
        std::cout << "add $30, $30, $4" << std::endl;
        std::cout << "lw $31, -4($30)" << std::endl;
        std::cout << "skipDelete" << c << ":"<< std::endl;
    }
}
void printTree(const Tree *t){
   // std::cout<< "HERE 7" << std::endl; 
    std::cout<< t->rule;
    std::string display;
    for(Tree *child: t->children){
        std::cout<< " " << child->rule;
    }
    if(t->children.size() == 0){
        std::cout<< " .EMPTY";
    }
    if(t->type != ""){
        std::cout<< " : " << t->type;
    }
    if(t->proc != ""){
        std::cout<< " //" << t->proc;
    }
    std::cout<<"\n";
    for(Tree *child: t->children){
         std::string leafWord = t->rule;
         bool isUpper = true;
        //check upper case
        if(leafWord != ""){
            int size = leafWord.length();
            for(int i = 0; i < size; i++){
                if(!isupper(leafWord[i])){
                    isUpper = false;
                }
            }
        }
        if(!isUpper){
            printTree(child);
        }
    }

}
int main() {
    Tree *mainTree = buildTree();
    addParam(mainTree);
    paramCount["wain"] = 0;
    //printTree(mainTree);
    translateTree(mainTree);
   /*std::cout <<"the procedure table:" << std::endl;
    for (auto it : procedureTable) {
    std::cout << it.first << " : ";
    std::map<std::string, int> &internal_map = it.second;
    for (auto it2: internal_map) {
        
        std::cout << ",";
        std::cout << it2.first << ":" << it2.second;
    }
    std::cout << std::endl;
    }*/
    delete mainTree;
}