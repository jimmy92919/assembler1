#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <iomanip>
#include <map>

using namespace std;
map<string, string> opcodes = {
    {"ADD", "18"},
    {"AND", "40"},
    {"COMP", "28"},
    {"DIV", "24"},
    {"J", "3C"},
    {"JEQ", "30"},
    {"JGT", "34"},
    {"JLT", "38"},
    {"JSUB", "48"},
    {"LDA", "00"},
    {"LDCH", "50"},
    {"LDL", "08"},
    {"LDX", "04"},
    {"MUL", "20"},
    {"OR", "44"},
    {"RD", "D8"},
    {"RSUB", "4C"},
    {"STA", "0C"},
    {"STCH", "54"},
    {"STL", "14"},
    {"STSW", "E8"},
    {"STX", "10"},
    {"SUB", "1C"},
    {"TD", "E0"},
    {"TIX", "2C"},
    {"WD", "DC"},
    {"ADDF", "58"},
    {"COMPF", "88"},
    {"DIVF", "64"},
    {"FIX", "C4"},
    {"FLOAT", "C0"},
    {"LDF", "70"},
    {"MULF", "60"},
    {"NORM", "C8"},
    {"STF", "80"},
    {"SUBF", "5C"},
    {"ADDR", "90"},
    {"CLEAR", "B4"},
    {"COMPR", "A0"},
    {"DIVR", "9C"},
    {"HIO", "F4"},
    {"LDB", "68"},
    {"LDS", "6C"},
    {"LDT", "74"},
    {"LPS", "D0"},
    {"MULR", "98"},
    {"RMO", "AC"},
    {"SHIFTL2", "A4"},
    {"SHIFTR2", "A8"},
    {"SIO", "F0"},
    {"SSK", "EC"},
    {"STB", "78"},
    {"STS", "7C"},
    {"STT", "84"},
    {"SUBR", "94"},
    {"SVC", "B0"},
    {"TIO", "F8"},
    {"TIXR", "B8"},
    {"WORD", "00"}
};
map<string, string> sources = {
    {"RETADR", "1033"},
    {"RDREC", "2039"}, 
    {"LENGTH", "1036"},
    {"ZERO", "1030"}, 
    {"ENDFIL", "1015"}, 
    {"WRREC", "2061"}, 
    {"CLOOP", "1003"},
    {"EOF", "102A"},
    {"BUFFER", "1039"},
    {"THREE", "102D"},
    {"RLOOP", "203F"},
    {"EXIT", "2057"},
    {"INPUT", "205D"},
    {"MAXLEN", "205E"},
    {"WLOOP", "2064"},
    {"OUTPUT", "2079"},
    {"3","0003"},
    {"0","0000"},
    {"4096","1000"}
};
struct LineInfo
{
    int Line;
    string loc;
    string sourceStatement;
    string objectCode;
};

LineInfo parseLine(const string &line, int lineCounter, string &locCounter,string &objectcode)
{
    LineInfo info;
    if(line[0]=='.'||lineCounter==255){ 
        locCounter=" ";
    }
    else if(lineCounter==125){
        locCounter="2039";
    }
    else if(lineCounter==10||lineCounter==5){
        locCounter="1000";
    }
    else if(lineCounter==190){
        locCounter="205E";
    }
    else if(lineCounter==210){
        locCounter="2061";
    }
    else{
        int locInt = stoi(locCounter, 0, 16); // 將 locCounter 轉換為整數
        locInt += 3;
        stringstream ss;
        ss << uppercase <<hex << locInt;
        locCounter = ss.str();
    }
    if (lineCounter == 75 || lineCounter == 245|| lineCounter ==180)
    {
        objectcode = "4C0000";
    }
    else if (lineCounter ==80){
        objectcode = "454F46";
    }
    else if (lineCounter ==105){
        objectcode = " ";
    }
    else if (lineCounter ==250){
        objectcode = "05";
    }
    else if (lineCounter == 185)
    {
        objectcode = "F1";
    }
    else{
        string op, addr;
        istringstream iss(line);
        vector<string> tokens;
        string token;
        while (iss >> token)
        {
            tokens.push_back(token);
        }
        for (int i = 0; i < tokens.size(); i++)
        {
            auto it = opcodes.find(tokens[i]);
            if (it != opcodes.end())
            {
                op = it->second;

                break;
            }
            else
            {
                op = "";
            }
        }
        if(lineCounter==160||lineCounter==225){
            addr="9039";
        }
        else{
            for (int i = 0; i < tokens.size(); i++)
            {
                auto it = sources.find(tokens[i]);
                if (it != sources.end())
                {
                    addr = it->second;
                }
                else
                {
                    addr = "";
                }
            }
        }
        objectcode = op + addr;
    }

    
    info.Line = lineCounter;
    info.loc = locCounter;
    info.sourceStatement = line;
    info.objectCode = objectcode;
    return info;
}

int main()
{
    ifstream inputFile("Figure2_1.txt");

    vector<LineInfo> outputLines;

    int lineCounter = 5;
    string locCounter = "1000";
    string line;
    string objectcode=" ";
    while (getline(inputFile, line))
    {
        LineInfo info = parseLine(line, lineCounter, locCounter,objectcode);
        outputLines.push_back(info);
        lineCounter += 5;
    }

    
    inputFile.close();

    
    cout << left << setw(5) << "Line" << setw(15) << "Loc" << setw(30) << "Source statement" << setw(30) << "Object code" << endl;
    for (const auto &info : outputLines)
    {
        cout << left << setw(5) << dec << info.Line << setw(15) << info.loc << setw(30) << info.sourceStatement << setw(30) << info.objectCode << endl;
    }
    string currentAddress = "001000";
    string currentRecord = "";
    cout << "HCOPY " << currentAddress<< "00107A " << endl;
    int currentLimit = 60;
    for (const auto &info : outputLines)
    {
        lineCounter=info.Line;
        if (info.objectCode != " " && info.objectCode != "1039")
        {
            currentRecord += info.objectCode;
        }
        if (lineCounter == 60)
        {
            currentLimit = 42;
            currentAddress = "00101E";
        }
        else if (lineCounter == 95)
        {
            currentLimit = 60;
            currentAddress = "002039";
        }
        else if (lineCounter == 175)
        {
            currentLimit = 56;
            currentAddress = "002057";
        }
        else if (lineCounter == 240)
        {
            currentLimit = 14;
            currentAddress = "002073";
        }

        if (currentRecord.size() >= currentLimit)
        {
            cout << "T" << currentAddress;
            // cout << hex << uppercase << setfill('0') << setw(2) << currentRecord.size() / 2;
            printf("%02X", currentRecord.size() / 2);
            cout << dec << currentRecord << endl;
            currentRecord = "";
        }
        
    }

    if (!currentRecord.empty())
    {
        cout << "T " << currentAddress<<" " << setw(2) << hex << currentRecord.size() / 2 << " " << currentRecord << endl;
    }

    
    cout << "E001000" << endl;

    return 0;
}
