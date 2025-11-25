#include <iostream>
#include <string>
#include <vector>

using namespace std;

class AbstractVal {
public:
    virtual void print() = 0;
    virtual unique_ptr<AbstractVal> clone() = 0;
};

using ValPtr = unique_ptr<AbstractVal>;

class IntVal : public AbstractVal {
public:
    IntVal(int x) : x_(move (x)) {}
    virtual void print() { cout << x_; }
    virtual ValPtr clone() { return make_unique<IntVal>(*this); }
private:
    int x_;
};

class StringVal : public AbstractVal {
public:
    StringVal(string x) : x_(move(x)) {}
    virtual void print() { cout << x_; }
    virtual ValPtr clone() { return make_unique<StringVal>(*this); }
private:
    string x_;
};

class Seznam {
public:
    Seznam() {}
    void add(ValPtr p) { pole.push_back(move(p)); }
    void print() { for (auto&& x : pole) { x->print(); } }
    Seznam(const Seznam& s) { clone(s); }
    Seznam& operator=(const Seznam& s) {
        if (this == &s) { return *this; }
        pole.clear();
        clone(s);
        return (*this);
    }
private:
    vector<ValPtr> pole;
    void clone(const Seznam& s) {
        for (auto&& x : s.pole) {
            pole.push_back(x->clone());
        }
    }
};

int main()
{
    Seznam s1;
    Seznam s2;
    s1.add(make_unique<IntVal>(123));
    s1.add(make_unique<StringVal>("456"));
    s2 = s1;
    s2 = s2;
    s2.print();
}

