#include <hash.c>

struct hashentry {
    unsigned long hashVal;
    unsigned int numRuns;
    unsigned int numWins;
}

typedef struct hashentry he
#define HTSIZE (1<<24)
#define MODHTSZ(x) (x&(HTSIZE-1))

he hashtable[HTSIZE];

*he probe(key){
    unsigned int k = MODHTSZ(key);
    unsigned int j = 1;
    while(hashtable[k].hashVal!=key && hashtable[k].hashVal!=0){
        k=MODHTSZ(k+j);
        j+=1;
    }
    return &hashtable[k];
}