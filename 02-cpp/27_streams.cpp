/*
 * Bài: I/O stream: định dạng bằng iomanip, manipulator "dính" và không dính, stringstream, trạng thái lỗi
 * Chủ đề: C++ (Udemy section 19)
 * Dự đoán trước khi chạy: 1234.5678 in mặc định ra gì? setprecision(2) có và không có fixed? setw áp dụng cho mấy lần in? hex có giữ cho lần in sau không? Đọc "12 abc" vào hai biến int thì sao?
 * Ý chính: Mặc định in 6 chữ số CÓ NGHĨA (không phải 6 số sau dấu phẩy). setprecision(n) không có fixed = n chữ số có nghĩa, có fixed = n số sau dấu phẩy. setw chỉ áp dụng cho lần in KẾ TIẾP; các manipulator khác (fixed, hex, left, setfill, boolalpha) giữ nguyên tới khi đổi. Đọc lỗi → stream vào trạng thái fail, mọi lần đọc sau đều bị bỏ qua cho tới khi clear().
 * Thử thêm: Ghi ra file bằng ofstream rồi đọc lại bằng ifstream + getline. Kiểm tra if (!file.is_open()).
 */
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
using namespace std;

int main()
{
    double d = 1234.5678;
    cout << "mac dinh:            " << d << ", 1234567.89 -> " << 1234567.89 << '\n';
    cout << "setprecision(2):     " << setprecision(2) << d << '\n';
    cout << "fixed setprecision(2): " << fixed << d << '\n';
    cout << "van con fixed:       " << 3.14159 << '\n';
    cout.unsetf(ios::fixed);
    cout << setprecision(6);

    cout << '[' << setw(6) << 42 << "][" << 42 << "]\n";
    cout << '[' << left << setw(6) << 42 << "][" << setw(6) << "ab" << "]\n";
    cout << right << setfill('*') << '[' << setw(6) << 42 << "][" << setw(3) << 7 << "]\n" << setfill(' ');

    cout << hex << 255 << ' ' << 16 << ' ' << showbase << 255 << dec << ' ' << 255 << noshowbase << '\n';
    cout << oct << 8 << dec << ' ' << boolalpha << (1 > 2) << noboolalpha << ' ' << showpoint << 2.0 << noshowpoint << ' ' << 2.0 << '\n';

    istringstream in{"Frank 100 1234.5"};
    string name;
    int num;
    double total;
    in >> name >> num >> total;
    cout << "doc: " << name << " | " << num << " | " << total << '\n';

    istringstream bad{"12 abc 34"};
    int x = 0, y = 0, z = 0;
    bad >> x >> y >> z;
    cout << "\"12 abc 34\": x = " << x << ", y = " << y << ", z = " << z << ", fail = " << bad.fail() << '\n';
    bad.clear();
    string skip;
    bad >> skip >> z;
    cout << "sau clear(): skip = " << skip << ", z = " << z << '\n';

    ostringstream out;
    out << "Total: " << setw(5) << 42 << '!';
    cout << "ostringstream: [" << out.str() << "]\n";

    int value;
    string input = "123abc";
    istringstream check{input};
    if (check >> value && check.eof()) cout << input << " la so nguyen\n";
    else cout << input << " khong phai so nguyen (doc duoc " << value << ")\n";
    return 0;
}
