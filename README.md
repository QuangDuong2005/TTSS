# TTSS
Bài tập cuối kỳ môn Tính Toán Song Song
# Bao cao Thuc hanh Tinh toan Song song voi OpenMP

Du an nay thuc hien cai dat va danh gia hieu nang cua hai bai toan tinh toan co ban su dung thu vien OpenMP tren ngon ngu C. Muc tieu la so sanh hieu suat giua thuc thi tuan tu va thuc thi song song de danh gia toc do tang toc Speedup va hieu qua cua da luong.


---

## Yeu cau he thong
* Trinh bien dich: GCC co ho tro OpenMP.
* Thu vien can thiet: omp.h, stdio.h, stdlib.h, math.h.
* He dieu hanh: Windows. Code mau hien tai toi uu cho Windows voi windows.h va lenh system.

---

## Bai 1: Tinh tich vo huong cua hai vector

### Mo ta bai toan
Tinh tong tich cac phan tu tuong ung cua hai vector A va B co kich thuoc n.

$$Sum = \sum_{i=0}^{n-1} A[i] \times B[i]$$

### Giai thuat va Cai dat
* Tuan tu: Ham vector_dot_product_seq duyet vong lap for tu 0 den n, nhan tung cap phan tu va cong don vao bien sum.
* Song song: Ham vector_dot_product_para su dung chi thi pragma omp parallel for reduction.
    * Phan chia: Vong lap duoc chia deu cho cac luong.
    * Reduction: Menh de reduction tao cac ban sao cuc bo cua bien sum cho moi luong de tranh xung dot du lieu, sau do cong gop lai khi ket thuc vung song song.

### Kich ban kiem thu
* Du lieu dau vao: Khoi tao ngau nhien so nguyen short.
* Quy trinh: Tang dan kich thuoc n va so luong luong de quan sat Speedup.
* Do luong: Thoi gian duoc do trung binh qua 15 lan chay sau khi da chay khoi dong warm-up.

---

## Bai 2: Tinh tich phan 2 lop

### Mo ta bai toan
Tinh gan dung tich phan cua ham f tai x, y tren mien hinh chu nhat a, b va c, d.

$$I \approx \int_{a}^{b} \int_{c}^{d} e^{x^2}\cos(x+y) \,dy\,dx$$

### Giai thuat: Quy tac hinh thang thich nghi
Chuong trinh tu dong tang do min cua luoi tuc la gap doi so doan chia n, m cho den khi sai so giua hai lan tinh lien tiep nho hon epsilon la 1e-6.

### Chien luoc song song hoa
De toi uu hieu nang va giam chi phi tao luong, vung song song duoc thiet ke bao trum ca vong lap thich nghi while.

1. pragma omp parallel: Khoi tao nhom luong mot lan duy nhat ben ngoai vong lap while.
2. pragma omp single: Chi mot luong thuc hien viec cap nhat bien trung gian, reset tong va kiem tra sai so.
3. pragma omp for reduction: Tinh toan tong dien tich hinh thang duoc chia song song cho cac luong.
4. pragma omp barrier: Dong bo hoa tat ca cac luong truoc khi kiem tra dieu kien lap de dam bao tinh dung dan cua du lieu.

---

## Huong dan bien dich va chay

Su dung gcc voi co -fopenmp de kich hoat thu vien OpenMP.

### Bien dich Bai 1 Vector
```bash
gcc Tinh_tich_vohuong_vector.c -o Tinh_tich_vohuong_vector -fopenmp
./Tinh_tich_vohuong_vector
Bai 2: 
gcc Tinh_tich_phan.c -o Tinh_tich_phan -fopenmp -lm
./Tinh_tich_phan
