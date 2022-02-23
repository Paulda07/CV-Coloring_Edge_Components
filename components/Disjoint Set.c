# -*- coding: utf-8 -*-
"""
Created on Tue Feb 22 12:18:06 2022

@author: Paul Ledala
"""

#include <stdio.h>

//--------------------------------------------------
//--------------------------------------------------
// You must modify this disjoint set implementation
//--------------------------------------------------
//--------------------------------------------------

struct DisjointSet;

typedef struct DisjointSet {
	/*int r,g,b;
	int x,y;
	int rank;
	struct DisjointSet *parent;
	*/
	 int *rank, *parent, n;
} DisjointSet;

    DisjSet(int n)
    {
        rank = new int[n];
        parent = new int[n];
        this->n = n;
        makeSet();
    }
  
    // Creates n single item sets
    void makeSet()
    {
        for (int i = 0; i < n; i++) {
            parent[i] = i;
        }
    }
DisjointSet *DisjointSetFindRoot(DisjointSet *curr)
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

int main() {
   // printf() displays the string inside quotation
   printf("Hello, World!");
   return 0;
}