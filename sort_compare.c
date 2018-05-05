#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <time.h>

#define max_allow_epoch 2 //each epoch == 5 threads for every algorithm

typedef struct record{
	int b_size;
	char *p_name;
	char *algo;
	double time;
}record;
typedef struct pool{
	int* array;
	pthread_mutex_t key;
	int len;
}pool;
/*sort algorithm function pointer*/
typedef int (*SORT)(int[], const int);

void epoch(const int batch_size);				//Non-thread security!!!(include wpool)
record analysis(SORT algo, pool ori);
double diffclktime(clock_t start, clock_t end);
char* wpool(pool wpl);	//Non-thread security!!!(must be used at least blank 1 sec.)
int wrlt_csv(record rcd);

int bubble_sort(int[], const int);
int selection_sort(int[], const int);
int insertion_sort(int[], const int);
int quick_sort(int[], const int);
static int quick_divide(int[], const int, const int);
int heap_sort(int[], const int);
static int heap(int[], const int, const int);

int main(){
	srand(time(NULL));
	
	system("pause");
	return 0;
}

void epoch(const int batch_size){
	int i;
	char str[100];
	
	pool ori;
	ori.array = (int*)malloc(batch_size*sizeof(int));
	for(i = 0; i < batch_size; i++) ori.array[i] = (rand() << 15 | rand());
	pthread_mutex_init(&ori.key, NULL);
	ori.len = batch_size;
	wpool(ori);
	
	
	//clock_t start, end;
	/*memcpy(cpool, pool, batch_size * sizeof(int));
	start = clock();
	bubble_sort(cpool, batch_size);
	end = clock();
	sprintf(buffer, "%lf sec for bubble sort.", ((double)(end - start)) / CLOCKS_PER_SEC);
	wrlt(buffer);
	
	time_t start, end;
	memcpy(cpool, pool, batch_size * sizeof(int));
	start = time(NULL);
	bubble_sort(cpool, batch_size);
	end = time(NULL);
	wrlt_csv(batch_size, str, "bubble", end - start);*/
	
	return;
}

record analysis(SORT algo, pool ori){
	record ret;
	
	return ret;
}

#define MAX_INT ((unsigned long)(-1) >> 1)
//#define MAX_INT (~(1 << (sizeof(long)*8-1)))
double diffclktime(clock_t start, clock_t end){
	if(end >= start) return ((end - start) / (double)CLOCKS_PER_SEC);
	else return ((MAX_INT - (start - end)) / (double)CLOCKS_PER_SEC);
}

char* wpool(pool wpl){
	char *filename = (char*)malloc(128*sizeof(char));
	time_t tm_tick = time(NULL);
	struct tm timer = *localtime(&tm_tick);
	sprintf(filename, "%02d%02d-%02d%02d%02d-%d.txt", timer.tm_mon+1, timer.tm_mday, timer.tm_hour, timer.tm_min, timer.tm_sec, wpl.len);
	
	int i;
	FILE* output =  fopen(filename, "w");
	for(i = 0; i < wpl.len; i++) fprintf(output, "%d\n", wpl.array[i]);
	fclose(output);
	return filename;
}

int wrlt_csv(record rcd){
	static int first = 0;
	static FILE* result;
	
	printf("%-12d %-24s %-11s %lf\n", rcd.b_size, rcd.p_name, rcd.algo, rcd.time);
	
	if(first == 0){
		result = fopen("result.csv", "w");
		printf("%-12s %-24s %-11s %s\n", "batch_size", "poolname", "algorithm", "time(s)");
		fputs("batch_size,poolname,algorithm,time(s)\n", result);
	}
	else result = fopen("result.csv", "a");
	
	fprintf(result, "%d,%s,%s,%lf\n", rcd.b_size, rcd.p_name, rcd.algo, rcd.time);
	fclose(result);
	first = 1;
	return 0;
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
