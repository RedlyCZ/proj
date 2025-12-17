#include <vector>
#include <memory>
#include <stdexcept>
#include <string>


#ifndef stableVec
#define stableVec

class stable_vector_exception : public std::out_of_range {
public:
    stable_vector_exception(size_t ix) : std::out_of_range(""), ix_(ix), s_("stable_vector: index out of range") {}
    virtual const char* what() const noexcept override { return s_.c_str(); }
    size_t get_index() const { return ix_; }
private:
    size_t ix_;
    std::string s_;
};


template<typename T>
class stable_vector {
public:
    stable_vector(size_t chunkSize = 100) : chunkSize{ chunkSize } {}
    //rule of FIVE
    ~stable_vector() = default; //destructor
    stable_vector(stable_vector&&) = default; //move constructor
    stable_vector& operator=(stable_vector&&) = default; //move assignment
    stable_vector(const stable_vector& other) : chunkSize(other.chunkSize), elementCount(other.elementCount) { //copy constructor
        rake.reserve(other.rake.size());
        for (const auto& chunk : other.rake) {
            // Create a new chunk
            rake.push_back(std::make_unique<T[]>(chunkSize));
            // Copy data manually
            for (size_t i = 0; i < chunkSize; ++i) {
                rake.back()[i] = chunk[i];
            }
        }
    }
    stable_vector& operator=(const stable_vector& other) { //copy assignment
        if (this != &other) {
            rake.clear();
            chunkSize = other.chunkSize;
            elementCount = other.elementCount;

            rake.reserve(other.rake.size());
            for (const auto& chunk : other.rake) {
                rake.push_back(std::make_unique<T[]>(chunkSize));
                for (size_t i = 0; i < chunkSize; ++i) {
                    rake.back()[i] = chunk[i];
                }
            }
        }
        return *this;
    }


    void push_back(T element); //would use const T& element, but this is specifically mentioned in assignment
    T& operator[](size_t index) {
        return rake[index / chunkSize][index % chunkSize];
    }
    const T& operator[](size_t index) const {
        return rake[index / chunkSize][index % chunkSize];
    }
    T& at(size_t index) {
        if (index >= elementCount) {
            throw stable_vector_exception{ index };
        }
        else {
            return (*this)[index];
        }
    }

    class iterator {
    public:
        iterator() : iterIndex{ 0 }, contPtr{ nullptr } {}
        iterator(stable_vector* pole, size_t index) : iterIndex(index), contPtr(pole) {}
        T& operator*() {
            return (*contPtr)[iterIndex];
        }
        bool operator!=(const iterator& it) const {
            return(this->contPtr != it.contPtr || this->iterIndex != it.iterIndex);
        }
        iterator& operator++() {
            ++iterIndex;
            return *this;
        }
    private:
        size_t iterIndex = 0;
        stable_vector* contPtr;
    };

    iterator begin() {
        return iterator(this, 0);
    }

    iterator end() {
        return iterator(this, elementCount);
    }

    class const_iterator {
    public:
        const_iterator() : iterIndex{ 0 }, contPtr{ nullptr } {}
        const_iterator(const stable_vector* pole, size_t index) : iterIndex(index), contPtr(pole) {}
        const T& operator*() const {
            return (*contPtr)[iterIndex];
        }
        bool operator!=(const const_iterator& it) const {
            return(this->contPtr != it.contPtr || this->iterIndex != it.iterIndex);
        }
        const_iterator& operator++() {
            ++iterIndex;
            return *this;
        }
    private:
        size_t iterIndex = 0;
        const stable_vector* contPtr;
    };

    const_iterator cbegin() const {
        return const_iterator(this, 0);
    }

    const_iterator cend() const {
        return const_iterator(this, elementCount);
    }
    const_iterator begin() const { return cbegin(); }
    const_iterator end() const { return cend(); }

private:
    size_t chunkSize;
    size_t elementCount = 0;
    std::vector<std::unique_ptr<T[]>> rake;
};


template<typename T>
void stable_vector<T>::push_back(T element) {
    if (!(elementCount % chunkSize)) { //adding the element requires adding new container
        size_t prevSize = rake.size();
        rake.push_back(std::make_unique<T[]>(chunkSize));
        rake[prevSize][0] = std::move(element);
    }
    else { //adding the element doesnt require adding new container
        rake[elementCount / chunkSize][elementCount % chunkSize] = std::move(element);
    }
    ++elementCount;
}


#endif