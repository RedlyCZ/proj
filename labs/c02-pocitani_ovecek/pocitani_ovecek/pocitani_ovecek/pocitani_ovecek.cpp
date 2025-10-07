#include <iostream>
#include <string>
#include <cctype>
#include <print>

using namespace std;

class SheepCounter {
public:
    int charCount;
    int rowCount;
    int wordCount;
    int sentenceCount;
    int numberCount;
    int numberSum;
    void processStream(istream& stream);
private:
    void processChar(char nextChar);
    bool wordActive;
    bool sentenceActive;
    bool numberActive;
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
    charCount++;
    if (nextChar == '\n') {
        rowCount++;
    }
}

int main()
{
    SheepCounter counter;
    counter.charCount = 0;
    counter.rowCount = 0;
    counter.processStream(cin);
    println("{}", counter.charCount);
    println("{}", counter.rowCount);
}

