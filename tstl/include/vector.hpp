//=======================================================================
// Copyright Baptiste Wicht 2013-2016.
// Distributed under the terms of the MIT License.
// (See accompanying file LICENSE or copy at
//  http://www.opensource.org/licenses/MIT)
//=======================================================================

#ifndef VECTOR_H
#define VECTOR_H

#include <initializer_list.hpp>
#include <types.hpp>
#include <algorithms.hpp>
#include <new.hpp>
#include <iterator.hpp>

namespace std {

/*!
 * \brief A contiguous container of elements, automatically increasing.
 */
template<typename T>
struct vector {
    using value_type           = T;                 ///< The value type contained in the vector
    using pointer_type         = value_type*;       ///< The pointer type contained in the vector
    using reference_type       = value_type&;       ///< The pointer type contained in the vector
    using const_reference_type = const value_type&; ///< The pointer type contained in the vector
    using size_type            = size_t;            ///< The size type
    using iterator             = value_type*;       ///< The iterator type
    using const_iterator       = const value_type*; ///< The const iterator type

    using reverse_iterator       = std::reverse_iterator<iterator>;       ///< The reverse iterator type
    using const_reverse_iterator = std::reverse_iterator<const_iterator>; ///< The const reverse iterator type

    /*!
     * \brief Constructs en empty vector
     */
    vector() : data(nullptr), _size(0), _capacity(0) {}

    /*!
     * \brief Constructs a vector of the given size
     */
    explicit vector(uint64_t c) : data(allocate(c)), _size(0), _capacity(c) {}

    /*!
     * \brief Construct a vector containing the given values
     */
    vector(initializer_list<T> values) : data(allocate(values.size())), _size(values.size()), _capacity(values.size()) {
        size_t i = 0;
        for(auto& v : values){
            new (&data[i++]) value_type(v);
        }
    }

    vector(const vector& rhs) : data(nullptr), _size(rhs._size), _capacity(rhs._capacity) {
        if(!rhs.empty()){
            data = allocate(_capacity);

            for(size_t i = 0; i < _size; ++i){
                new (&data[i]) value_type(rhs.data[i]);
            }
        }
    }

    vector& operator=(const vector& rhs){
        if (this != &rhs) {
            if (_capacity < rhs._capacity) {
                if (data) {
                    release();
                }

                _capacity = rhs._capacity;
                data      = allocate(_capacity);
            } else {
                destruct_all();
            }

            _size = rhs._size;

            for (size_t i = 0; i < _size; ++i) {
                new (&data[i]) value_type(rhs.data[i]);
            }
        }

        return *this;
    }

    //Move constructors

    vector(vector&& rhs) : data(rhs.data), _size(rhs._size), _capacity(rhs._capacity) {
        rhs.data = nullptr;
        rhs._size = 0;
        rhs._capacity = 0;
    };

    vector& operator=(vector&& rhs){
        if (this != &rhs) {
            if (data) {
                release();
            }

            data          = rhs.data;
            _size         = rhs._size;
            _capacity     = rhs._capacity;

            rhs.data      = nullptr;
            rhs._size     = 0;
            rhs._capacity = 0;
        }

        return *this;
    }

    ~vector(){
        if(data){
            release();
        }
    }

    //Getters

    /*!
     * \brief Returns the size of the vector
     */
    constexpr size_type size() const {
        return _size;
    }

    /*!
     * \brief Indicates if the vector is empty
     */
    bool empty() const {
        return _size == 0;
    }

    /*!
     * \brief Returns the capacity of the vector
     */
    constexpr size_type capacity() const {
        return _capacity;
    }

    /*!
     * \brief Returns a const reference to the elemenet at the given position
     */
    constexpr const value_type& operator[](size_type pos) const {
        return data[pos];
    }

    /*!
     * \brief Returns a reference to the elemenet at the given position
     */
    value_type& operator[](size_type pos){
        return data[pos];
    }

    /*!
     * \brief Returns a reference to the element at the front of the collection
     */
    value_type& front(){
        return data[0];
    }

    /*!
     * \brief Returns a const reference to the element at the front of the collection
     */
    const value_type& front() const  {
        return data[0];
    }

    /*!
     * \brief Returns a reference to the element at the back of the collection
     */
    value_type& back(){
        return data[size() - 1];
    }

    /*!
     * \brief Returns a const reference to the element at the back of the collection
     */
    const value_type& back() const  {
        return data[size() - 1];
    }

    //Modifiers

    /*!
     * \brief Augments the capacity to at least the given capacity
     */
    void reserve(size_t new_capacity){
        if(new_capacity > capacity()){
            ensure_capacity(new_capacity);
        }
    }

    /*!
     * \brief Resize the vector to the given size
     */
    void resize(size_t new_size){
        if(new_size > size()){
            ensure_capacity(new_size);

            // Default initialize the new elements
            for(size_t i = _size; i < new_size; ++i){
                new (&data[i]) value_type();
            }

            _size = new_size;
        } else if(new_size < _size){
            // Call the necessary destructors
            for(size_t i = new_size; i < _size; ++i){
                data[i].~value_type();
            }

            //By diminishing the size, the last elements become unreachable
            _size = new_size;
        }
    }

