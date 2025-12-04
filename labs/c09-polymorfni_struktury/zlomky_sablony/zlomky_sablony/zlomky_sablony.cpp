#include <print>

template<typename T> class zlomek
{
public:
    zlomek(T citatel, T jmenovatel) {
        this->citatel = citatel;
        this->jmenovatel = jmenovatel;
    }
    template<typename X> friend zlomek<X> operator+(const zlomek<X>& a, const zlomek<X>& b);

    void vypisse() {
        println("{}", this->citatel);
        println("{}", this->jmenovatel);
    }
private:
    T citatel;
    T jmenovatel;
};

template<typename X> 
zlomek<X> operator+(const zlomek<X>& a, const zlomek<X>& b) {
    return zlomek<X> {a.citatel* b.jmenovatel + b.citatel * a.jmenovatel, a.jmenovatel* b.jmenovatel};
}

int main()
{
    zlomek<int> prvni = zlomek<int>(1, 2);
    zlomek<int> druhy = zlomek<int>(2, 3);
    zlomek<int> treti = prvni + druhy;
    treti.vypisse();

}

