#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

int main(int argc, char ** argv){
	if(argc != 3){
		fprintf(stderr, "./check out ans\n");
		exit(1);
	}

	// open files
	FILE *fp_out, *fp_ans;
	if((fp_out = fopen(argv[1], "r")) == NULL){
		fprintf(stderr, "open output error\n");
		exit(0);
	}

	if((fp_ans = fopen(argv[2], "r")) == NULL){
		fprintf(stderr, "open ans error\n");
		exit(0);
	}

	// read file
	int res;
	int err = 0, tot = 0;
	char buf1[512], buf2[512];
	if((res = fscanf(fp_out, "%s", buf1)) == EOF){
		fprintf(stderr, "read error\n");
		exit(0);
	}
	if((res = fscanf(fp_ans, "%s", buf2)) == EOF){
		fprintf(stderr, "read error\n");
		exit(0);
	}

	while(1){
		//fprintf(stderr, "\n%d\n", tot);
		if((res = fscanf(fp_out, "%s", buf1)) == EOF){
			break;
		}

		if((res = fscanf(fp_ans, "%s", buf2)) == EOF){
			break;
		}
		
		if(strcmp(buf1, buf2) != 0){
			//fprintf(stderr, "err\n");
			err ++;
		}
		tot ++;
	}
	float rate = 1.0 - ((float)err) / ((float)tot);
	fprintf(stderr, "rate = %f\n", rate);
	return 0;
}
