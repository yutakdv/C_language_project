#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <stdbool.h>

#define INPUT_FILE_NAME "test_set_1.txt"
#define OUTPUT_FILE_NAME "test_set_1_sol.txt"
#define STRING_SIZE 256

struct point {
    double *Arr;
    int ch; // centroid (어느 그룹에 속하는 지)
    int idx; // 인덱스번호 지정
};

struct point *points;
struct point *clusterHead;
struct point *prev_arr;

int cluster = 0;
int dimension = 0;
int data = 0;

// 거리 계산
double Distance(double *arr, double *head_arr);

int main(void) {
    FILE* infp;
    FILE* outfp;
    srand((unsigned)time(NULL));

    infp = fopen(INPUT_FILE_NAME, "r");
    outfp = fopen(OUTPUT_FILE_NAME, "w");
    char line[STRING_SIZE];

    fscanf(infp, "%d %d %d", &cluster, &dimension, &data);

    struct point *points = (struct point *)malloc(sizeof(struct point) * data);
    struct point *clusterHead = (struct point *)malloc(sizeof(struct point) * cluster);
    struct point *prev_arr = (struct point *)malloc(sizeof(struct point) * data);
    
    double **data_store = (double **)malloc(sizeof(double) * data * dimension);
    for (int i = 0; i < data; i++) {
        data_store[i] = (double *)malloc(sizeof(double) * dimension);
    }

    int line_tmp = 0;
    while (!feof(infp)) {
        int tmp = 1;
        fgets(line, sizeof(line), infp);
        char *ptr = strtok(line, " ");

        while (ptr != NULL) {
            if (atof(ptr) == '\0') break;
            
            if (tmp <= dimension) {
                data_store[line_tmp][tmp - 1] = atof(ptr);
                if (tmp == dimension) line_tmp++;
                tmp++;
            }
            ptr = strtok(NULL, " ");
        }
    }

    // random k개의 cluster head를 정함
    int *random_number = (int *)malloc(sizeof(int) * cluster);
    for (int i = 0; i < cluster; i++) {
        random_number[i] = rand() % data;
        for (int j = 0; j < i; j++) {
            if (random_number[i] == random_number[j]) i--;
        }
    }

    // set Random Cluster Head
    for (int index = 0; index < cluster; index++) {
        clusterHead[index].Arr = data_store[random_number[index]];
        clusterHead[index].ch = index + 1;
        clusterHead[index].idx = random_number[index];
    }

    // 주어진 data 저장
    for (int i = 0; i < data; i++) {
        points[i].Arr = data_store[i];
        points[i].ch = -1;
        points[i].idx = i + 1;
    }

    // prev_arr
    for (int i = 0; i < data; i++) {
        prev_arr[i].idx = -1;
    }

    bool flag = true;
    int count = 0;

    while (flag) {
        // 주어진 데이터 각각이 어느 그룹에 속하는지 분류
        for (int i = 0; i < data; i++) {
            // 모든 cluster head와 거리를 계산, 가장 작은 거리값을 가진 centroid로 분류 작업
            int nearestIdx = 0;
            float nearestDistance = Distance(points[i].Arr, clusterHead[0].Arr);

            for (int j = 1; j < cluster; j++) {
                float _d = Distance(points[i].Arr, clusterHead[j].Arr);

                if (nearestDistance > _d) {
                    nearestDistance = _d;
                    nearestIdx = j;
                }
            }

            int clusterHead_idx = 0;
            for (int j = 0; j < cluster; j++) {
                if ((clusterHead[j].ch - 1) == nearestIdx) {
                    clusterHead_idx = clusterHead[j].idx;
                    break;
                }
            }
            points[i].ch = clusterHead_idx;
        }

        double moveDistance = 0;
        for (int ch = 0; ch < cluster; ch++) {
            int cnt = 0;
            float *tmp_arr = (float *)malloc(sizeof(float) * dimension);  // 모든 데이터 x좌표 합, 모든 데이터 y좌표 합, 모든 데이터 z좌표 합, ... 저장 배열

            for (int j = 0; j < data; j++) { // 모든 데이터 확인
                if (points[j].ch == clusterHead[ch].idx) { // 군집에 포함되어있는지 체크
                    cnt++; // 군집 개수++

                    for (int idx = 0; idx < dimension; idx++) {
                        tmp_arr[idx] += points[j].Arr[idx];
                    }
                }
            } 

            // 새로운 클러스터로 지정
            if (cnt == 0) cnt = 1;
            else {
                double *ClusterHead_means = (double *)malloc(sizeof(double) * dimension);
                memset(ClusterHead_means, 0, sizeof(double) * dimension);

                for (int j = 0; j < dimension; j++) {
                    ClusterHead_means[j] = tmp_arr[j] / cnt;
                    moveDistance += pow((ClusterHead_means[j] - clusterHead[ch].Arr[j]), 2);
                    clusterHead[ch].Arr[j] = ClusterHead_means[j];
                }
            }
            free(tmp_arr);
        }

        // 종료 조건
        bool breakpoint = false; 
        for (int c = 0; c < cluster; c++) {
            for (int d = 0; d < data; d++) {
                if (points[d].ch == clusterHead[c].idx) {
                    if (prev_arr[d].idx == points[d].idx) {
                        prev_arr[d].idx = points[d].idx;
                        breakpoint = true;
                    }
                    else {
                        breakpoint = false;
                        prev_arr[d].idx = points[d].idx;
                    }
                }
            }
        }

        // Ouput 
        fprintf(outfp, "[Itr : %d]\n", count + 1);
        count += 1;
        for (int j = 0; j < cluster; j++) {
            fprintf(outfp, "Cluster %d(centroid_idx -> %d) : ", j + 1, clusterHead[j].idx);
            for (int k = 0; k < data; k++) {
                if (points[k].ch == clusterHead[j].idx) { // centroid에 포함되어있는 점들    
                    fprintf(outfp, "%d ", points[k].idx); 
                }
            }
            fprintf(outfp, "\n");
        }
         
        // 무한 루프 탈출해야하는지 체크
        if (breakpoint) break;
        else flag = (moveDistance == 0.0) ? false : true;
    }

    free(points);
    free(clusterHead);
    free(random_number);
    free(data_store);
    free(prev_arr);
    fclose(infp);
    fclose(outfp);

    return 0;
}

// 거리 계산
double Distance(double *arr, double *head_arr) {
    double tmp = 0;
    for (int i = 0; i < dimension; i++) {
        tmp += (head_arr[i] - arr[i]) * (head_arr[i] - arr[i]);
    }
    return sqrt(tmp);
}