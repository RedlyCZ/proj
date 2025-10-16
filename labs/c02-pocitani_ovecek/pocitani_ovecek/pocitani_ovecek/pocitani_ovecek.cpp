#include <iostream>
#include <string>
#include <cctype>
#include <print>
#include <algorithm>

using namespace std;

constexpr char sentenceEnders[] = { '.', '?', '!' };

struct counters {
    int charCount = 0;
    int rowCount = 0;
    int wordCount = 0;
    int sentenceCount = 0;
    int numberCount = 0;
};

class SheepCounter {
public:
    counters cntrs;
    int numberSum = 0;
    void processStream(istream& stream);
    void printInfo();
private:
    void processChar(char nextChar);
    bool sentenceActive = false;
    bool numberActive = false;
    bool rowActive = false;
    bool wordPotential = true;
    int activeNmbValue = 0;
};

void SheepCounter::processStream(istream& stream) {
    char c;
    for (;;) {
        c = stream.get();
        if (stream.fail()) {
            numberSum = numberSum + activeNmbValue; //Usefull, if stream ended with a number
            return;
        }
        processChar(c);
    }
}

void SheepCounter::processChar(char nextChar) {
    cntrs.charCount++; //Chars
    //Numbers
    if (isdigit(nextChar) && !numberActive && wordPotential) {
        numberActive = true;
        cntrs.numberCount++;
        activeNmbValue = nextChar - '0';
    }
    else {
        if (isdigit(nextChar)) {
            activeNmbValue = (activeNmbValue * 10) + nextChar - '0'; //Horners method
        }
        if (numberActive && !isdigit(nextChar)) {
            numberActive = false;
            numberSum = numberSum + activeNmbValue;
            activeNmbValue = 0;
        }
    }
    //Words (and part of sentences
    if (isalpha(nextChar) && wordPotential) {
        cntrs.wordCount++;
        sentenceActive = true;
    }
    if (isalnum(nextChar)) {
        wordPotential = false;
    }
    else {
        wordPotential = true;
    }
    //Sentences
    if ((ranges::contains(sentenceEnders, nextChar)) && sentenceActive) {
        cntrs.sentenceCount++;
        sentenceActive = false;
    }
    //Rows
    if (!rowActive && isalnum(nextChar)) {
        rowActive = true;
        cntrs.rowCount++;
    }
    if (nextChar == '\n') {
        rowActive = false;
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

int main()
{
    SheepCounter counter;
    counter.processStream(cin);
    counter.printInfo();
}

