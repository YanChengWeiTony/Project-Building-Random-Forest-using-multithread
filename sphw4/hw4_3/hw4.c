#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <time.h>

#define TRAIN_DATA_SIZE (26150)
#define DATA_PER_TREE (40)
#define DEBUG (0)
#define BUFFERSIZE (4096)

//Global Data
float sample_data[TRAIN_DATA_SIZE][35];
int sample_num;
//Type Define
typedef struct node{
	float thre;
	int dim;
	int ans;
	struct node * L;
	struct node * R;
	int t;
} Node;

typedef struct feature{
	float val;
	int ID;
} Feature;

typedef struct task{
	int datalist[DATA_PER_TREE];
	int size;
	Node * root;
} Task;

typedef struct test{
	Node **root;
	float *test_data;
	int *ans_sheet;
	int id_s;
	int id_f;
	int tree_num;
} Test;

//Functions
void Display_Buf(Feature * buf, int size){
	for(int i=0;i<size;i++){
		fprintf(stderr, "(ID=%d, %f)\t", buf[i].ID, buf[i].val);
	}
	fprintf(stderr, "\n\n");
}

void printid(int *buf, int sz, int dim){
	fprintf(stderr, "\n\n");
	fprintf(stderr, "id:\n");
	for(int i=0;i<sz;i++) fprintf(stderr, "(ID=%d , val[%d]=%f, ans=%f)\n", buf[i], dim, sample_data[buf[i]][dim], sample_data[buf[i]][34]);
	if(dim != 34) fprintf(stderr, "\n");
	return;
}

void printnode(Node *root){
	if(root == NULL) return;
	fprintf(stderr, "(thre = %f, dim = %d, ans = %d)\n", root->thre, root->dim, root->ans);
	printnode(root->L);
	printnode(root->R);
	return;
}

int compare(const void *a, const void *b){
	if(((Feature *)a)->val < ((Feature *)b)->val) return -1;
	if(((Feature * )a)->val == ((Feature *)b)->val) return 0;
	return 1;
}

void solve(Task* stack, int *num){
	int data_size = stack[*num].size;
	int *datalist = stack[*num].datalist;
	Node * root = stack[*num].root;

	//Termination Condition
	int Pure = -1;
	for(int i=0;i<data_size;i++){
		if(i == 0) Pure = sample_data[ datalist[i] ][34];
		else{
			if(Pure != sample_data[ datalist[i] ][34]){
				Pure = -1;
				break;
			}
		}
	}
	if(Pure != -1){
		root->L = NULL;
		root->R = NULL;
		root->thre = -1;
		root->dim = -1;
		root->ans = Pure;

		(*num) --;

		if(DEBUG){
			printid(datalist, data_size, 34);
			fprintf(stderr, "leaf\n\n\n");
		}

		return;
	}
	//Store datalist into buf, sorting
	Feature buf[35][data_size];
	for(int dim=1;dim<34;dim++){
		for(int i=0;i<data_size;i++){
			buf[dim][i].val = sample_data[ datalist[i] ][dim];
			buf[dim][i].ID = datalist[i];
		}
		qsort(&(buf[dim][0]), data_size, sizeof(Feature), compare);

		//Debug
		//Display_Buf(&(buf[dim][0]), data_size);
	}
	//find best cutting point
	float GI_min = 64;
	int dim_min = -1, idx_min = -1;
	float Thres_min = -1;
	for(int dim=1;dim<34;dim++){
		//ans_sum
		float ans_sum[data_size];
		float ans_sum_buf = 0;
		for(int i=0;i<data_size;i++){
			ans_sum_buf += sample_data[ buf[dim][i].ID ][34];
			ans_sum[i] = ans_sum_buf;
		}

		//GI
		for(int i=0;i<data_size - 1;i++){
			//i, i+1
			//GI = sum[j=L,R]: (2 * f1_j * (1-f1_j))

			float f1_L = ans_sum[i] / ((float)(i + 1)), f1_R = (ans_sum[data_size - 1] - ans_sum[i]) /((float) (data_size - 1 - i));
			float tmp = 2.0 * f1_L * (1.0 - f1_L) + 2.0 * f1_R * (1.0 - f1_R);
			if(tmp < GI_min){
				GI_min = tmp;
				dim_min = dim;
				idx_min = i;
				Thres_min = (buf[dim][i].val + buf[dim][i + 1].val) / 2.0;
			}
		}

	}
	
	if(DEBUG){
		printid(datalist, data_size, dim_min);
		fprintf(stderr, "dim_min=%d, thre_min = %f(ID:%d -- ID:%d ), idx_min = %d, GI_min=%f\n", dim_min, Thres_min, buf[dim_min][idx_min].ID, buf[dim_min][idx_min + 1].ID , idx_min, GI_min);
	}

	//Root, Divide Data
	root->thre = Thres_min;
	root->dim = dim_min;
	root->ans = -1;
	root->L = (Node *)malloc(sizeof(Node));
	root->R = (Node *)malloc(sizeof(Node));

	//push
	stack[*num + 1].size = idx_min + 1;
	stack[*num].size = data_size - 1 - idx_min;
	stack[*num + 1].root = root->L;
	stack[*num].root = root->R;

	for(int i=0;i<data_size;i++){
		if(i <= idx_min) stack[*num + 1].datalist[i] = buf[dim_min][i].ID;
		else stack[*num].datalist[i - 1 - idx_min] = buf[dim_min][i].ID;
	}

	(*num) ++;
	return;
}


