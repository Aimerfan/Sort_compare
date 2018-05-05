#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define pool_size 1000000

int *pool, *cpool; // pool = 排序前的資料 // cpool = 排序後的資料 
char name_log[6][25][100] = {};
double time_log[6][25][5] = {}; 


int epoch(const int, int);  //主要工作 : 產生亂數與排序
int wpool(const char *, const int *, int); //次要工作 : 產生特定檔案 
void records(const char *, int, int, int, double); 
void wrlt_csv();
//int wrlt_csv(int batch_size, char *poolname, char *algorithm, int time); //主要工作 : 產生時間記錄檔

int bubble_sort(int[], const int);   		 //泡沫排序法 
int selection_sort(int[], const int);		 //選擇排序法 
int insertion_sort(int[], const int);		 //插入排序法 
int quick_sort(int[], const int, const int); //快速排序 
int heap_sort(int[], const int);			 //堆積排序 
int heap(int[], const int, const int);       //堆積排序 : 產生Max heap tree 

int main(){
	srand(time(NULL));

	pool = (int*)malloc( pool_size * sizeof(int) );
	cpool = (int*)malloc( pool_size * sizeof(int) );
	
	int i, j;
	for(i = 1; i < 7; i++){ //總共六種 case 
		for(j = 0; j < 25; j++){ //每種 case 跑 25 次 
			if(i * 50000 < pool_size) epoch(i * 50000, j);  
			else puts("Batch size is over then pool size!!!");
		}
	}
	
	free(pool);
	free(cpool);
	
	system("pause");
	return 0;
}

int epoch(const int batch_size,int number){
	int i;
	char str[100], str_1[100],str_2[100];
	time_t tm_tick = time(NULL);
	struct tm timer = *localtime(&tm_tick);
	
	sprintf(str, "%02d%02d-%02d%02d-%d-%02d", timer.tm_mon+1, timer.tm_mday, timer.tm_hour, timer.tm_min, batch_size, number + 1);
	sprintf(str_1, "%02d%02d-%02d%02d-%d-%02d-Before_Sort.txt", timer.tm_mon+1, timer.tm_mday, timer.tm_hour, timer.tm_min, batch_size, number + 1);
	sprintf(str_2, "%02d%02d-%02d%02d-%d-%02d-After_Sort.txt", timer.tm_mon+1, timer.tm_mday, timer.tm_hour, timer.tm_min, batch_size, number + 1);
	
	//創立亂數並記錄排序前亂數 
	for(i = 0; i < batch_size; i++){
		pool[i] = (rand() << 15 | rand());
	}
	wpool(str_1, pool, batch_size);
	
	//紀錄時間 
	clock_t start, end;
	double period;
	
	//泡沫排序法
	memcpy(cpool, pool, batch_size * sizeof(int));
	start = clock();
	bubble_sort(cpool, batch_size);
	end = clock();
	period = ((double)(end - start) / CLK_TCK);
	records(str, batch_size, number, 0, period);
	
	//選擇排序法
	memcpy(cpool, pool, batch_size * sizeof(int));
	start = clock();
	selection_sort(cpool, batch_size);
	end = clock();
	period = ((double)(end - start) / CLK_TCK);
	records(str, batch_size, number, 1, period);
	
	//插入排序法 
	memcpy(cpool, pool, batch_size * sizeof(int));
	start = clock();
	insertion_sort(cpool, batch_size);
	end = clock();
	period = ((double)(end - start) / CLK_TCK);
	records(str, batch_size, number, 2, period);
	
	//快速排序法 
	memcpy(cpool, pool, batch_size * sizeof(int));
	start = clock();
	quick_sort(cpool, 0, batch_size);
	end = clock();
 	period = ((double)(end - start) / CLK_TCK);
	records(str, batch_size, number, 3, period);
	
	//堆積排序法	
	memcpy(cpool, pool, batch_size * sizeof(int));
	start = clock();
	heap_sort(cpool, batch_size);
	end = clock();
	period = ((double)(end - start) / CLK_TCK);
	records(str, batch_size, number, 4, period);
	
	//輸出排序結果 
	wpool(str_2, cpool, batch_size);
	
	return 0;
}

int wpool(const char *filename, const int *pool, int batch_size){
	int i;
	FILE* output =  fopen(filename, "w");
	for(i = 0; i < batch_size; i++) fprintf(output, "%d\n", pool[i]);
	fclose(output);
	return 0;
}

