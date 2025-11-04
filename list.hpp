#ifndef SJTU_LIST_HPP
#define SJTU_LIST_HPP

#include "exceptions.hpp"
#include "algorithm.hpp"

#include <climits>
#include <cstddef>

namespace sjtu {
/**
 * a data container like std::list
 * allocate random memory addresses for data and they are doubly-linked in a list.
 */
template<typename T>
class list {
protected:
    class node {
    public:
        node *prev;
        node *next;
        T *val; // nullptr for sentinels, non-null for data nodes
        node() : prev(nullptr), next(nullptr), val(nullptr) {}
        node(const T &v) : prev(nullptr), next(nullptr), val(new T(v)) {}
        ~node() { if (val) { delete val; val = nullptr; } }
    };

protected:
    // doubly linked list with head/tail sentinels
    node *head;
    node *tail;
    size_t n;

    /**
     * insert node cur before node pos
     * return the inserted node cur
     */
    node *insert(node *pos, node *cur) {
        // link cur before pos
        cur->next = pos;
        cur->prev = pos->prev;
        pos->prev->next = cur;
        pos->prev = cur;
        return cur;
    }
    /**
     * remove node pos from list (no need to delete the node)
     * return the removed node pos
     */
    node *erase(node *pos) {
        node *p = pos;
        p->prev->next = p->next;
        p->next->prev = p->prev;
        p->prev = p->next = nullptr;
        return p;
    }

public:
    class const_iterator;
    class iterator {
    private:
        node *p;
        const list *owner;
    public:
        iterator() : p(nullptr), owner(nullptr) {}
        iterator(node *np, const list *o) : p(np), owner(o) {}
        /**
         * iter++
         */
        iterator operator++(int) {
            if (owner == nullptr || p == nullptr) throw invalid_iterator();
            if (p == owner->tail) throw invalid_iterator();
            iterator tmp = *this;
            p = p->next;
            return tmp;
        }
        /**
         * ++iter
         */
        iterator & operator++() {
            if (owner == nullptr || p == nullptr) throw invalid_iterator();
            if (p == owner->tail) throw invalid_iterator();
            p = p->next;
            return *this;
        }
        /**
         * iter--
         */
        iterator operator--(int) {
            if (owner == nullptr || p == nullptr) throw invalid_iterator();
            if (p == owner->head) throw invalid_iterator();
            iterator tmp = *this;
            if (p == owner->tail) {
                if (owner->n == 0) throw invalid_iterator();
                p = owner->tail->prev;
            } else {
                p = p->prev;
            }
            return tmp;
        }
        /**
         * --iter
         */
        iterator & operator--() {
            if (owner == nullptr || p == nullptr) throw invalid_iterator();
            if (p == owner->head) throw invalid_iterator();
            if (p == owner->tail) {
                if (owner->n == 0) throw invalid_iterator();
                p = owner->tail->prev;
            } else {
                p = p->prev;
            }
            return *this;
        }
        /**
         * TODO *it
         * remember to throw if iterator is invalid
         */
        T & operator *() const {
            if (owner == nullptr || p == nullptr || p == owner->head || p == owner->tail || p->val == nullptr)
                throw invalid_iterator();
            return *(p->val);
        }
        /**
         * TODO it->field
         * remember to throw if iterator is invalid
         */
        T * operator ->() const {
            if (owner == nullptr || p == nullptr || p == owner->head || p == owner->tail || p->val == nullptr)
                throw invalid_iterator();
            return p->val;
        }
        /**
         * a operator to check whether two iterators are same (pointing to the same memory).
         */
        bool operator==(const iterator &rhs) const { return p == rhs.p && owner == rhs.owner; }
        bool operator==(const const_iterator &rhs) const { return p == rhs.p && owner == rhs.owner; }
        /**
         * some other operator for iterator.
         */
        bool operator!=(const iterator &rhs) const { return !(*this == rhs); }
        bool operator!=(const const_iterator &rhs) const { return !(*this == rhs); }
        friend class list<T>;
    };
    /**
     * TODO
     * has same function as iterator, just for a const object.
     * should be able to construct from an iterator.
     */
    class const_iterator {
    private:
        const node *p;
        const list *owner;
    public:
        const_iterator() : p(nullptr), owner(nullptr) {}
        const_iterator(const node *np, const list *o) : p(np), owner(o) {}
        const_iterator(const iterator &it) : p(it.p), owner(it.owner) {}
        const_iterator operator++(int) {
            if (owner == nullptr || p == nullptr) throw invalid_iterator();
            if (p == owner->tail) throw invalid_iterator();
            const_iterator tmp = *this;
            p = p->next;
            return tmp;
        }
        const_iterator & operator++() {
            if (owner == nullptr || p == nullptr) throw invalid_iterator();
            if (p == owner->tail) throw invalid_iterator();
            p = p->next;
            return *this;
        }
        const_iterator operator--(int) {
            if (owner == nullptr || p == nullptr) throw invalid_iterator();
            if (p == owner->head) throw invalid_iterator();
            const_iterator tmp = *this;
            if (p == owner->tail) {
                if (owner->n == 0) throw invalid_iterator();
                p = owner->tail->prev;
            } else {
                p = p->prev;
            }
            return tmp;
        }
        const_iterator & operator--() {
            if (owner == nullptr || p == nullptr) throw invalid_iterator();
            if (p == owner->head) throw invalid_iterator();
            if (p == owner->tail) {
                if (owner->n == 0) throw invalid_iterator();
                p = owner->tail->prev;
            } else {
                p = p->prev;
            }
            return *this;
        }
        const T & operator *() const {
            if (owner == nullptr || p == nullptr || p == owner->head || p == owner->tail || p->val == nullptr)
                throw invalid_iterator();
            return *(p->val);
        }
        const T * operator ->() const {
            if (owner == nullptr || p == nullptr || p == owner->head || p == owner->tail || p->val == nullptr)
                throw invalid_iterator();
            return p->val;
        }
        bool operator==(const const_iterator &rhs) const { return p == rhs.p && owner == rhs.owner; }
        bool operator==(const iterator &rhs) const { return rhs == *this; }
        bool operator!=(const const_iterator &rhs) const { return !(*this == rhs); }
        bool operator!=(const iterator &rhs) const { return !(rhs == *this); }
        friend class list<T>;
    };

