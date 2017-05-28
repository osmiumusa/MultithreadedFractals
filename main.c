#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "bitmap.h"
#include "complex.h"

#define MAX(a,b) (a)>(b)? (a): (b)
#define MIN(a,b) (a)<(b)? (a): (b)

typedef struct threadinfo {
    int id;
    int linechunksize;
    double* r;
    double* g;
    double* b;
    int numsamples;
    long int maxiter;
    double deltai,deltar,maxnorm;
    complex* c;
} threadInfo;

void* worker_thread(void*);
long int iterate(complex,complex,long int, double);
void HSVtoRGB(double*,double*,double*,double);

int main(int argc, char** argv) {
    //usage: size, a, b, rmax, rmin, imax, imin, outputfile, numthreads, maxiter, maxnorm
    if(argc<12) {
        printf("Usage: %s size a b rmin rmax imin imax outputfile numthreads maxiter maxnorm\n",argv[0]);
        printf("\tsize: number of pixels of image (both width and height)\n");
        printf("\ta and b: the julia set to calculate in terms of a+bi\n");
        printf("\tr/i max/min: the bounds of the image in the real and imaginary axes\n");
        printf("\toutputfile: image file to output; will output a bitmap\n");
        printf("\tnumthreads: number of threads to spawn to make image\n");
        printf("\tmaxiter: the maximum number of iterations per pixel\n");
        printf("\tmaxnorm: the maximum normal vector during the iteration\n\n");
        return 1;
    }

    //Parse the arguments
    int numsamples = atoi(argv[1]);
    complex c;
    sscanf(argv[2],"%lf",&c.real);
    sscanf(argv[3],"%lf",&c.imag);
    double rmax, rmin, imax, imin;
    sscanf(argv[4],"%lf",&rmin);
    sscanf(argv[5],"%lf",&rmax);
    sscanf(argv[6],"%lf",&imin);
    sscanf(argv[7],"%lf",&imax);
    char* outputfile = argv[8];
    int numthreads = atoi(argv[9]);
    long int maxiter;
    sscanf(argv[10],"%ld",&maxiter);
    double maxnorm;
    sscanf(argv[11],"%lf",&maxnorm);

    //Set up some initial math
    double deltar=(rmax-rmin)/numsamples;
	double deltai=(imax-imin)/numsamples;
    int linechunksize = MIN(numsamples/(numthreads)+1,numsamples);

    //Set up the global image space
    double* r = (double*) malloc(sizeof(double)*numsamples*numsamples);
    double* g = (double*) malloc(sizeof(double)*numsamples*numsamples);
    double* b = (double*) malloc(sizeof(double)*numsamples*numsamples);

    //Set up the assignments for each thread to follow
    int i;
    threadInfo* threadinfo = (threadInfo*) malloc(sizeof(threadInfo)*numthreads);
    for(i=0;i<numthreads;i++) {
        threadinfo[i].id = i;
        threadinfo[i].linechunksize = linechunksize;
        threadinfo[i].r = r;
        threadinfo[i].g = g;
        threadinfo[i].b = b;
        threadinfo[i].numsamples = numsamples;
        threadinfo[i].maxiter = maxiter;
        threadinfo[i].maxnorm = maxnorm;
        threadinfo[i].deltar = deltar;
        threadinfo[i].deltai = deltai;
        threadinfo[i].c = &c;
    }

    //Next, let's spawn the threads and tell them what to do
    pthread_t* threads = (pthread_t*) malloc(sizeof(pthread_t)*numthreads);
    for(i=0;i<numthreads;i++) {
        pthread_create(threads+i,NULL,&worker_thread,threadinfo+i);
    }

    //Now we wait for them to finish...
    for(i=0;i<numthreads;i++) {
        pthread_join(threads[i],NULL);
    }

    //At this point, our image should be in memory, let's write it!
    write_rgb_bmp(outputfile,r,g,b,numsamples,numsamples,0,1,0);

    //Now clean up!
    free(r);
    free(g);
    free(b);
    free(threads);
    free(threadinfo);
}

void* worker_thread(void* arg) {
    threadInfo* info = (threadInfo*) arg;
    //printf("Thread %d starting \n",info->id); fflush(stdout);
    int x,y;
    int start = (info->id)*(info->linechunksize);
	complex z;
	for(y=start; y<info->linechunksize+start; y++) {
        if(y>=info->numsamples) break;
		for(x=0; x<info->numsamples; x++) {
			z.real=(x-info->numsamples/2)*info->deltar;
			z.imag=(y-info->numsamples/2)*info->deltai;
			long int i = iterate(z,*info->c,info->maxiter,info->maxnorm);
			HSVtoRGB(&(info->r[info->numsamples*y+x]),&(info->g[info->numsamples*y+x]),&(info->b[info->numsamples*y+x]),i);
		}
	}
    //printf("Thread %d is done \n",info->id); fflush(stdout);
}

long int iterate(complex z, complex c, long int maxiter, double maxnorm) {
	long int out = 0;
	complex* z0 = malloc(sizeof(complex));
	complex* zn;
	complex_clone(&z,z0);
	while(out <= maxiter && complex_norm(z0)<maxnorm) {
		zn = complex_mul(z0,z0);
		complex_clone(zn,z0);
		free(zn);
		complex_addx(z0,&c);
		out++;
	}
	return out;
}

void HSVtoRGB(double *r, double *g, double *b, double h) {
	int i;
	h /= 60;
	i = floor( h );
	switch( i ) {
	case 0:
		*r = 1;
		*g = (h-i);
		*b = 0;
		break;
	case 1:
		*r = (1-h+i);
		*g = 1;
		*b = 0;
		break;
	case 2:
		*r = 0;
		*g = 1;
		*b = (h-i);
		break;
	case 3:
		*r = 0;
		*g = (1-h+i);
		*b = 1;
		break;
	case 4:
		*r = (h-i);
		*g = 0;
		*b = 1;
		break;
	default:
		*r = 1;
		*g = 0;
		*b = (1-h+i);
		break;
	}
}
