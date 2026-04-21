#include <fstream>
#include <string>
#include <vector>
#include <iostream>
#include <algorithm>

using namespace std;


struct item_t {
  int id,val;
};

struct knaps_t {
  vector<item_t> items;
  int total;
};

void sort_knaps(vector<item_t>& items) {
  sort(items.begin(), items.end(), [](const item_t& a, const item_t& b) {
    return a.val > b.val;
  });
}

int main() {
  int N=30,nprocs=5,max=10;
  vector<item_t> items;
  printf("items: ");

  for (int k=0; k<N; k++) {
    int val = (rand()%max+1);
    item_t item={k,val};
    items.push_back(item);
    printf("(%d,%d) ",k,val);
  }
  printf("\n");

  vector<knaps_t> knapsv(nprocs);
  for (int k=0; k<nprocs; k++)
    knapsv[k].total = 0;

  //IMPLEMENTACIÓN:
  sort_knaps(items);

  printf("\nitems ordenados: ");
  for (auto item: items)
    printf("(%d,%d) ",item.id,item.val);
  printf("\n");

  for (int i=0; i<N; i++) {
    int min_knaps = 0;
    for (int k=1; k<nprocs; k++) {
      if (knapsv[k].total < knapsv[min_knaps].total)
        min_knaps = k;
    }
    knapsv[min_knaps].items.push_back(items[i]);
    knapsv[min_knaps].total += items[i].val;
  }

  printf("\n");
  for (int k=0; k<nprocs; k++) {
    printf("knaps %d: total=%d items: ",k,knapsv[k].total);
    for (auto item: knapsv[k].items)
      printf("(%d,%d) ",item.id,item.val);
    printf("\n");
  }
}