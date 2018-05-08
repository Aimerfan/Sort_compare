#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

/*sort algorithm function pointer*/
typedef int (*SORT)(int[], const int);
typedef struct record{
	int  *pool;
	SORT algo;
	int  b_size;
	char p_name[64];
	char *algo_name;
	double time;
}record;
typedef struct pthread_link{
	pthread_t id;
	struct pthread_link* next;
}link;

void* epoch(void* size);				//Non-thread security!!!(include wpool)
void* analysis(void* rec);
double diffclktime(clock_t start, clock_t end);
void wpool(record* rec);	//Non-thread security!!!(must be used at least blank 1 sec.)
void wrlt_csv(record rcd);

int bubble_sort(int[], const int);
int selection_sort(int[], const int);
int insertion_sort(int[], const int);
int quick_sort(int[], const int);
static int quick_divide(int[], const int, const int);
int heap_sort(int[], const int);
static int heap(int[], const int, const int);

/*!!!limit semaphores count!!!*/
sem_t t_limit;
/*!!!result.csv file mutex lock!!!*/
pthread_mutex_t resfile;

void printlist(link* head){
	link* tmp = head;
	printf("alive : ");
	do{
		printf("%d ", tmp->id);
		tmp = tmp->next;
	}while(tmp != head);
	putchar('\n');
}

int main(){
	srand(time(NULL));
	sem_init(&t_limit, 0, 10);
	pthread_mutex_init(&resfile, NULL);
	
	printf("%-12s %-24s %-11s %s\n", "batch_size", "poolname", "algorithm", "time(s)");
	
	int i = 0, j = 50000;
	time_t block;
	link* handle = (link*)malloc(sizeof(link));
	handle->next = handle;
	pthread_create(&(handle->id), NULL, epoch, &j);
	printf("create %d\n", handle->id);
	printlist(handle);
	i++;
	sleep(1);
	
	link *prev = handle, *tmp;
	while(handle){
		if(j <= 300000 && !sem_trywait(&t_limit)){
			tmp = (link*)malloc(sizeof(link));
			tmp->next = handle->next;
			handle->next = tmp;
			pthread_create(&(tmp->id), NULL, epoch, &j);
			//-------------------------------------------------
			printf("create %d by size %d\n", tmp->id, j);
			printlist(handle);
			//-------------------------------------------------
			i++;
			sleep(1);
			if(i == 25){
				i = 0;
				j += 50000;
			}
		}
		if(pthread_kill(handle->id, 0)){
			tmp = handle;
			//pthread_join(tmp->id, NULL);
			if(prev == handle){
				free(tmp);
				handle = NULL;
				//-------------------------------------------------
				printf("empty\n");
				//-------------------------------------------------
				break;
			}
			else{
				handle = handle->next;
				prev->next = handle;
				//-------------------------------------------------
				printf("killed %d\n", tmp->id);
				printlist(handle);
				//-------------------------------------------------
				free(tmp);
			}
		}
		prev = handle;
		handle = handle->next;
	}
	
	system("pause");
	return 0;
}

void* epoch(void* size){
	int i, batch_size = *(int*)size;
	int *pool = (int*)malloc(batch_size*sizeof(int));
	for(i = 0; i < batch_size; i++) pool[i] = (rand() << 15 | rand());
	
	record recs[5] = {0};
	recs[0].b_size = batch_size;
	recs[0].pool = pool;
	wpool(&recs[0]);
	for(i = 1; i < 5; i++) memcpy(&recs[i], &recs[0], sizeof(record));
	recs[0].algo = bubble_sort;
	recs[1].algo = selection_sort;
	recs[2].algo = insertion_sort;
	recs[3].algo = quick_sort;
	recs[4].algo = heap_sort;
	
	pthread_t sorts[5] = {0};
	for(i = 0; i < 5; i++){
		sem_wait(&t_limit);
		pthread_create(&sorts[i], NULL, analysis, (void*)&recs[i]);
	}
	for(i = 4; i >= 0; i--){
		pthread_join(sorts[i], NULL);
		sem_post(&t_limit);
	}
	
	for(i = 0; i < 5; i++)  printf("%-12d %-24s %-11s %lf\n", recs[i].b_size, recs[i].p_name, recs[i].algo_name, recs[i].time);
	puts("---------------------------------------------------------");
	
	pthread_mutex_lock(&resfile);
	for(i = 0; i < 5; i++) wrlt_csv(recs[i]);
	pthread_mutex_unlock(&resfile);
	
	pthread_exit(NULL);
}

