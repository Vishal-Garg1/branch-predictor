#include<iostream>
#include<fstream>  // for file handling
#include<unordered_map>
#include<unordered_set>
#include<vector>
#include<bits/stdc++.h>
using namespace std;

unordered_set<string> branchInstructions;
unordered_set<string> jumpInstructions;
unordered_map<string,string> branchTargetBuffer;
unordered_map<string,bool> historyTableOneBit;  // starts from not taken
unordered_map<string,bitset<2>> historyTableTwoBit; // starts from weakly not taken

string PCtoCompare;
string prevPC;

int alwaysTakenBranches=0;
int alwaysNotTakenBranches=0;
int oneBitDynamicBranchCorrectPredictions=0;
int twoBitDynamicBranchCorrectPredictions=0;
int branches=0;

bool branch=false;

void fillBranchInstructions(){
    vector<string> branchInstructs={"beq","bge","bgeu","blt","bltu","bne","beqz","bnez"};
    vector<string> jumpInstructs={"jal","jalr","j","jr"};  // ret is not included.
    for(int idx=0;idx<branchInstructs.size();idx++){
        branchInstructions.insert(branchInstructs[idx]);
    }
    for(int idx=0;idx<jumpInstructs.size();idx++){
        jumpInstructions.insert(jumpInstructs[idx]);
    }
}

unsigned int hexToInt(string hex){
    unsigned int uint = 0;
    for(int j=0;j<hex.size();j++){
        if(hex[j]<='9'){
            uint=16*uint+hex[j]-'0';
        }
        else{
            uint=16*uint+hex[j]-'a'+10;
        }
    }
    return uint;
}

unsigned int pcInHexToInt(string pc){
    unsigned int intPC = 0;
    for(int j=2;j<10;j++){
        if(pc[j]<='9'){
            intPC=16*intPC+pc[j]-'0';
        }
        else{
            intPC=16*intPC+pc[j]-'a'+10;
        }
    }
    return intPC;
}

string pcInIntToHex(unsigned int intPC){
    string PC="";
    for(int j=0;j<8;j++){
        int dig=intPC%16;
        if(dig<=9){
            PC+=dig+'0';
        }
        else{
            PC+=dig+'a'-10;
        }
        intPC/=16;
    }
    reverse(PC.begin(),PC.end());
    return "0x"+PC;
}

string addOffsetToPC(string pc,int offset){
    unsigned int intPC = pcInHexToInt(pc);
    intPC+=offset;
    string nextPC=pcInIntToHex(intPC);
    return nextPC;
}