    // definitions moved inside class to avoid out-of-class template member placement issues

    /**
     * TODO Constructs
     * Atleast two: default constructor, copy constructor
     */
    list() : head(new node()), tail(new node()), n(0) {
        head->next = tail; tail->prev = head;
    }
    list(const list &other) : list() {
        for (const_iterator it = other.cbegin(); it != other.cend(); ++it) {
            push_back(*it);
        }
    }
    /**
     * TODO Destructor
     */
    virtual ~list() {
        clear();
        delete head; head = nullptr;
        delete tail; tail = nullptr;
    }
    /**
     * TODO Assignment operator
     */
    list &operator=(const list &other) {
        if (this == &other) return *this;
        clear();
        for (const_iterator it = other.cbegin(); it != other.cend(); ++it) {
            push_back(*it);
        }
        return *this;
    }
    /**
     * access the first / last element
     * throw container_is_empty when the container is empty.
     */
    const T & front() const {
        if (n == 0) throw container_is_empty();
        return *(head->next->val);
    }
    const T & back() const {
        if (n == 0) throw container_is_empty();
        return *(tail->prev->val);
    }
    /**
     * returns an iterator to the beginning.
     */
    iterator begin() { return iterator(head->next, this); }
    const_iterator cbegin() const { return const_iterator(head->next, this); }
    /**
     * returns an iterator to the end.
     */
    iterator end() { return iterator(tail, this); }
    const_iterator cend() const { return const_iterator(tail, this); }
    /**
     * checks whether the container is empty.
     */
    virtual bool empty() const { return n == 0; }
    /**
     * returns the number of elements
     */
    virtual size_t size() const { return n; }

