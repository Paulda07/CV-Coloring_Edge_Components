#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "dc_image.h"



#define MIN(a,b)  ( (a) < (b) ? (a) : (b) )
#define MAX(a,b)  ( (a) > (b) ? (a) : (b) )
#define ABS(x)    ( (x) <= 0 ? 0-(x) : (x) )


//--------------------------------------------------
//--------------------------------------------------
// You must modify this disjoint set implementation
//--------------------------------------------------
//--------------------------------------------------

struct DisjointSet;

typedef struct DisjointSet {
	int r,g,b;
	int x,y;
	int rank;
	struct DisjointSet *parent;
} DisjointSet;

DisjointSet *DisjointSetFindRoot(DisjointSet *curr)
{
	// Add code here
	
	return NULL;
}

void DisjointSetUnion(DisjointSet *a, DisjointSet *b)
{
	// Add code here
}

//--------------------------------------------------
//--------------------------------------------------
// The following "run" function runs the entire algorithm
//  for a single vision file
//--------------------------------------------------
//--------------------------------------------------

void run(const char *infile, const char *outpre, int canny_thresh, int canny_blur)
{
	int y,x,i,j;
	int rows, cols, chan;

	//-----------------
	// Read the image    [y][x][c]   y number rows   x cols  c 3
	//-----------------
	byte ***img = LoadRgb(infile, &rows, &cols, &chan);
	printf("img %p rows %d cols %d chan %d\n", img, rows, cols, chan);

	char str[4096];
	sprintf(str, "out/%s_1_img.png", outpre);
	SaveRgbPng(img, str, rows, cols);
	
	//-----------------
	// Convert to Grayscale
	//-----------------
	byte **gray = malloc2d(rows, cols);
	for (y=0; y<rows; y++){
		for (x=0; x<cols; x++) {
			int r = img[y][x][0];   // red
			int g = img[y][x][1];   // green
			int b = img[y][x][2];   // blue
			gray[y][x] =  (r+g+b) / 3;
		}
	}

	sprintf(str, "out/%s_2_gray.png", outpre);
	SaveGrayPng(gray, str, rows, cols);

	//-----------------
	// Box Blur   ToDo: Gaussian Blur is better
	//-----------------
	
	// Box blur is separable, so separately blur x and y
	int k_x=canny_blur, k_y=canny_blur;
	
	// blur in the x dimension
	byte **blurx = (byte**)malloc2d(rows, cols);
	for (y=0; y<rows; y++) {
		for (x=0; x<cols; x++) {
			
			// Start and end to blur
			int minx = x-k_x/2;      // k_x/2 left of pixel
			int maxx = minx + k_x;   // k_x/2 right of pixel
			minx = MAX(minx, 0);     // keep in bounds
			maxx = MIN(maxx, cols);
			
			// average blur it
			int x2;
			int total = 0;
			int count = 0;
			for (x2=minx; x2<maxx; x2++) {
				total += gray[y][x2];    // use "gray" as input
				count++;
			}
			blurx[y][x] = total / count; // blurx is output
		}
	}
	
	sprintf(str, "out/%s_3_blur_just_x.png", outpre);
	SaveGrayPng(blurx, str, rows, cols);
	
	// blur in the y dimension
	byte **blur = (byte**)malloc2d(rows, cols);
	for (y=0; y<rows; y++) {
		for (x=0; x<cols; x++) {
			
			// Start and end to blur
			int miny = y-k_y/2;      // k_x/2 left of pixel
			int maxy = miny + k_y;   // k_x/2 right of pixel
			miny = MAX(miny, 0);     // keep in bounds
			maxy = MIN(maxy, rows);
			
			// average blur it
			int y2;
			int total = 0;
			int count = 0;
			for (y2=miny; y2<maxy; y2++) {
				total += blurx[y2][x];    // use blurx as input
				count++;
			}
			blur[y][x] = total / count;   // blur is output
		}
	}
	
	sprintf(str, "out/%s_3_blur.png", outpre);
	SaveGrayPng(blur, str, rows, cols);
	
	
	//-----------------
	// Take the "Sobel" (magnitude of derivative)
	//  (Actually we'll make up something similar)
	//-----------------
	
	byte **sobel = (byte**)malloc2d(rows, cols);
	
	for (y=0; y<rows; y++) {
		for (x=0; x<cols; x++) {
			int mag=0;
			
			if (y>0)      mag += ABS((int)blur[y-1][x] - (int)blur[y][x]);
			if (x>0)      mag += ABS((int)blur[y][x-1] - (int)blur[y][x]);
			if (y<rows-1) mag += ABS((int)blur[y+1][x] - (int)blur[y][x]);
			if (x<cols-1) mag += ABS((int)blur[y][x+1] - (int)blur[y][x]);
			
			int out = 3*mag;
			sobel[y][x] = MIN(out,255);
		}
	}
	
	
	sprintf(str, "out/%s_4_sobel.png", outpre);
	SaveGrayPng(sobel, str, rows, cols);
	
	//-----------------
	// Non-max suppression
	//-----------------
	byte **nonmax = malloc2d(rows, cols);    // note: *this* initializes to zero!
	
	for (y=1; y<rows-1; y++)
	{
		for (x=1; x<cols-1; x++)
		{
			// Is it a local maximum
			int is_y_max = (sobel[y][x] > sobel[y-1][x] && sobel[y][x]>=sobel[y+1][x]);
			int is_x_max = (sobel[y][x] > sobel[y][x-1] && sobel[y][x]>=sobel[y][x+1]);
			if (is_y_max || is_x_max)
				nonmax[y][x] = sobel[y][x];
			else
				nonmax[y][x] = 0;
		}
	}
	
	sprintf(str, "out/%s_5_nonmax.png", outpre);
	SaveGrayPng(nonmax, str, rows, cols);
	
	//-----------------
	// Final Threshold
	//-----------------
	byte **edges = malloc2d(rows, cols);    // note: *this* initializes to zero!
	
	for (y=0; y<rows; y++) {
		for (x=0; x<cols; x++) {
			if (nonmax[y][x] > canny_thresh)
				edges[y][x] = 255;
			else
				edges[y][x] = 0;
		}
	}
	
	sprintf(str, "out/%s_6_edges.png", outpre);
	SaveGrayPng(edges, str, rows, cols);
	

	//
	// Add your code here ....
	//

}


int main()
{
	system("mkdir out");
	
	//
	// Main simply calls the run function 
	//  with different parameters for each image
	//
	run("puppy.jpg", "puppy", 45, 25);
	run("pentagon.png", "pentagon", 45, 10);
	run("tiger.jpg", "tiger", 45, 10);
	run("houses.jpg", "houses", 45, 10);
	
	printf("Done!\n");
}