void *Build_Tree(void * root_pass){
	Node *root = (Node *)root_pass;

	//fprintf(stderr, "t=%d\n", root->t);
	root->L = NULL;
	root->R = NULL;
	root->thre = -1;
	root->dim = -1;
	root->ans = -1;

	Task stack[DATA_PER_TREE + 1];
	int num = 1;
	stack[1].size = DATA_PER_TREE;
	stack[1].root = root;
	
	for(int i=0;i<DATA_PER_TREE;i++){
		stack[1].datalist[i] = rand() % sample_num;
		//fprintf(stderr, "id:%d\n", ID_List[i]);
	}

	while(num != 0){
		//fprintf(stderr, "num:%d\n", num);
		solve(stack, &num);
	}
	//if(DEBUG) printnode(root);
	pthread_exit(0);
}


int test_func(Node * root, float * test_data){
	if(root->ans == 0) return 0;
	if(root->ans == 1) return 1;

	if(test_data[ root->dim ] < root->thre) return test_func(root->L, test_data);
	return test_func(root->R, test_data);
}
	
void *Run_Test(void * mytest){
	Test * buf = (Test*) mytest;
	int id_s = buf->id_s, id_f = buf->id_f;
	float *test_data = buf->test_data;
	int *ans_sheet = buf->ans_sheet;
	int tree_num = buf->tree_num;
	Node **root = buf->root;

	//fprintf(stderr, "Run: %d ~ %d\n", id_s, id_f);

	for(int i=id_s;i<=id_f;i++){
		int zero_num = 0, one_num = 0;
		for(int t=0;t<tree_num;t++){
			int ans = test_func(root[t], &(test_data[ 35 * i]));
			if(ans == 0) zero_num ++;
			else if(ans == 1) one_num ++;
			else{
				fprintf(stderr, "test error\n");
				exit(0);
			}
		}
		int final_ans = (zero_num > one_num) ? 0 : 1;
		//fprintf(stderr, "ans: %d\n", final_ans);
		ans_sheet[i] = final_ans;
	}

	pthread_exit(NULL);
}