void* analysis(void* rec){
	record *ret = (record*)rec;
	int *cpool = (int*)malloc(ret->b_size*sizeof(int));
	memcpy(cpool, ret->pool, ret->b_size*sizeof(int));
	
	if(ret->algo == bubble_sort) ret->algo_name = "bubble";
	else if(ret->algo == selection_sort) ret->algo_name = "selection";
	else if(ret->algo == insertion_sort) ret->algo_name = "insertion";
	else if(ret->algo == quick_sort) ret->algo_name = "quick";
	else if(ret->algo == heap_sort) ret->algo_name = "heap";
	
	clock_t start, end;
	start = clock();
	ret->algo(cpool, ret->b_size);
	end = clock();
	ret->time = diffclktime(start, end);
	free(cpool);
	pthread_exit(NULL);
}

#define MAX_INT ((unsigned long)(-1) >> 1)
//#define MAX_INT (~(1 << (sizeof(long)*8-1)))
double diffclktime(clock_t start, clock_t end){
	if(end >= start) return ((end - start) / (double)CLOCKS_PER_SEC);
	else return ((MAX_INT - (start - end)) / (double)CLOCKS_PER_SEC);
}

void wpool(record* rec){
	char filename[128];
	time_t tm_tick = time(NULL);
	struct tm timer = *localtime(&tm_tick);
	sprintf(filename, "%02d%02d-%02d%02d%02d-%d.txt", timer.tm_mon+1, timer.tm_mday, timer.tm_hour, timer.tm_min, timer.tm_sec, rec->b_size);
	strcpy(rec->p_name, filename);
	
	int i;
	/*FILE* output =  fopen(filename, "w");
	for(i = 0; i < rec->b_size; i++) fprintf(output, "%d\n", rec->pool[i]);
	fclose(output);*/
	return;
}

void wrlt_csv(record rcd){
	static int first = 0;
	static FILE* result;
	
	if(first == 0){
		result = fopen("result.csv", "w");
		fputs("batch_size,poolname,algorithm,time(s)\n", result);
	}
	else result = fopen("result.csv", "a");
	
	fprintf(result, "%d,%s,%s,%lf\n", rcd.b_size, rcd.p_name, rcd.algo_name, rcd.time);
	fclose(result);
	first = 1;
	return;
}

int bubble_sort(int arr[], const int len){
	int i, j, tmp;
	for(i = 0; i < len; i++){
		for(j = 0; j < len - i - 1; j++){
			if(arr[j] > arr[j + 1]){
				tmp = arr[j];
				arr[j] = arr[j + 1];
				arr[j + 1] = tmp;
			}
		}
	}
	return 0;
}

int selection_sort(int arr[], const int len){
	int i, j, min;
	for(i = 0; i < len - 1; i++){
		min = i;
		for(j = i + 1; j < len; j++){
			if(arr[min] > arr[j]) min = j;
		}
		j = arr[i];
		arr[i] = arr[min];
		arr[min] = j;
	}
	return 0;
}

int insertion_sort(int arr[], const int len){
	int i, j, tmp;
	for (i = 1; i < len; i++){
        tmp = arr[i];
        for (j = i; j > 0 && arr[j-1] > tmp; j--){
    		arr[j] = arr[j-1];
        }
        arr[j] = tmp;
    }
	return 0;
}

int quick_sort(int arr[], const int len){
	return quick_divide(arr, 0, len);
}

int quick_divide(int arr[], const int left, const int right){
	int i, j, pivot, tmp;
	if(left < right - 1){
		i = left, j = right;
		pivot = arr[left];
		while(i < j){
			while(arr[++i] < pivot && i < j){}
			while(arr[--j] > pivot && i < j){}
			if(i < j){
				tmp = arr[i];
				arr[i] = arr[j];
				arr[j] = tmp;
			}
		}
		tmp = arr[left];
		arr[left] = arr[j];
		arr[j] = tmp;
		
		quick_divide(arr, left, j - 1);
		quick_divide(arr, j + 1, right);
	}
	return 0;
}

int heap_sort(int arr[], const int len){
	int i, tmp;
	//初始化，調整所有可能成為parent的節點 
	for (i = len / 2 - 1; i >= 0; i--){
		heap(arr, i, len - 1);
	} 
	for (i = len - 1; i > 0; i--) {
		tmp = arr[0];
		arr[0] = arr[i];
		arr[i] = tmp;
		heap(arr, 0, i - 1);
	}
	return 0;
}

static int heap(int arr[], const int root, const int end){
	int dad = root;
	int son = dad * 2 + 1;
	int tmp;
	while (son <= end) {
		//選擇較大的子節點 
		if (son + 1 <= end && arr[son] < arr[son + 1]) son++;
		//dad > son, 調整結束, break
		if (arr[dad] > arr[son])
			break;
		//dad節點往下移動繼續調整 
		else {
			tmp = arr[dad];
			arr[dad] = arr[son];
			arr[son] = tmp;
			dad = son;
			son = dad * 2 + 1;
		}
	}
	return 0;
}