void records(const char *poolname, int batch_size, int index, int algorithm, double period){
	static int first = 0;
	static int change_case = 0;
	static FILE* result;
	
	int i = (batch_size / 50000) - 1;
	char *algorithm_type;
	
	if(first == 0){
		result = fopen("result.csv", "w");
		printf("%-24s %-10s %-10s %-10s %-10s %-10s\n", "Poolnames", "Bubble", "Selection", "Insertion", "Quick", "Heap");
		fputs("Poolnames,Bubble,Selection,Insertion,Quick,Heap\n", result);
	}
	else result = fopen("result.csv", "a");
	
	strcpy(name_log[i][index], poolname);
    time_log[i][index][algorithm] = period;
    
    if(change_case == 0){
		printf("%-24s ", name_log[i][index]);
		fprintf(result, "%s,",name_log[i][index]);
		change_case = 1;
	}
	
	if(algorithm < 4){
		printf("%-11lf% ", time_log[i][index][algorithm]);
		fprintf(result,"%lf,",time_log[i][index][algorithm]);	
	} 
	else{ 
	    printf("%-11lf%\n", time_log[i][index][algorithm]);
		fprintf(result,"%lf\n",time_log[i][index][algorithm]);
		change_case = 0;	
	} 
	
	fclose(result);
	first = 1;
}

/*int wrlt_csv(int batch_size, char *poolname, char *algorithm, int time){
	static int first = 0;
	static FILE* result;
	
	if(first == 0){
		result = fopen("result.csv", "w");
		printf("%-24s %-11s %-11s %-11s %-11s %-11s\n", "Poolnames", "Bubble", "Selection", "Insertion", "Quick", "Heap");
		fputs("Poolnames,Bubble,Selection,Insertion,Quick,Heap\n", result);
	}
	else result = fopen("result.csv", "a");
	
	printf("%-12d %-24s %-11s %lf\n", poolname, , algorithm, time);
	fprintf(result, "%d,%s,%s,%lf\n", batch_size, poolname, algorithm, time);
	
	fclose(result);
	first = 1;
	
	return 0;
}*/

int bubble_sort(int arr[], const int len){
	int i, j, temp;
	for(i = 0; i < len; i++){
		for(j = 0; j < len - i - 1; j++){
			if(arr[j] > arr[j + 1]){
				temp = arr[j];
				arr[j] = arr[j + 1];
				arr[j + 1] = temp;
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
	int i, j, temp;
	for (i = 1; i < len; i++){
        temp = arr[i];
        for (j = i; j > 0 && arr[j-1] > temp; j--){
    		arr[j] = arr[j-1];
        }
        arr[j] = temp;
    }
	return 0;
}

int quick_sort(int arr[], const int left, const int right){
	int i, j, pivot, temp;
	if(left < right - 1){
		i = left, j = right;
		pivot = arr[left];
		while(i < j){
			while(arr[++i] < pivot && i < j){}
			while(arr[--j] > pivot && i < j){}
			if(i < j){
				temp = arr[i];
				arr[i] = arr[j];
				arr[j] = temp;
			}
		}
		temp = arr[left];
		arr[left] = arr[j];
		arr[j] = temp;
		
		quick_sort(arr, left, j - 1);
		quick_sort(arr, j + 1, right);
	}
	return 0;
}

int heap_sort(int arr[], const int len){
	int i, temp;
	//初始化，調整所有可能成為parent的節點 
	for (i = len / 2 - 1; i >= 0; i--){
		heap(arr, i, len - 1);
	} 
	for (i = len - 1; i > 0; i--) {
		temp = arr[0];
		arr[0] = arr[i];
		arr[i] = temp;
		heap(arr, 0, i - 1);
	}
	return 0;
}

int heap(int arr[], const int root, const int end){
	int dad = root;
	int son = dad * 2 + 1;
	int temp;
	while (son <= end) {
		//選擇較大的子節點 
		if (son + 1 <= end && arr[son] < arr[son + 1]) son++;
		//dad > son, 調整結束, break
		if (arr[dad] > arr[son])
			break;
		//dad節點往下移動繼續調整 
		else {
			temp = arr[dad];
			arr[dad] = arr[son];
			arr[son] = temp;
			dad = son;
			son = dad * 2 + 1;
		}
	}
	return 0;
}
