#include <print>
#include <iostream>
#include <vector>
#include <memory>

using namespace std;

template<typename T> 
class GumPole {
public:
    GumPole(int chunkSize = 100) : chunkSize{ chunkSize }{}
    void push_back(const T& element);
    T& operator[](int index) {
        return rake[index / chunkSize][index % chunkSize];
    }


    class iterator {
    public:
        iterator() : iterIndex{ 0 }, contPtr{ nullptr } {}
        iterator(const iterator& it) {
            this->iterIndex = it.iterIndex;
            this->contPtr = it.contPtr;
        }
        iterator(GumPole* pole, int index) : contPtr(pole), iterIndex(index) {}
        T& operator*(){
            return (*contPtr)[iterIndex];
        }
        bool operator!=(const iterator& it) {
            return(this->contPtr != it.contPtr || this->iterIndex != it.iterIndex);
        }
        iterator& operator++() {
            ++iterIndex;
            return *this;
        }
    private:
        int iterIndex = 0;
        GumPole* contPtr;
    };

    iterator begin() {
        return iterator(this, 0);
    }

    iterator end() {
        return iterator(this, elementCount);
    }

private:
    int chunkSize;
    int elementCount = 0;
    vector<unique_ptr<T[]>> rake;
};

template<typename T>
void GumPole<T>::push_back(const T& element) {
    if (!(elementCount % chunkSize)) { //adding the element requires adding new container
        size_t prevSize = rake.size();
        rake.push_back(make_unique<T[]>(chunkSize));
        rake[prevSize][0] = element;
    }
    else { //adding the element doesnt require adding new container
        rake[elementCount / chunkSize][elementCount % chunkSize] = element;
    }
    ++elementCount;
}


int main()
{
    println("Bonan Tagon!");
}

