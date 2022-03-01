// # -*- coding: utf-8 -*-
// """
// Created on Tue Feb 22 12:18:06 2022

// @author: Paul Ledala
// """

#include <stdio.h>
#include <string.h>

//--------------------------------------------------
//--------------------------------------------------
// You must modify this disjoint set implementation
//--------------------------------------------------
//--------------------------------------------------

struct DisjointSet;

typedef struct DisjointSet {
	
	 int rank []; 
     int parent []; 
     int n;
     /*int r,g,b;
	int x,y;
	int rank;
	struct DisjointSet *parent;
	*/
} DisjointSet;

   
  
    // Creates n single item sets
    void makeSet(int *parent, int size)
    {
        printf("Here1b\n");
        for (int i = 0; i < size; i++) {
            parent[i] = i;
        }
    }
/*DisjointSet *DisjointSetFindRoot(DisjointSet *curr)
{
	  // Finds set of given item x

    
        // Finds the representative of the set
        // that x is an element of
        if (parent[x] != x) {
  
            // if x is not the parent of itself
            // Then x is not the representative of
            // his set,
            parent[x] = find(parent[x]);
  
            // so we recursively call Find on its parent
            // and move i's node directly under the
            // representative of this set
        }
  
        return parent[x];
    
	
	//return NULL;
}

void DisjointSetUnion(DisjointSet *a, DisjointSet *b)
{
	// Add code here
	// Find current sets of x and y
        int xset = find(x);
        int yset = find(y);
  
        // If they are already in same set
        if (xset == yset)
            return;
  
        // Put smaller ranked item under
        // bigger ranked item if ranks are
        // different
        if (rank[xset] < rank[yset]) {
            parent[xset] = yset;
        }
        else if (rank[xset] > rank[yset]) {
            parent[yset] = xset;
        }
  
        // If ranks are same, then increment
        // rank.
        else {
            parent[yset] = xset;
            rank[xset] = rank[xset] + 1;
        }
    }
}
*/
int initDisjointSet(DisjointSet *s, int size)
{
    // if (s == NULL)
    //     return -1;
    printf("Here1a\n");
    printf("Here1b\n");
    s->n = size;
    printf("Here1c\n");
    printf("%d", s->n);
    int rank[5];
    printf("Here1d\n");
    memset (rank, 0, size* sizeof(int));
    printf("Here1e\n");
    memcpy(s->rank, rank, 40);
    printf("Here1f\n");
    printf("Here1d\n");
    memset (s->rank, 0, size* sizeof(int));
    printf("Here1d\n");
    memset (s->parent, 0, size* sizeof(int));
    s->n = size;
    makeSet(s->parent, size);
    return 0;
}

enum {LENGTH = 10};

int initDisjointSet(DisjointSet *s, int size)
{
    if (s == NULL)
        return -1;

    memset (s->rank, 0, size);
    memset (s->parent, 0, size);
    s->n = size;
    p->alive = alive;

    return 0;
}

enum {LENGTH = 10};

int main() {
   // printf() displays the string inside quotation
<<<<<<< Updated upstream
   DisjointSet a; //= { .flag = true, .value = 123, .stuff = 0.456 };
=======
   struct DisjointSet a; //= { .flag = true, .value = 123, .stuff = 0.456 };
>>>>>>> Stashed changes
//    printf("Hello, World!");

//    DisjointSet me = { "John\0", "McCarthy\0", 24, 1};
    // Person arr[LENGTH];

    // for (int i = 0; i < LENGTH; ++i) {
<<<<<<< Updated upstream

        initDisjointSet (&a, 5);
    // }

    for (int i = 0; i < LENGTH; ++i) {
        printPerson(&arr[i]);
        printf("\n");
    }

    exit(EXIT_SUCCESS);
=======
        printf("Here1\n");
        initDisjointSet (&a, 5);
    // }

        printf("Here2\n");
    for (int i = 0; i < 5; ++i) {
        printf("%d\n", (a.rank[i]));
    }

>>>>>>> Stashed changes
   return 0;
}