    /*!
     * \brief Add an element at the back of the vector
     */
    void push_back(value_type&& element){
        ensure_capacity(_size + 1);

        new (&data[_size++]) value_type(std::move(element));
    }

    /*!
     * \brief Add an element at the back of the vector
     */
    void push_back(const value_type& element){
        ensure_capacity(_size + 1);

        new (&data[_size++]) value_type(element);
    }

    value_type& emplace_back(){
        ensure_capacity(_size + 1);

        new (&data[_size++]) value_type();

        return back();
    }

    template<typename... Args>
    value_type& emplace_back(Args... args){
        ensure_capacity(_size + 1);

        new (&data[_size++]) value_type{std::forward<Args>(args)...};

        return back();
    }

    /*!
     * \brief Add an element at the front of the vector
     */
    void push_front(value_type&& element){
        ensure_capacity(_size + 1);

        if(!empty()){
            new (&data[_size]) value_type(std::move(data[_size - 1]));

            for (size_t i = _size - 1; i > 0; --i) {
                data[i] = std::move(data[i - 1]);
            }
        }

        // At this point data[0] has been deleted
        data[0] = std::move(element);

        ++_size;
    }

    /*!
     * \brief Add an element at the front of the vector
     */
    void push_front(const value_type& element){
        ensure_capacity(_size + 1);

        if(!empty()){
            new (&data[_size]) value_type(std::move(data[_size - 1]));

            for (size_t i = _size - 1; i > 0; --i) {
                data[i] = std::move(data[i - 1]);
            }
        }

        // At this point data[0] has been deleted
        data[0] = element;

        ++_size;
    }

    /*!
     * \brief Removes the last element of the vector
     */
    void pop_back(){
        --_size;

        // Call the destructor of the erased value
        data[_size].~value_type();
    }

    /*!
     * \brief Removes all the elements of the vector
     */
    void clear(){
        destruct_all();

        _size = 0;
    }

    void erase(size_t position){
        for(size_t i = position; i < _size - 1; ++i){
            data[i] = std::move(data[i+1]);
        }

        --_size;

        // Call the destructor of the last value
        data[_size].~value_type();
    }

    void erase(iterator position){
        for(size_t i = position - begin(); i < _size - 1; ++i){
            data[i] = std::move(data[i+1]);
        }

        --_size;

        // Call the destructor of the last value
        data[_size].~value_type();
    }

    void erase(iterator first, iterator last){
        auto n = std::distance(first, last);

        for(size_t i = first - begin(); i < _size - n; ++i){
            data[i] = std::move(data[i+n]);
        }

        // Call the destructors on the erase elements
        for(size_t i = _size - n; i < _size; ++i){
            data[i].~value_type();
        }

        _size -= n;
    }

    //Iterators

    iterator begin(){
        return iterator(&data[0]);
    }

    constexpr const_iterator begin() const {
        return const_iterator(&data[0]);
    }

    iterator end(){
        return iterator(&data[_size]);
    }

    constexpr const_iterator end() const {
        return const_iterator(&data[_size]);
    }

    //Iterators

    reverse_iterator rbegin(){
        return reverse_iterator(&data[_size] - 1);
    }

    constexpr const_reverse_iterator rbegin() const {
        return const_iterator(&data[_size - 1]);
    }

    reverse_iterator rend(){
        return reverse_iterator(&data[-1]);
    }

    constexpr const_reverse_iterator rend() const {
        return const_reverse_iterator(&data[-1]);
    }

    // Relational operators

    bool operator==(const vector& rhs) const {
        if(size() != rhs.size()){
            return false;
        }

        for(size_t i = 0; i < size(); ++i){
            if((*this)[i] != rhs[i]){
                return false;
            }
        }

        return true;
    }

    bool operator!=(const vector& rhs) const {
        return !(*this == rhs);
    }

private:
    static value_type* allocate(size_t n){
        return reinterpret_cast<value_type*>(new uint8_t[n * sizeof(value_type)]);
    }

    static void deallocate(value_type* ptr){
        delete[] reinterpret_cast<uint8_t*>(ptr);
    }

    void destruct_all(){
        // Call the destructors
        for(size_t i = 0; i< _size; ++i){
            data[i].~value_type();
        }
    }

    void release(){
        destruct_all();

        // Deallocate the memory
        deallocate(data);
        data = nullptr;
    }

    void ensure_capacity(size_t new_capacity){
        if(_capacity == 0){
            _capacity = new_capacity;
            data = allocate(_capacity);
        } else if(_capacity < new_capacity){
            // Double the current capacity
            _capacity= _capacity * 2;

            // If not enough, use the given new_capacity
            if(new_capacity > _capacity){
                _capacity = new_capacity;
            }

            auto new_data = allocate(_capacity);

            // Move the old data into the new one
            for(size_t i = 0; i < _size; ++i){
                new (&new_data[i]) value_type(std::move(data[i]));
            }

            release();

            data = new_data;
        }
    }

    T* data; ///< The data storage
    uint64_t _size; ///< The vector size
    uint64_t _capacity; ///< The data capacity

};

} //end of namespace std

#endif
