#include <iostream>
#include <string>
#include <cctype>
#include <print>

using namespace std;

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
    bool wordActive = false;
    bool sentenceActive = false;
    bool numberActive = false;
    int activeNmbValue = 0;
};

void SheepCounter::processStream(istream& stream) {
    char c;
    for (;;) {
        c = stream.get();
        if (stream.fail()) {
            return;
        }
        processChar(c);
    }
}

void SheepCounter::processChar(char nextChar) {
    cntrs.charCount++; //Chars - MEL BY SE POCITAT I KONEC RADKY MEZI ZNAKY?
    //Numbers
    if (isdigit(nextChar)) {
        numberActive = true;
        activeNmbValue = (activeNmbValue * 10) + nextChar - '0'; //Horners method
    }
    else {
        if (numberActive) {
            numberActive = false;
            cntrs.numberCount++;
            numberSum = numberSum + activeNmbValue;
            activeNmbValue = 0;
        }
    }
    //Words
    if (isalpha(nextChar)) {
        wordActive = true;
    }
    else {
        if (wordActive) {
            wordActive = false;
            cntrs.wordCount++;
        }
    }
    //Sentences
    if (isupper(nextChar)) {
        sentenceActive = true;
    }
    else {
        if (nextChar == '.' && sentenceActive) {
            cntrs.sentenceCount++;
            sentenceActive = false;
        }
    }
    //Rows
    if (nextChar == '\n') {
        cntrs.rowCount++;
        if (wordActive) {
            cntrs.wordCount++;
        }
    }
}

void SheepCounter::printInfo() {
    println("In the stream, there were:");
    println("   {} characters", cntrs.charCount);
    println("   {} rows", cntrs.rowCount);
    println("   {} words", cntrs.wordCount);
    println("   {} numbers", cntrs.numberCount);
    println("   {} sentences", cntrs.sentenceCount);
    println("The sum of all the numbers was {}", numberSum);
}

int main()
{
    SheepCounter counter;
    counter.processStream(cin);
    counter.printInfo();
}

