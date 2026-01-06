#include <print>
#include <string>
#include <iostream>

using namespace std;

template<typename T> class zlomek
{
public:
    zlomek(T citatel, T jmenovatel) {
        this->citatel = citatel;
        this->jmenovatel = jmenovatel;
    }
    template<typename X> friend zlomek<X> operator+(const zlomek<X>& a, const zlomek<X>& b);

    friend std::ostream& operator<<(std::ostream& os, const zlomek<T>& z) {
        os << z.citatel << "/" << z.jmenovatel;
        return os;
    }

private:
    T citatel;
    T jmenovatel;
};

template<typename X> 
zlomek<X> operator+(const zlomek<X>& a, const zlomek<X>& b) {
    return zlomek<X> {a.citatel* b.jmenovatel + b.citatel * a.jmenovatel, a.jmenovatel* b.jmenovatel};
}

template<typename T> class DA {
public:
    DA(T v) : v_(v) {}

    template<typename X>
    friend DA<X> operator+(const DA<X>& x, const DA<X>& y);

    void print() {
        std::cout << v_ << std::endl;
    }

private:
    T v_;
};

template<typename X>
DA<X> operator+(const DA<X>& x, const DA<X>& y) {
    // The logic from the slide
    return DA<X> { x.v_ + x.v_ + y.v_ + y.v_ };
}

int main()
{
    DA<int> a = 2;
    DA<int> b = 3;
    DA<int> c = a + b;
    c.print();

    DA<double> d = 0.5687;
    DA<double> e = 0.4313;
    DA<double> f = d + e;
    f.print();

    DA<string> g = string("Hi");
    DA<string> h = string("Ye");
    DA<string> ch = g + h;
    ch.print();

    DA<zlomek<int>> i = zlomek<int>(1, 2);
    DA<zlomek<int>> j = zlomek<int>(2, 3);
    DA<zlomek<int>> k = i + j;
    k.print();


}