void fetch(string line){
    string pc="0x";
    string instruction="";
    string offset;
    int intOffset;
    bool sign;
    int i=0;
    int s=line.size();
    while(i<s and line[i]!='x'){
        i++;
    }
    if(i>=s)    return;
    i++;
    for(int j=i;j<i+8;j++){
        pc+=line[j];
    }
    if(branch){
        if(historyTableTwoBit.find(prevPC)==historyTableTwoBit.end()){
            historyTableTwoBit[prevPC]=1;
        }
        if(pc==PCtoCompare){
            if(historyTableTwoBit[prevPC]==3){
                twoBitDynamicBranchCorrectPredictions++;
            }
            else if(historyTableTwoBit[prevPC]==2){
                twoBitDynamicBranchCorrectPredictions++;
                historyTableTwoBit[prevPC]=3;
            }
            else if(historyTableTwoBit[prevPC]==1){
                historyTableTwoBit[prevPC]=2;
            }
            else if(historyTableTwoBit[prevPC]==0){
                historyTableTwoBit[prevPC]=1;
            }
            if(historyTableOneBit[prevPC]==true){
                oneBitDynamicBranchCorrectPredictions++;
            }
            historyTableOneBit[prevPC]=true;
            alwaysTakenBranches++;
        }
        else{
            if(historyTableTwoBit[prevPC]==0){
                twoBitDynamicBranchCorrectPredictions++;
            }
            else if(historyTableTwoBit[prevPC]==1){
                twoBitDynamicBranchCorrectPredictions++;
                historyTableTwoBit[prevPC]=0;
            }
            else if(historyTableTwoBit[prevPC]==2){
                historyTableTwoBit[prevPC]=1;
            }
            else if(historyTableTwoBit[prevPC]==3){
                historyTableTwoBit[prevPC]=2;
            }
            if(historyTableOneBit[prevPC]==false){
                oneBitDynamicBranchCorrectPredictions++;
            }
            historyTableOneBit[prevPC]=false;
            alwaysNotTakenBranches++;
        }
    }
    while(i<s and line[i]!=')'){
        i++;
    }
    i++;
    while(i<s and line[i]==' '){
        i++;
    }
    while(i<s and line[i]!=' '){
        instruction+=line[i];
        i++;
    }
    if(i>=s)    return;
    if(branchInstructions.find(instruction)!=branchInstructions.end()){
        branch=true;
        while(line[i]!='p'){
            i++;
        }
        while(line[i]!='+' and line[i]!='-'){
            i++;
        }
        (line[i]=='+')?(sign=true):(sign=false);
        while(!isdigit(line[i])){
            i++;
        }
        while(i<s and line[i]!=' '){
            offset+=line[i];
            i++;
        }
        intOffset=stoi(offset);
        if(sign==false){
            intOffset=-intOffset;
        }
        string nextPC=addOffsetToPC(pc,intOffset);
        PCtoCompare=nextPC;
        branchTargetBuffer[pc]=nextPC;
        branches++;
    }
    else if(jumpInstructions.find(instruction)!=jumpInstructions.end()){
        while(line[i]!='p'){
            i++;
        }
        while(line[i]!='+' and line[i]!='-'){
            i++;
        }
        (line[i]=='+')?(sign=true):(sign=false);
        while(line[i]!='x'){
            i++;
        }
        i++;
        while(i<s and line[i]!=' '){
            offset+=line[i];
            i++;
        }
        intOffset=hexToInt(offset);
        if(sign==false){
            intOffset=-intOffset;
        }
        string nextPC=addOffsetToPC(pc,intOffset);
        branchTargetBuffer[pc]=nextPC;
        branches++;
        if(historyTableTwoBit.find(pc)==historyTableTwoBit.end()){
            historyTableTwoBit[pc]=1;
        }
            if(historyTableTwoBit[pc]==3){
                twoBitDynamicBranchCorrectPredictions++;
            }
            else if(historyTableTwoBit[pc]==2){
                twoBitDynamicBranchCorrectPredictions++;
                historyTableTwoBit[pc]=3;
            }
            else if(historyTableTwoBit[pc]==1){
                historyTableTwoBit[pc]=2;
            }
            else if(historyTableTwoBit[pc]==0){
                historyTableTwoBit[pc]=1;
            }
            if(historyTableOneBit[pc]==true){
                oneBitDynamicBranchCorrectPredictions++;
            }
            historyTableOneBit[pc]=true;
            alwaysTakenBranches++;
    }
    else{
        branch=false;
    }
    prevPC=pc;
}

float Accuracy(int a,int b){
    return (float)a/float(b)*100;
}

void printBranchTargetBuffer(){
    cout<<endl<<"Branch target buffer"<<endl;
    for(auto it:branchTargetBuffer){
        cout<<it.first<<" : "<<it.second<<endl;
    }
    return;
}

void printOneBitHistoryTable(){
    cout<<endl<<"History table for one bit branch predictor"<<endl;
    for(auto it:historyTableOneBit){
        cout<<it.first<<" : "<<it.second<<endl;
    }
    return;
}

void printTwoBitHistoryTable(){
    cout<<endl<<"History table for two bit branch predictor"<<endl;
    for(auto it:historyTableTwoBit){
        cout<<it.first<<" : "<<it.second<<endl;
    }
    return;
}

int main(){
    ifstream fin;    // to read a file
    string line;
    fillBranchInstructions();
    fin.open("trace.txt");       // open trace.txt to read
    while(getline(fin,line)){
        fetch(line);
    }
    fin.close();
    cout<<"Always taken Branch Predictor Accuracy = "<<Accuracy(alwaysTakenBranches,branches)<<"%"<<endl;
    cout<<"Always not taken Branch Predictor Accuracy = "<<Accuracy(alwaysNotTakenBranches,branches)<<"%"<<endl;
    cout<<"1-bit dynamic Branch Predictor Accuracy = "<<Accuracy(oneBitDynamicBranchCorrectPredictions,branches)<<"%"<<endl;
    cout<<"2-bit dynamic Branch Predictor Accuracy = "<<Accuracy(twoBitDynamicBranchCorrectPredictions,branches)<<"%"<<endl;
    printBranchTargetBuffer();
    //printOneBitHistoryTable();
    //printTwoBitHistoryTable();
    return 0;
}