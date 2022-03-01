#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <limits.h>
#include "dc_image.h"




#define MIN(a,b)  ( (a) < (b) ? (a) : (b) )
#define MAX(a,b)  ( (a) > (b) ? (a) : (b) )
#define ABS(x)    ( (x) <= 0 ? 0-(x) : (x) )
#define DISTANCE_THRESHOLD 200


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
	// printf("Function is called\n");
	if (curr->parent != curr)
		curr->parent = DisjointSetFindRoot(curr->parent);
	return curr->parent;
}

void DisjointSetUnion(DisjointSet *a, DisjointSet *b)
{
	// Add code here
	// printf("Function is called\n");
	DisjointSet *a_set = DisjointSetFindRoot(a);
	// printf("Function ends\n");
	DisjointSet *b_set = DisjointSetFindRoot(b);
	// printf("Not coming out\n");
	if (a_set == b_set)
		return;
	if (a_set->rank < b_set->rank){
		a_set->parent = b_set;
		a_set->r = b_set->r;
		a_set->g = b_set->g;
		a_set->b = b_set->b;
	}
		
	else if (a_set->rank > b_set->rank){
		b_set->parent = a_set;	
		b_set->r = a_set->r;
		b_set->g = a_set->g;
		b_set->b = a_set->b;
	}	
	else{
		a_set->parent = b_set;
		a_set->r = b_set->r;
		a_set->g = b_set->g;
		a_set->b = b_set->b;
		b_set->rank = b_set->rank + 1;
	}
}

double gaussian_pdf(double sd, int x, int original_x){
	return exp(-(1/2)*pow(((x-original_x)/sd),2)/sd)/(sd*sqrt((2*3.14159265358979323846)));
}

double mean (int connected_coordinates[], int odd){
	double sol = 0;
	for (int i = odd; i<sizeof(connected_coordinates)/sizeof(connected_coordinates[0]); i+=2){
		if (connected_coordinates[i] == -1)
			return sol/(i/2);
		sol+= connected_coordinates[i];
	}
}

