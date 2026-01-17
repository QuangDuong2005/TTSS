#include <stdio.h>
#include <omp.h>
#include <stdlib.h>
#include <windows.h>
#include <stdbool.h>
// Tính tỉ lệ song song được
double Al(double s, int thread){
    return (1/s - 1.0/thread)*thread/(thread -1);
}
// Hàm tính song song tích vector
long long vector_dot_product_para(short *vector1, short *vector2, int n) {
    long long sum = 0;
    int i;

    #pragma omp parallel for reduction(+:sum)
        for (i = 0; i < n; i++) {
            sum += vector1[i] * vector2[i];
        }

    return sum;
}   
// Hàm tính tuần tự tích vector
long long vector_dot_product_seq(short *vector1, short *vector2, long n) {
    long long sum = 0;
    for (int i = 0; i<n; i++){
        sum+=vector1[i]*vector2[i];
    }
    return sum;    
}   
void test_len(long n, int z, int thead) {
    // Cấp phát bộ nhớ cho 2 vector
    short *vector1 = (short *)malloc(n * sizeof(short));
    short *vector2 = (short *)malloc(n * sizeof(short));

    // Kiểm tra xem máy tính có đủ bộ nhớ không
    if (vector1 == NULL || vector2 == NULL) {
        printf("Khong du bo nho de cap phat!\n");
        return ;
    }

    //  Khởi tạo dữ liệu ngẫu nhiên để test
    for (long i = 0; i < n; i++) {
        vector1[i] = rand() % 100;
        vector2[i] = rand() % 100;
    }
    double time_para, time_para_avg = 0, time_seq, time_seq_avg = 0;
    long long sum_para,sum_seq;
    double start_para, end_para,start_seq,end_seq;
    vector_dot_product_seq(vector1, vector2, n); // warm up
    for (int i=0; i<z; i++){ // Đo z lần phần tuần tự
        start_seq = omp_get_wtime(); // tạo biến để đo thời gian bắt đầu
        sum_seq = vector_dot_product_seq(vector1, vector2, n);
        end_seq = omp_get_wtime(); // tạo biến để đo thời gian kết thúc
        time_seq = end_seq - start_seq;
        time_seq_avg+=time_seq;
    }
    vector_dot_product_para(vector1, vector2, n); //warm up
    for (int i = 0;i<z;i++){ // Đo trung bình z lần song song
        start_para = omp_get_wtime(); // tạo biến để đo thời gian bắt đầu
        sum_para = vector_dot_product_para(vector1, vector2, n);
        end_para = omp_get_wtime(); // tạo biến để đo thời gian kết thúc
        time_para = end_para - start_para;
        time_para_avg+=time_para;
    }
    time_para_avg = time_para_avg/z;
    time_seq_avg = time_seq_avg/z;
    double speedup_  = time_seq_avg / time_para_avg;

    /* ---- In kết quả ---- */
    printf("%-12ld | %-15lld | %-12.6f | %-15lld | %-12.6f | %-8.2f| %-12s | %lf\n",n, sum_para, time_para_avg, sum_seq, time_seq_avg, speedup_,sum_para == sum_seq? "true" : "false",Al(speedup_,thead));
    free(vector1);
    free(vector2);
}

void test_thread(int thread){
    int num_thread = thread; // Lấy số thread
    printf("Số thread đang dùng là: %d\n", num_thread);
    omp_set_num_threads(num_thread); // Khởi tạo số thread
    long n = 2000000; // Số chiều ban đầu
    int z = 15; // Số lần đo trung bình
    printf("Bắt đầu chạy song song, lấy thời gian là trung bình của %d lần\n",z);
    printf("%-12s | %-15s | %-12s | %-15s | %-12s | %-8s| %-12s| %-12s\n",
           "Số chiều n", "Result_para", "T_para (s)",
           "Result_seq", "T_seq (s)", "Speedup",
        "True ? Flase", "Tỉ lệ tuần tự (A)");
    for (int i = 0;i <14; i++){
        test_len(n,z, num_thread);
        n = n*1.5; // Tăng số chiều vector lên để test
    }

}
int main(){
    system("chcp 65001"); // chuyển sang tiếng việt
    int num_thread = 4; // Lấy số thread
    for (int i = 0;i <4; i++){
        test_thread(num_thread);
        num_thread = num_thread+4; // tăng độ dài thread
        printf("\n--------------------------------------------------------------------\n");
    }
    return 0;
}
