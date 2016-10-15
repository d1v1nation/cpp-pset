//
// Created by cat on 13.10.16.
//

#ifndef PSET_PERSISTENT_SET_H
#define PSET_PERSISTENT_SET_H


#include <memory>
#include <list>
#include <assert.h>

struct persistent_set {

    // Вы можете определить этот тайпдеф по вашему усмотрению.
    typedef int value_type;

    // Bidirectional iterator.
    struct iterator;
    struct node;
    typedef std::shared_ptr<node> nodeptr;


    // Создает пустой persistent_set.
    persistent_set();

    // Создает копию указанного persistent_set-а.
    persistent_set(persistent_set const &);

    // Изменяет this так, чтобы он содержал те же элементы, что и rhs.
    // Инвалидирует все итераторы, принадлежащие persistent_set'у this, включая end().
    persistent_set &operator=(persistent_set const &rhs);

    // Деструктор. Вызывается при удалении объектов persistent_set.
    // Инвалидирует все итераторы ссылающиеся на элементы этого persistent_set
    // (включая итераторы ссылающиеся на элементы следующие за последними).
    ~persistent_set();

    // Поиск элемента.
    // Возвращает итератор на элемент найденный элемент, либо end().
    iterator find(value_type);

    // Вставка элемента.
    // 1. Если такой ключ уже присутствует, вставка не производиться, возвращается итератор
    //    на уже присутствующий элемент и false.
    // 2. Если такого ключа ещё нет, производиться вставка, возвращается итератор на созданный
    //    элемент и true.
    // Если вставка произведена, инвалидирует все итераторы, принадлежащие persistent_set'у this, включая end().
    std::pair<iterator, bool> insert(value_type);

    // Удаление элемента.
    // Инвалидирует все итераторы, принадлежащие persistent_set'у this, включая end().
    void erase(iterator);

    // Возващает итератор на элемент с минимальный ключом.
    iterator begin() const;

    // Возващает итератор на элемент следующий за элементом с максимальным ключом.
    iterator end() const;


private:
    nodeptr root;
    std::list<persistent_set::nodeptr> locate(persistent_set::value_type orient) const;
    nodeptr rebuild(std::list<persistent_set::nodeptr> const& desc, nodeptr start, bool detest = false, value_type which = -1) const;


    static bool lt(value_type val, nodeptr nd);
    static bool ne(value_type val, nodeptr nd);

};

class persistent_set::node : public std::enable_shared_from_this<node> // al private therefore class
{

    bool horizon; // EVENT HORIZON aka the end mark aka the root mark (root considered greatest)
    nodeptr left = nullptr, right = nullptr;
    persistent_set::value_type value;

    node();
    node(value_type value);


    nodeptr get_shared() {
        return shared_from_this();
    }

    friend class persistent_set;
    friend class persistent_set;
    friend class persistent_set::iterator;
    
};

struct persistent_set::iterator
{
    // Элемент на который сейчас ссылается итератор.
    // Разыменование итератора end() неопределено.
    // Разыменование невалидного итератора неопределено.
    value_type const& operator*() const;

    // Переход к элементу со следующим по величине ключом.
    // Инкремент итератора end() неопределен.
    // Инкремент невалидного итератора неопределен.
    iterator& operator++();
    iterator operator++(int);

    // Переход к элементу со следующим по величине ключом.
    // Декремент итератора begin() неопределен.
    // Декремент невалидного итератора неопределен.
    iterator& operator--();
    iterator operator--(int);
    ~iterator();


private:
    std::list<persistent_set::nodeptr> asc; // not worse than O(h) * size_t
    const persistent_set& p;

    iterator() = delete;
    iterator(std::list<persistent_set::nodeptr> found, const persistent_set& p);

    friend class persistent_set;
    friend class persistent_set::node;
};


// Сравнение. Итераторы считаются эквивалентными если одни ссылаются на один и тот же элемент.
// Сравнение с невалидным итератором не определено.
// Сравнение итераторов двух разных контейнеров не определено.
bool operator==(persistent_set::iterator, persistent_set::iterator);
bool operator!=(persistent_set::iterator, persistent_set::iterator);


#endif //PSET_PERSISTENT_SET_H