int main(int argc, char ** argv){
	if(argc != 9){
		fprintf(stderr, "argument error\n$./hw4 -data data_dir -output submission -tree tree_num -thread thread_num\n");
		exit(0);
	}

	//Open Files
	FILE *fp_test, *fp_sample, *fp_out;
	char path_test[512];
	char charbuf[512];
	strcpy(charbuf, argv[2]);
	strcpy(path_test, strcat(strcat(charbuf, "/"), "testing_data"));
	if((fp_test = fopen(path_test, "r")) == NULL){
		fprintf(stderr, "open test file error\n");
		exit(0);
	}

	if((fp_out = fopen(argv[4], "w")) == NULL){
		fprintf(stderr, "open output file error\n");
		exit(0);
	}

	if((fp_sample = fopen("training_data", "r")) == NULL){
		fprintf(stderr, "open training file error\n");
		exit(0);
	}

	//Read Sample File
	int res, brk = 0;
	sample_num = 0;
	while(!brk && sample_num <= TRAIN_DATA_SIZE){
		for(int i=0;i<35;i++){
			if((res = fscanf(fp_sample, "%f", &(sample_data[sample_num][i]))) == EOF){
				brk = 1;
				break;
			}
		}
		sample_num++;
	}

	//Create thread, build tree
	int tree_num = atoi(argv[6]);
	srand(time(NULL));
	Node * root[tree_num];

	int thread_num = atoi(argv[8]);
	pthread_t tid[thread_num];

	for(int t=0;t<tree_num;t++){
		//fprintf(stdout, "t=%d\n", t);
		root[t] = (Node *) malloc(sizeof(Node));
		root[t]->t = t;
		//Build_Tree( (void *) root[t]);

		int thread_idx = t % thread_num;
		if(t >= thread_num){
			pthread_join(tid[thread_idx], NULL);
		}
		pthread_create(&tid[thread_idx], NULL, Build_Tree, (void *) root[t]); 
	}

	//Wait for thread
	int thread_max = (tree_num > thread_num) ? thread_num : tree_num;
	for(int t=0;t<thread_max;t++){
		pthread_join(tid[t], NULL);
	}
	if(DEBUG){
		fprintf(stderr, "print start\n");
		for(int t=0;t<tree_num;t++){
			printnode(root[t]);
		}
	}

	fprintf(stderr, "Build done\nStart test\n");
	//test data
	if(fprintf(fp_out, "id,label\n") == -1){
		fprintf(stderr, "write error\n");
		exit(0);
	}
	

	float test_data[BUFFERSIZE][35];
	int ans_sheet[BUFFERSIZE];
	pthread_t tid_t[thread_num];
	Test mytest[thread_num];
	brk = 0;
	int ID = 0;//test obj;
	while(1){
		//read into buffer
		int buf_num = 0;
		while(buf_num < BUFFERSIZE){
			for(int i=0;i<34;i++){
				if((res = fscanf(fp_test, "%f", &(test_data[buf_num][i]))) == EOF){
					brk = 1;
					break;
				}
			}
			if(brk) break;
			buf_num ++;
		}

		//thread
		int task_num = (int)(buf_num / thread_num);
		int idx = 0;//in Buffer
		for(int t=0;t<thread_num;t++){
			mytest[t].root = root;
			mytest[t].test_data = &(test_data[0][0]);
			mytest[t].ans_sheet = ans_sheet;
			mytest[t].tree_num = tree_num;
			
			if(t == thread_num - 1){
				mytest[t].id_s = idx;
				mytest[t].id_f = buf_num - 1;
			}else{
				mytest[t].id_s = idx;
				mytest[t].id_f = idx + task_num - 1;
				idx += task_num;
			}

			pthread_create(&tid_t[t], NULL, Run_Test, (void*) &mytest[t]);
		}


		for(int t=0;t<thread_num;t++){
			pthread_join(tid_t[t], NULL);
		}

		//write into file
		for(int i=0;i<buf_num;i++){
			if(fprintf(fp_out, "%d,%d\n", ID, ans_sheet[i]) == -1){
				fprintf(stderr, "write error\n");
				exit(0);
			}
			ID ++;
		}

		if(brk == 1) break;
	}
	return 0;
}
