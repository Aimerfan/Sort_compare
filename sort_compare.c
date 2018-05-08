#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

//#define DEBUG
#define pool_size 500000

int *pool, *cpool;

int epoch(const int);
double diffclktime(clock_t start, clock_t end);
int wpool(const char *filename, const int *pool, int batch_size);
int wrlt_csv(int batch_size, char *poolname, char *algorithm, double time);

int bubble_sort(int[], const int);
int selection_sort(int[], const int);
int insertion_sort(int[], const int);
int quick_sort(int[], const int, const int);
int heap_sort(int[], const int);
static int heap(int[], const int, const int);

int main(){
	srand(time(NULL));
	//printf("%d\n", RAND_MAX);	//RAND_MAX == 32767
	//for(i = 0; i < 100; i++) printf("%d, ", pool[i]);
	pool = (int*)malloc( pool_size * sizeof(int) );
	cpool = (int*)malloc( pool_size * sizeof(int) );
	
	time_t start, end;
	time(&start);
	
	int i, j;
	for(i = 1; i < 7; i++){
		for(j = 0; j < 25; j++){
			if(i * 50000 < pool_size) epoch(i * 50000);
			else puts("Batch size is over then pool size!!!");
		}
	}
	
	time(&end);
	printf("Analysis over. Spend %.0lf secs for all.\n", difftime(end, start));
	
	free(pool);
	free(cpool);
	
	system("pause");
	return 0;
}

int epoch(const int batch_size){
	int i;
	char str[100];
	
	for(i = 0; i < batch_size; i++) pool[i] = (rand() << 15 | rand());
	
	time_t tm_tick = time(NULL);
	struct tm timer = *localtime(&tm_tick);
	sprintf(str, "%02d%02d-%02d%02d%02d-%d.txt", timer.tm_mon+1, timer.tm_mday, timer.tm_hour, timer.tm_min, timer.tm_sec, batch_size);
	#ifndef DEBUG
	wpool(str, pool, batch_size);
	#endif
	
	clock_t start, end;
	
	memcpy(cpool, pool, batch_size * sizeof(int));
	start = clock();
	bubble_sort(cpool, batch_size);
	end = clock();
	wrlt_csv(batch_size, str, "bubble", diffclktime(start, end));
	
	memcpy(cpool, pool, batch_size * sizeof(int));
	start = clock();
	selection_sort(cpool, batch_size);
	end = clock();
	wrlt_csv(batch_size, str, "selection", diffclktime(start, end));
	
	memcpy(cpool, pool, batch_size * sizeof(int));
	start = clock();
	insertion_sort(cpool, batch_size);
	end = clock();
	wrlt_csv(batch_size, str, "insertion", diffclktime(start, end));
	
	memcpy(cpool, pool, batch_size * sizeof(int));
	start = clock();
	quick_sort(cpool, 0, batch_size);
	end = clock();
	wrlt_csv(batch_size, str, "quick", diffclktime(start, end));
	
	memcpy(cpool, pool, batch_size * sizeof(int));
	start = clock();
	heap_sort(cpool, batch_size);
	end = clock();
	wrlt_csv(batch_size, str, "heap", diffclktime(start, end));
	
	puts("---------------------------------------------------------");
	
	return 0;
}

#define MAX_INT ((unsigned long)(-1) >> 1)
//#define MAX_INT (~(1 << (sizeof(long)*8-1)))
double diffclktime(clock_t start, clock_t end){
	if(end >= start) return ((end - start) / (double)CLOCKS_PER_SEC);
	else return ((MAX_INT - (start - end)) / (double)CLOCKS_PER_SEC);
}

int wpool(const char *filename, const int *pool, int batch_size){
	int i;
	FILE* output =  fopen(filename, "w");
	for(i = 0; i < batch_size; i++) fprintf(output, "%d\n", pool[i]);
	fclose(output);
	return 0;
}

int wrlt_csv(int batch_size, char *poolname, char *algorithm, double time){
	static int first = 0;
	static FILE* result;
	
	if(first == 0){
		result = fopen("result.csv", "w");
		printf("%-12s %-24s %-11s %s\n", "batch_size", "poolname", "algorithm", "time(s)");
		fputs("batch_size,poolname,algorithm,time(s)\n", result);
	}
	else result = fopen("result.csv", "a");
	
	printf("%-12d %-24s %-11s %lf\n", batch_size, poolname, algorithm, time);
	fprintf(result, "%d,%s,%s,%lf\n", batch_size, poolname, algorithm, time);
	
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

int quick_sort(int arr[], const int left, const int right){
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
		
		quick_sort(arr, left, j - 1);
		quick_sort(arr, j + 1, right);
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