    /**
     * clears the contents
     */
    virtual void clear() {
        node *cur = head->next;
        while (cur != tail) {
            node *next = cur->next;
            // unlink then delete
            cur->prev = cur->next = nullptr;
            delete cur;
            cur = next;
        }
        head->next = tail; tail->prev = head; n = 0;
    }
    /**
     * insert value before pos (pos may be the end() iterator)
     * return an iterator pointing to the inserted value
     * throw if the iterator is invalid
     */
    virtual iterator insert(iterator pos, const T &value) {
        if (pos.owner != this || pos.p == nullptr) throw invalid_iterator();
        node *cur = new node(value);
        insert(pos.p, cur);
        ++n;
        return iterator(cur, this);
    }
    /**
     * remove the element at pos (the end() iterator is invalid)
     * returns an iterator pointing to the following element, if pos pointing to the last element, end() will be returned.
     * throw if the container is empty, the iterator is invalid
     */
    virtual iterator erase(iterator pos) {
        if (n == 0) throw container_is_empty();
        if (pos.owner != this || pos.p == nullptr || pos.p == tail) throw invalid_iterator();
        node *next = pos.p->next;
        erase(pos.p);
        delete pos.p;
        --n;
        return iterator(next, this);
    }
    /**
     * adds an element to the end
     */
    void push_back(const T &value) {
        node *cur = new node(value);
        insert(tail, cur);
        ++n;
    }
    /**
     * removes the last element
     * throw when the container is empty.
     */
    void pop_back() {
        if (n == 0) throw container_is_empty();
        node *last = tail->prev;
        erase(last);
        delete last;
        --n;
    }
    /**
     * inserts an element to the beginning.
     */
    void push_front(const T &value) {
        node *cur = new node(value);
        insert(head->next, cur);
        ++n;
    }
    /**
     * removes the first element.
     * throw when the container is empty.
     */
    void pop_front() {
        if (n == 0) throw container_is_empty();
        node *first = head->next;
        erase(first);
        delete first;
        --n;
    }
    /**
     * sort the values in ascending order with operator< of T
     */
    void sort() {
        if (n <= 1) return;
        // collect data nodes in an array of node* and sort pointers by value
        node **arr = new node*[n];
        size_t i = 0;
        for (node *cur = head->next; cur != tail; cur = cur->next) arr[i++] = cur;
        sjtu::sort<node*>(arr, arr + n, [](const node *a, const node *b){ return *(a->val) < *(b->val); });
        // relink according to arr
        head->next = arr[0]; arr[0]->prev = head;
        for (size_t k = 0; k + 1 < n; ++k) {
            arr[k]->next = arr[k+1];
            arr[k+1]->prev = arr[k];
        }
        arr[n-1]->next = tail; tail->prev = arr[n-1];
        delete [] arr;
    }
    /**
     * merge two sorted lists into one (both in ascending order)
     * compare with operator< of T
     * container other becomes empty after the operation
     * for equivalent elements in the two lists, the elements from *this shall always precede the elements from other
     * the order of equivalent elements of *this and other does not change.
     * no elements are copied or moved
     */
    void merge(list &other) {
        if (&other == this) return; // nothing to do
        node *ai = head->next;
        node *bi = other.head->next;
        while (ai != tail && bi != other.tail) {
            if (*(bi->val) < *(ai->val)) {
                node *nextb = bi->next;
                other.erase(bi);
                insert(ai, bi);
                ++n; --other.n;
                bi = nextb;
            } else {
                ai = ai->next;
            }
        }
        // append remaining of other
        while (bi != other.tail) {
            node *nextb = bi->next;
            other.erase(bi);
            insert(tail, bi);
            ++n; --other.n;
            bi = nextb;
        }
        // other becomes empty
        other.head->next = other.tail; other.tail->prev = other.head;
    }
    /**
     * reverse the order of the elements
     * no elements are copied or moved
     */
    void reverse() {
        // swap next/prev for all nodes including sentinels
        node *cur = head;
        while (cur) {
            node *tmp = cur->next;
            cur->next = cur->prev;
            cur->prev = tmp;
            // move to original next, which is now prev
            cur = cur->prev;
        }
        // swap head and tail pointers
        node *tmp = head;
        head = tail;
        tail = tmp;
    }
    /**
     * remove all consecutive duplicate elements from the container
     * only the first element in each group of equal elements is left
     * use operator== of T to compare the elements.
     */
    void unique() {
        if (n <= 1) return;
        node *cur = head->next;
        while (cur != tail) {
            node *nx = cur->next;
            while (nx != tail && (*(cur->val) == *(nx->val))) {
                node *dup = nx;
                nx = nx->next;
                erase(dup);
                delete dup;
                --n;
            }
            cur = nx;
        }
    }
};

}

#endif //SJTU_LIST_HPP
