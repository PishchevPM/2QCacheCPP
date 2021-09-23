#include "cache.cpp"
#include <iostream>

using namespace std;
using namespace ppmipt;

int loadfakedata (int page)
{
    return page;
}

int main()
{
    size_t sz;
    unsigned long counter = 0;
    unsigned long npages;
    int tmp;
    cin >> sz >> npages;
    Cache2Q<int> cache((sz + 3)/ 4, sz / 2, (sz + 1)/ 4);

    for (unsigned long i = 0; i < npages; i++)
    {
        cin >> tmp;
        cache.get_page(tmp, loadfakedata, &counter);
    }

    cout << "Number of pages: " << npages << ", hits: " << counter << ", hitrate: " << (double) 100 * counter / npages << "\%" << endl;
}