#include <iostream>
#include <vector>
#include <algorithm>
#include <print>
#include <string>
#include <set>
#include <numeric>

using namespace std;

void exerciseOne() {
    int X = 10;
    int Y = 100;
    vector<int> vstup = { 1, 15, 8, 7, 16, 20, 70 };

    multiset<int> vystup;
    
    for_each(vstup.cbegin(), vstup.cend(), [&](int element) {
        if (element > X) {
            vystup.emplace(element + Y);
        }
    });

    for (auto&& prvek : vystup) {
        print("{} ", prvek);
    }
    println("");
}

void exerciseTwo() {
    int n = 5;

    vector<int> vstup = { 5, 6, 8, 15, 1, 3 };

    auto it = adjacent_find(vstup.cbegin(), vstup.cend(), [&](int a, int b) { return abs(a - b) >= n;});

    if (it != vstup.end()) {
        print("{}", *(it + 1));
    }
    else {
        print("neobsahuje takovou zmenu");
    }
}

class exerciseThreeFunctor {
public:
    int operator() (int& a) {
        if (a >= dolnimez && a <= hornimez) {
            int c = a + n;
            n = n + m;
            return c;
        }
        else {
            return a;
        }
    }
private:
    int n = 2;
    int m = n;
    int hornimez = 5;
    int dolnimez = 3;
};

void exerciseThree() {
    vector<int> vstup = { 1,2,4,5,6 };
    transform(vstup.begin(), vstup.end(), vstup.begin(), exerciseThreeFunctor());
    for (auto&& element : vstup) {
        print("{} ", element);
    }
}

void exerciseFour() {
    vector<int> vstup = { 1, 5, 20, 22, 30 };

    vector<int> rozdily;

    adjacent_difference(vstup.begin(), vstup.end(), back_inserter(rozdily),
        [](int a, int b) {
            return abs(a - b); // Chceme absolutní velikost díry
        }
    );

    auto it = max_element(rozdily.begin() + 1, rozdily.end());

    int index = distance(rozdily.begin(), it);

    print("{}", index);
}


class exerciseFiveFunctor {
public:
    int operator() (int& a, int& b) {
        int c = a * a + b * b;
        return c;
    }
};

void exerciseFive() {
    vector<int> vstup = { 1, 2, 4, 5, 6 };
    int posun = (vstup.size() / 2);
    if (vstup.size() % 2 == 1) {
        posun++;
    }

    transform(vstup.begin(), vstup.end() - posun, vstup.begin() + posun, vstup.begin(), exerciseFiveFunctor());

    vstup.resize(posun-1);

    for (auto it = vstup.begin(); it != vstup.end(); it++) {
        print("{} ", *it);
    }
    println("");
}

int main()
{
    //exerciseOne();
    //exerciseTwo();
    exerciseThree();
    //exerciseFour();
    //exerciseFive();
}

