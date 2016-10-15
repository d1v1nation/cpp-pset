#include <iostream>
#include <stdlib.h>
#include "persistent_set.h"

using namespace std;


bool pset_populate_o(persistent_set &p, int h = 256, int l = 0) {
    if (h - l < 2)
        return true;


    int i = (h + l) / 2;
    p.insert(i);
    pset_populate_o(p, i, l);
    pset_populate_o(p, h, i);
    return true;
}

bool pset_populate_r(persistent_set &p, int i = 8192) {
    int cand = -1;

    while(--i >= 0) {
        if (i % 2048 == 0) {
            cand = std::rand();
            p.insert(cand);
            continue;
        }
        p.insert(std::rand());
    }

    bool pred = true;
    if (cand != -1)
    pred = *(p.find(cand)) == cand;
//    cout << (pred ? "One-in-many-find success!" : "shitty") << '\n';
    return pred;
}


bool pset_iter(persistent_set& p) {
    bool ord = true;
    int len = 0;
    persistent_set::value_type t = INT32_MIN;
    for (auto i = p.begin(); i != p.end(); ++i) {
//        cout << *i << ' ';
        ord &= (*i > t);
        t = (*i);
        len++;
    }
//    cout << '\n';
//    cout << ((ord) ? "All-in-order success!" : "nasty") << '\n';
//    cout << "Length: " << len << '\n';

    return ord;
}

int pset_count(persistent_set& p) {
    int l = 0;
    for (auto i = p.begin(); i != p.end(); ++i) {
        l++;
    }

    return l;
}

bool pset_removeat(persistent_set& p, int at) {
    auto it = p.begin();
    while (at-- > 0)
        it++;

    persistent_set::value_type val = *it;
    p.erase(it);

    auto it2 = p.find(val);
    persistent_set::value_type val2 = *it2;
    bool pred = (p.end() == p.find(val));
//    cout << (pred ? "Remove-at-lookup success" : "fiasco") << '\n';

    if (!pred)
        cout << " || || || " << val << '\n';

    return pred;

}


int main() {
    bool test = true;

    // Case 1: of ordered insert
    {
        bool cs = true;
        persistent_set p;
        pset_populate_o(p, 8192);

        cs &= pset_iter(p); // assert ordered

        cout << "Case 1 status : " << cs << '\n';

        test &= cs;
    }

    // Case 2: of random insert
    {
        bool cs = true;
        persistent_set p;
        pset_populate_r(p, 8192);

        cs &= pset_iter(p); // assert ordered

        cout << "Case 2 status : " << cs <<'\n';

        test &= cs;

    }

    //Case 3: of random deletion
    {
        bool cs = true;

        persistent_set p;
        pset_populate_r(p, 8192);

        for (int i = 0; i < 500; i++) {
            cs &= pset_removeat(p, abs(rand()) % 4000);
        }

        cs &= pset_iter(p);

        cout << "Case 3 status : " << cs << '\n';

        test &= cs;
    }

    //Case 4: of copypasta
    {
        persistent_set p;
        pset_populate_r(p, 50000);

        persistent_set a[50000];
        for (int i = 0; i < 50000; i++)
            a[i] = p;

        cout << "Case 4 : Now waiting - assess memory via external tools\n";
        cin.get();
    }

    //Case 5: of insertion persistence
    {
        bool cs = true;

        persistent_set p;
        pset_populate_r(p, 4096);
        persistent_set copy(p);
        int cc = pset_count(copy);
        pset_populate_r(p, 4096);
        int pc = pset_count(p);

        cs &= (cc == 4096);

        pset_populate_r(copy, 4096);

        cs &= (pc == 8192);


        cout << "Case 5 status : " << cs << '\n';

        test &= cs;
    }

    //Case 6: of erasure persistence
    {
        bool cs = true;
        persistent_set p;
        pset_populate_r(p, 4096);
        persistent_set copy(p);
        int count = pset_count(copy);

        for (int i = 0; i < 500; i++) {
            cs &= pset_removeat(p, abs(rand()) % 3000);
        }

        cs &= (pset_count(copy) == count);

        cout << "Case 6 status : " << cs << '\n';

        test &= cs;
    }

    cout << (test ? "GLOBAL SUCCESS" : "fiasco");
}