#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <time.h>

#define INPUT_FILE_NAME "test_set_1.txt"
#define OUTPUT_FILE_NAME "test_set_1_sol.txt"

struct save_matrix {
    int id;
    int **arr;
};

struct matrix {
    int **arr;
};

int minimum_mul(int *ls, int i, int j) {
    if (i == j) {
        return 0;
    }
    
    int min = INT_MAX;
    
    for (int k = i; k < j; k++) {
        int cnt = minimum_mul(ls, i, k) + minimum_mul(ls, k + 1, j) + ls[i - 1] * ls[k] * ls[j];
        
        if (cnt < min) min = cnt;
    }
    return min;
}

void create_random_matrix(int **arr, int row, int col) {
    srand((unsigned)time(NULL));
    for (int i = 0; i < row; i++) {
        for (int j = 0; j < col; j++) {
            arr[i][j] = rand() % 4 + 1;
        }
    }
}

int **Matrix_dot(int **ar1, int **ar2, int row, int col, int tmp) {
    int **Matrix_tmp = (int **)malloc(sizeof(int) * row * col);
    for (int i = 0; i < row; i++)
        Matrix_tmp[i] = (int *)malloc(sizeof(int) * col);
    
    for (int i = 0; i < row; i++) {
        for (int j = 0; j < col; j++) {
            Matrix_tmp[i][j] = 0;
            for (int k = 0; k < tmp; k++)
                Matrix_tmp[i][j] += ar1[i][k] * ar2[k][j];
        }
    }
    return Matrix_tmp;
}

int main(void) {
    FILE* infp;
    FILE* outfp;
    
    infp = fopen(INPUT_FILE_NAME, "r");
    outfp = fopen(OUTPUT_FILE_NAME, "w");

    int row, col;
    int tmp = 0;
    char c;
    int count_line = 1;
    int **random_Array;
    
    // 주어진 파일 줄 수 계산
    for (c = getc(infp); c != EOF; c = getc(infp))
        if (c == '\n')
            count_line++;
    
    // 주어진 파일 행, 렬 2차원 배열에 담기
    int **matrix = (int**)malloc(sizeof(int) * count_line * 2);
    for (int i = 0; i < count_line; i++) {
        matrix[i] = (int *)malloc(sizeof(int) * 2);
    }
    
    infp = fopen(INPUT_FILE_NAME, "r"); // 파일 다시 오픈
    while (fscanf(infp, "%d %d", &row, &col) != EOF)
    {
        matrix[tmp][0] = row;
        matrix[tmp][1] = col;
        tmp++;
    }
    
    // 1차원 배열로 전환
    int cnt_tmp = count_line + 1;
    int *ls = (int *)malloc(sizeof(int) * cnt_tmp);
    ls[0] = matrix[0][0];
    for (int i = 0; i < count_line; i++) {
        ls[i + 1] = matrix[i][1];
    }
    
    // 최소 연산 곱셈 횟수 계산 알고리즘
    int min = minimum_mul(ls, 1, cnt_tmp - 1);
    
    fprintf(outfp, "%d\n", min);
    fputs("\n", outfp);
    
    // two - demension array store in structure named by save_matrix.
    struct save_matrix *randomArray = (struct save_matrix *)malloc(sizeof(struct save_matrix) * count_line);
    
    for (int i = 0; i < count_line; i++) {
        int row_tmp = matrix[i][0];
        int col_tmp = matrix[i][1];
        
        random_Array = (int **)malloc(sizeof(int) * row_tmp * col_tmp);
        for (int j = 0; j < row_tmp; j++) {
            random_Array[j] = (int *)malloc(sizeof(int) * col_tmp);
        }
        
        create_random_matrix(random_Array, row_tmp, col_tmp);
        randomArray[i].arr = random_Array;
        randomArray[i].id = i + 1;
    }
    
    // Output Matrix Code / 행렬 계산 과정을 matrix라고 이름지은 구조체에 저장시키며 계산 진행.
    struct matrix *Output_Matrix = (struct matrix *)malloc(sizeof(struct matrix) * count_line);
    
    int r_fix = matrix[0][0];
    Output_Matrix[0].arr = randomArray[0].arr;
    for(int i = 1; i < count_line; i++){
        int c_tmp = matrix[i][1];
        int tmp = matrix[i][0];
        Output_Matrix[i].arr = Matrix_dot(Output_Matrix[i - 1].arr, randomArray[i].arr, r_fix, c_tmp, tmp);
    }
    
    fputs("Output Matrix\n", outfp);
    for (int i = 0; i < r_fix; i++) {
        fputs("| ", outfp);
        for (int j = 0; j < matrix[count_line - 1][1]; j++) {
            fprintf(outfp, "%12d ", Output_Matrix[count_line - 1].arr[i][j]);
        }
        fputs(" |\n", outfp);
    }
    fputs("\n", outfp);
    
    // Input Matrix Code
    for (int i = 0; i < count_line; i++) {
        int row_tmp = matrix[i][0];
        int col_tmp = matrix[i][1];
        
        fprintf(outfp, "Input Matrix %d\n", randomArray[i].id);
        for (int k = 0; k < row_tmp; k++) {
            fputs("| ", outfp);
            for (int t = 0; t < col_tmp; t++) {
                fprintf(outfp, "%2d ", randomArray[i].arr[k][t]);
            }
            fputs(" |\n", outfp);
        }
        fputs("\n", outfp);
    }
    
    free(randomArray);
    free(random_Array);
    free(ls);
    free(Output_Matrix);
    free(matrix);

    fclose(infp);
    fclose(outfp);
}
