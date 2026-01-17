#include <stdio.h>
#include <omp.h>
#include <stdlib.h>
#include <math.h>
// Tính tỉ lệ song song được
double Al(double s, int thread){
    return (1/s - 1.0/thread)*thread/(thread -1);
}
// Định nghĩa hàm số f
double f(double x, double y){
    return exp(x*x)*cos(x+y);
}
// tính tích phân tuần tự
double Trapezoidal2D_seq(double (*func)(double, double), double a, double b, double c, double d, double epsilon){
    double Imn = 0, Ipmpn = 0, sai_so = 10;
    long n = 8, m = 8; // Khởi tạo số đoạn chia ban đầu
    while (sai_so>epsilon){
        Imn = Ipmpn; // Vì dùng lưới mịn p = 2 và độ dài đoạn sau gấp đôi đoạn trước
                  // nên chỉ cần tính cái Ipn
        double h = (b-a)/(2*n); // Khoảng cách dx của Ipn
        double k = (d-c)/(2*m);
        Ipmpn = 0; //  Tính lại tổng mỗi lần tạo
        for (int i=0; i<2*n;i++){
            for (int j=0; j<2*m; j++){
                Ipmpn += (f(a + i*h, c + j*k)+f(a + (i+1)*h, c + j*k)+f(a + i*h, c + (j+1)*k)+f(a + (i+1)*h, c + (j+1)*k));
            }
        }
        Ipmpn = Ipmpn*h*k/4;
        sai_so = fabs((Ipmpn-Imn)/3); // Tính toán sai số lưới mịn
        n = n*2;
        m = m*2;
    }
    return Ipmpn;
}
// Tạo hàm tính tích phân song song
double Trapezoidal2D_para(double (*func)(double, double), double a, double b, double c, double d, double epsilon){
    double Imn = 0, Ipmpn = 0, sai_so = 10;
    long n = 8, m = 8; // Khởi tạo số đoạn chia ban đầu
    #pragma omp parallel // Cho vòng song song ở đây để tránh bị hủy do cái for para
    {
        while (sai_so>epsilon){
            #pragma omp single
            {
                Imn = Ipmpn; // Vì dùng lưới mịn p = 2 và độ dài đoạn sau gấp đôi đoạn trước
                        // nên chỉ cần tính cái Ipn
                Ipmpn = 0; //  Tính lại tổng mỗi lần tạo
            }
            double h = (b-a)/(2*n); // Khoảng cách dx của Ipn
            double k = (d-c)/(2*m);

            #pragma omp for reduction(+:Ipmpn) // Không song song trong vòng lặp 2 vì nó sẽ mất nhiều thời gian chia việc hơn
            for (int i=0; i<2*n;i++){
                for (int j=0; j<2*m; j++){
                    Ipmpn += (f(a + i*h, c + j*k)+f(a + (i+1)*h, c + j*k)+f(a + i*h, c + (j+1)*k)+f(a + (i+1)*h, c + (j+1)*k));
                }
            }
            
        #pragma omp single
        {
            Ipmpn = Ipmpn*h*k/4;
            sai_so = fabs((Ipmpn-Imn)/3); // Tính toán sai số lưới mịn
            n = n*2;
            m = m*2;
        }
        #pragma omp barrier
        }
    }
    return Ipmpn;
}

void test(double (*func)(double, double),double a, double b, double c, double d, double epsilon, int z, int numthread) {
    double time_para, time_para_avg = 0, time_seq, time_seq_avg = 0; // Khởi tạo các thông số đo thời gian
    double sum_para,sum_seq;
    double start_para, end_para,start_seq,end_seq;
    Trapezoidal2D_seq(f,a,b,c,d,epsilon); // warm up
    for (int i=0; i<z; i++){ // Đo z lần phần tuần tự
        start_seq = omp_get_wtime(); // tạo biến để đo thời gian bắt đầu
        sum_seq = Trapezoidal2D_seq(f,a,b,c,d,epsilon);
        end_seq = omp_get_wtime(); // tạo biến để đo thời gian kết thúc
        time_seq = end_seq - start_seq;
        time_seq_avg+=time_seq;
    }
    Trapezoidal2D_para(f,a,b,c,d,epsilon); //warm up
    for (int i = 0;i<z;i++){ // Đo trung bình z lần song song
        start_para = omp_get_wtime(); // tạo biến để đo thời gian bắt đầu
        sum_para = Trapezoidal2D_para(f,a,b,c,d,epsilon);
        end_para = omp_get_wtime(); // tạo biến để đo thời gian kết thúc
        time_para = end_para - start_para;
        time_para_avg+=time_para;
    }
    time_para_avg = time_para_avg/z;
    time_seq_avg = time_seq_avg/z;
    double speedup_  = time_seq_avg / time_para_avg;

    /* ---- In kết quả ---- */
    printf("| %-15.9lf | %-12.9f | %-15.9lf | %-12.9f | %-8.2f| %f\n", sum_para, time_para_avg, sum_seq, time_seq_avg, speedup_,Al(speedup_,numthread));
}

int main(){
    system("chcp 65001"); // chuyển sang tiếng việt trên cmd
    int max = omp_get_max_threads();
    int num_thread = 12; // Lấy số thread

    omp_set_num_threads(num_thread); // Khởi tạo số thread
    int z = 15; // Số lần đo trung bình
    double a = 0, b = 1, c = 2, d = 5; // Các đoạn lấy tích phân
    double epsilon = 1e-6; // Khởi tạo sai số

    for (int i = 0;i <5; i++){
        omp_set_num_threads(num_thread);
        printf("Số thread đang dùng là: %d\n", num_thread);
        printf("Bắt đầu chạy song song, lấy thời gian là trung bình của %d lần\n",z);
        printf("| %-15s | %-12s | %-15s | %-12s | %-8s| Tỉ lệ tuần tự (A)\n"
            , "I_para", "T_para (s)",
            "I_seq", "T_seq (s)", "Speedup");
        test(f,a,b,c,d,epsilon,z,num_thread);
        num_thread = num_thread+1; // Tăng số thread lên để test
        printf("\n--------------------------------------------------------------------\n");
    }

    return 0;
}



