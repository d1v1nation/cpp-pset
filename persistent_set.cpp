//
// Created by cat on 13.10.16.
//

#include <iostream>
#include "persistent_set.h"

persistent_set::node::node() {
    horizon = true;
    left = right = nullptr;
    value = 0;
}

persistent_set::node::node(persistent_set::value_type value) : value(value) {
    horizon = false;
    left = right = nullptr;
}

persistent_set::iterator::iterator(std::list<persistent_set::nodeptr> found, const persistent_set& p) : asc(found), p(p) {
}

persistent_set::iterator::~iterator() {
}

const persistent_set::value_type &persistent_set::iterator::operator*() const {
    return asc.front()->value;
}

persistent_set::iterator &persistent_set::iterator::operator++() {
    nodeptr cur = asc.front();

    if (cur->right != nullptr) { // RST exists, desc into it
        cur = cur->right;
        while (cur->left != nullptr) {
            asc.push_front(cur);
            cur = cur->left;
        }
        asc.push_front(cur);

        return *this;
    } else { // no RST, go up until parent is greater
        value_type val = cur->value;
        asc.pop_front();

        while (!lt(val, asc.front())) {
            asc.pop_front();
        }

        return *this;
    }
}

persistent_set::iterator persistent_set::iterator::operator++(int) {
    iterator cpy(*this);
    ++(*this);
    return cpy;
}

persistent_set::iterator &persistent_set::iterator::operator--() {
    nodeptr cur = asc.front();
    if (cur->left != nullptr) {
        cur = cur->left;
        while (cur->right != nullptr) {
            asc.push_front(cur);
            cur = cur->right;
        }
        asc.push_front(cur);


        return *this;
    } else {
        value_type val = cur->value;
        while (!lt(val, asc.front())) {
            asc.pop_front();
        }
        asc.pop_front();

        return *this;
    }
}

persistent_set::iterator persistent_set::iterator::operator--(int) {
    iterator cpy(*this);
    --(*this);
    return cpy;
}


persistent_set::persistent_set() {
    root = std::shared_ptr<node>(new persistent_set::node);
}

persistent_set::persistent_set(persistent_set const & other) {
    root = other.root->get_shared();
}

persistent_set &persistent_set::operator=(persistent_set const &rhs) {
    this->root = rhs.root;
    return *this;
}

persistent_set::~persistent_set() {
}

persistent_set::iterator persistent_set::find(persistent_set::value_type orient) {
    std::list<persistent_set::nodeptr> desc;
    nodeptr cur = root;

    while (cur != nullptr && ne(orient, cur)) {
        desc.push_front(cur);
        if (lt(orient, cur)) {
            cur = cur->left;
        } else {
            cur = cur->right;
        }
    }

    // found or nullptr achieved - save it
    desc.push_front(cur);
    if (desc.front() == nullptr) {
        desc.clear();
        desc.push_front(root); // AKA horizon
    }

    return persistent_set::iterator(desc, *this); // desc is bound to iterator and deallocated there
}

std::pair<persistent_set::iterator, bool> persistent_set::insert(persistent_set::value_type orient) {

    std::list<persistent_set::nodeptr> desc = this->locate(orient);

    if (desc.front() == nullptr) { // not found - initiate insertion!
        nodeptr newnode = nodeptr(new node(orient));

        desc.pop_front(); // remove nullptr

        root = rebuild(desc, newnode);


        // update root


        return {find(orient), true};

    }

    if (desc.front()->value == orient) { // already exists, produce iter and exit
        return {persistent_set::iterator(desc, *this), false}; // desc bound to iter
    }
}


std::list<persistent_set::nodeptr> persistent_set::locate(persistent_set::value_type orient) const {
    std::list<persistent_set::nodeptr> desc;
    nodeptr cur = this->root;

    while (cur != nullptr && ne(orient, cur)) {
        desc.push_front(cur);
        if (lt(orient, cur)) {
            cur = cur->left;
        } else {
            cur = cur->right;
        }
    }
    desc.push_front(cur);
    return desc;
}

persistent_set::iterator persistent_set::end() const {
    std::list<persistent_set::nodeptr> desc;
    desc.push_front(root);

    return persistent_set::iterator(desc, *this);
}

persistent_set::iterator persistent_set::begin() const {
    std::list<persistent_set::nodeptr> desc;
    nodeptr cur = root;
    while (cur != nullptr) {
        desc.push_front(cur);
        cur = cur->left;
    }
    return persistent_set::iterator(desc, *this);
}

void persistent_set::erase(persistent_set::iterator iter) {
    nodeptr cur = iter.asc.front();

    if (cur->left == nullptr && cur->right == nullptr) {
        cur = nullptr;
    } else if (cur->left == nullptr) {
        cur = nodeptr(new node(*cur->right));
    } else if (cur->right == nullptr) {
        cur = nodeptr(new node(*cur->left));
    } else {
        nodeptr old = cur;

        persistent_set subset;
        nodeptr new_horizon(new node());
        new_horizon->left = cur->right;
        subset.root = new_horizon;
        value_type repl = *(subset.begin());

        subset.erase(subset.begin());

        cur = nodeptr(new node(repl));
        cur->right = subset.root->left;
        cur->left = old->left;


    }

    nodeptr old = iter.asc.front();
    iter.asc.pop_front();

    root = rebuild(iter.asc, cur, true, old->value);
}

persistent_set::nodeptr
persistent_set::rebuild(std::list<persistent_set::nodeptr> const& desc, nodeptr start, bool detest, value_type which) const {
    for (std::list<persistent_set::nodeptr>::const_iterator i = desc.cbegin(); i != desc.cend(); ++i) {
        nodeptr oldptr(*i);
        nodeptr newptr(new node(*oldptr));

        if (start != nullptr) {
            if (lt(start->value, newptr)) {
                newptr->left = start;
            } else {
                newptr->right = start;
            }
        }

        if (detest) {
            if (newptr->left != nullptr && newptr->left->value == which)
                newptr->left = nullptr;
            if (newptr->right != nullptr && newptr->right->value == which)
                newptr->right = nullptr;
            detest = false;
        }

        start = newptr;
    }

    return start;
}

inline bool persistent_set::ne(persistent_set::value_type val, persistent_set::nodeptr nd) {
    return (nd->horizon) || (val != nd->value);
}


inline bool persistent_set::lt(persistent_set::value_type val, persistent_set::nodeptr nd) {
    return (nd->horizon) || (val <= nd->value);
}


bool operator==(persistent_set::iterator a, persistent_set::iterator b) {
    return (*a) == (*b);
}

bool operator!=(persistent_set::iterator a, persistent_set::iterator b) {
    return (*a) != (*b);
}