double sd (int connected_coordinates[], int mean, int odd){
	double sol = 0;
	for (int i = odd; i<sizeof(connected_coordinates)/sizeof(connected_coordinates[0]); i+=2){
		if (connected_coordinates[i] == -1)
			return sqrt(sol/(i/2));
		sol = sol+ pow((connected_coordinates[i]-mean), 2);
	}
}
gaussian_kernel_smoothening(int connected_coordinates[], int *x, int *y)
{
	int original_x = *x;
	int original_y = *y;
	double mean_x = mean (connected_coordinates, 0);
	double mean_y = mean (connected_coordinates, 1);
	double sd_x  = sd (connected_coordinates, mean_x, 0);
	double sd_y = sd (connected_coordinates, mean_y, 1);
	double sol = 0;
	double pdf_x = 0;
	double pdf_y = 0;
	double denominator = 0;
	int smoothened_x = 0;
	int smoothened_y = 0;
	for (int i = 0; i<sizeof(connected_coordinates)/sizeof(connected_coordinates[0]); i+=2){
		if (connected_coordinates[i] == -1)
			{
				*x = smoothened_x/(int)denominator;
				*y = smoothened_y/(int)denominator;
			}
		pdf_x = gaussian_pdf(sd_x, connected_coordinates[i], original_x);
		pdf_y = gaussian_pdf(sd_y, connected_coordinates[i+1], original_y);
		smoothened_x += (int)(pdf_x*pdf_y)*original_x;
		smoothened_y += (int)(pdf_x*pdf_y)*original_y;
		denominator += pdf_x*pdf_y;
	}

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
	// Connected components to color each edge region using a different color
	//

	struct DisjointSet **connected_components = (struct DisjointSet**)malloc(rows*sizeof(struct DisjointSet*));
	for (y=0; y<rows; y++) {
		connected_components[y] = (struct DisjointSet*)calloc(cols,sizeof(struct DisjointSet));		
		for (x=0; x<cols; x++) {	
			if (edges[y][x] != 255){
				continue;
			}
				
			// printf("Inside initialization\n");		
			connected_components[y][x].r = y%255;
			connected_components[y][x].g = x%255;
			connected_components[y][x].b = (x+y)%255;
			connected_components[y][x].x = x;
			connected_components[y][x].y = y;
			connected_components[y][x].rank = 0;
			connected_components[y][x].parent = &connected_components[y][x];
		}
	}
	printf("Initialized connected_components\n");
	for (y=0; y<rows; y++) {
		for (x=0; x<cols; x++) {
			if (edges[y][x] != 255)
				continue;
			// printf("%d %d %d %d\n", x, y, connected_components[y][x].x, connected_components[y][x].y);
			
			int m = y;
			int n = x;
			while (y-m < DISTANCE_THRESHOLD && m > 0)
				m--;
			while (x-n < DISTANCE_THRESHOLD && n > 0)
				n--;
			int lowest_rank = INT_MAX;
			int lowest_m = m;
			int lowest_n = n;
			// printf("%d  %d  %d  %d %d %d\n",m,n, y, x, connected_components[y][x].y, connected_components[y][x].x);
			while (m <= y + DISTANCE_THRESHOLD && m< rows)
			{
				while (n<= x + DISTANCE_THRESHOLD && n< cols){
					if (edges[m][n] == 255 && connected_components[m][n].rank<lowest_rank && (m != y || n!= x)){
						lowest_rank = connected_components[m][n].rank;
						lowest_m = m;
						lowest_n = n;
					}
					n++;	
				}
				m++;
			}
			
			
			if (edges[lowest_m][lowest_n] == 255){
				
				// printf("%d %d\n", rows, cols);
				// printf("%d %d %d %d %d %d %d\n", lowest_m, lowest_n, lowest_rank, y, x, &connected_components[y][x], &connected_components[lowest_m][lowest_n]);
				// printf("%d %d %d %d %d %d\n", connected_components[lowest_m][lowest_n].y, connected_components[lowest_m][lowest_n].x, connected_components[y][x].y, connected_components[y][x].x, connected_components[y][x].parent, connected_components[lowest_m][lowest_n].parent);
				DisjointSetUnion(&connected_components[lowest_m][lowest_n], &connected_components[y][x]);
				
			}
			// printf("%d %d %d %d\n", connected_components[lowest_m][lowest_n].x, connected_components[lowest_m][lowest_n].y, connected_components[x][y].x, connected_components[x][y].y);
			// printf("Not coming out\n");
			
		}
		
	}
	byte ***colored_edges = malloc3d(rows, cols, 3);
	
	for (y=0; y<rows; y++) {
		for (x=0; x<cols; x++) {
			colored_edges[y][x][0] = connected_components[y][x].r;
			colored_edges[y][x][1] = connected_components[y][x].g;
			colored_edges[y][x][2] = connected_components[y][x].b;
		}
	}
	sprintf(str, "out/%s_7_colored_edges.png", outpre);
	// printf("Here\n");
	SaveRgbPng(colored_edges, str, rows, cols);
	
	//
	// Implementing Gaussian Kernal to smooth the Connected components pixels 
	//
	byte ***gaussian_kernal = malloc3d(rows, cols, 3);
	int connected_coordinates [DISTANCE_THRESHOLD*DISTANCE_THRESHOLD*8];
	
	gaussian_kernal = colored_edges;
	for (y=0; y<rows; y++) {
		for (x=0; x<cols; x++) {
			if (edges[y][x] != 255)
				continue;
			// printf("%d %d %d %d\n", x, y, connected_components[y][x].x, connected_components[y][x].y);
			int iterator = 0;
			int m = y;
			int n = x;
			while (y-m < DISTANCE_THRESHOLD && m > 0)
				m--;
			while (x-n < DISTANCE_THRESHOLD && n > 0)
				n--;
			int lowest_rank = INT_MAX;
			int lowest_m = m;
			int lowest_n = n;
			// printf("%d  %d  %d  %d %d %d\n",m,n, y, x, connected_components[y][x].y, connected_components[y][x].x);
			while (m <= y + DISTANCE_THRESHOLD && m< rows)
			{
				while (n<= x + DISTANCE_THRESHOLD && n< cols){
					if (edges[m][n] == 255 && connected_components[m][n].parent == connected_components[x][y].parent){
						connected_coordinates[iterator] = m;
						connected_coordinates[iterator+1] = n;
						iterator+=2;
					}
					n++;	
				}
				m++;
			}
			connected_coordinates[iterator] = -1;
			connected_coordinates[iterator+1] = -1;
			int smoothened_x = x;
			int smoothened_y = y;
			gaussian_kernel_smoothening(connected_coordinates, &smoothened_x, &smoothened_y);
			gaussian_kernal[smoothened_x][smoothened_y][0] = 255 - (gaussian_kernal[x][y][0]%255);
			gaussian_kernal[smoothened_x][smoothened_y][1] = 255 - (gaussian_kernal[x][y][1]%255);
			gaussian_kernal[smoothened_x][smoothened_y][2] = 255 - (gaussian_kernal[x][y][2]%255);
		}
		
	}

	sprintf(str, "out/%s_7_smoothened_edges.png", outpre);
	// printf("Here\n");
	SaveRgbPng(gaussian_kernal, str, rows, cols);

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
