#include <iostream>
#include <string>
#include <cctype>
#include <print>
#include <algorithm>
#include <vector>
#include <fstream>

using namespace std;

constexpr char sentenceEnders[] = { '.', '?', '!' };

struct counters {
    int charCount = 0;
    int rowCount = 0;
    int wordCount = 0;
    int sentenceCount = 0;
    int numberCount = 0;
};

struct booleans {
    bool sentenceActive = false;
    bool numberActive = false;
    bool rowActive = false;
    bool wordPotential = true;
    void reset() {
        sentenceActive = false;
        numberActive = false;
        rowActive = false;
        wordPotential = true;
    }
};

class SheepCounter {
public:
    counters cntrs;
    int numberSum = 0;
    void processStream(istream& stream);
    void printInfo();
private:
    void processChar(char nextChar);
    booleans bl;
    int activeNmbValue = 0;
};

void SheepCounter::processStream(istream& stream) {
    char c;
    bl.reset();
    for (;;) {
        c = stream.get();
        if (stream.fail()) {
            numberSum = numberSum + activeNmbValue; //Useful, if stream ended with a number
            return;
        }
        processChar(c);
    }
}

void SheepCounter::processChar(char nextChar) {
    cntrs.charCount++; //Chars
    //Numbers
    if (isdigit(nextChar) && !bl.numberActive && bl.wordPotential) {
        bl.numberActive = true;
        cntrs.numberCount++;
        activeNmbValue = nextChar - '0';
    }
    else {
        if (bl.numberActive) {
            if (isdigit(nextChar)) {
                activeNmbValue = (activeNmbValue * 10) + nextChar - '0'; //Horners method
            }
            if (bl.numberActive && !isdigit(nextChar)) {
                bl.numberActive = false;
                numberSum = numberSum + activeNmbValue;
                activeNmbValue = 0;
            }
        }
    }
    //Words (and part of sentences)
    if (isalpha(nextChar) && bl.wordPotential) {
        cntrs.wordCount++;
        bl.sentenceActive = true;
    }
    if (isalnum(nextChar)) {
        bl.wordPotential = false;
    }
    else {
        bl.wordPotential = true;
    }
    //Sentences
    if ((ranges::contains(sentenceEnders, nextChar)) && bl.sentenceActive) {
        cntrs.sentenceCount++;
        bl.sentenceActive = false;
    }
    //Rows
    if (!bl.rowActive && isalnum(nextChar)) {
        bl.rowActive = true;
        cntrs.rowCount++;
    }
    if (nextChar == '\n') {
        bl.rowActive = false;
    }
}

void SheepCounter::printInfo() {
    println("znaku: {}", cntrs.charCount);
    println("slov: {}", cntrs.wordCount);
    println("vet: {}", cntrs.sentenceCount);
    println("radku: {}", cntrs.rowCount);
    println("cisel: {}", cntrs.numberCount);
    println("soucet: {}", numberSum);
}

int main(int argc, char ** argv)
{
    vector<string> args(argv, argv + argc);
    SheepCounter counter;
    if (argc > 1) { //if filemode
        for (int i = 1; i < argc; i++) {
            ifstream f;
            f.open(args[i]);
            if (f.good()) {
                counter.processStream(f);
            }
            else {
                println("error, problem with file");
                break;
            }
        }
    }
    else {
        counter.processStream(cin);
    }
    counter.printInfo();
}

