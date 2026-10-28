/*
 * Bài: Exception: throw kiểu gì cũng được, thứ tự catch, catch(...), rethrow, lớp exception tự định nghĩa
 * Chủ đề: C++ (Udemy section 18)
 * Dự đoán trước khi chạy: Mỗi lần gọi test(k) được catch bởi khối nào? Dòng "sau test(k)" in ra với những k nào? Bắt theo giá trị (catch(std::exception e)) thì what() trả về gì? stoi("abc") và v.at(10) ném kiểu gì?
 * Ý chính: catch được thử theo thứ tự viết, khớp cái đầu tiên (không phải cái "khớp nhất") → lớp con phải đặt trước lớp cha. catch(...) bắt mọi thứ. throw; (không có biểu thức) ném lại exception đang xử lý. Luôn bắt bằng const reference để tránh slicing. Chia số nguyên cho 0 KHÔNG phải exception (là UB).
 * Cảnh báo biên dịch cố ý: -Wcatch-value ở catch (exception e) (bắt theo giá trị → slicing).
 * Thử thêm: Đổi thứ tự catch (const std::exception&) lên trước catch (const InsufficientFunds&) và đọc cảnh báo của gcc. Ném exception từ một hàm noexcept.
 */
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>
using namespace std;

class InsufficientFunds : public runtime_error {
public:
    double need;
    InsufficientFunds(double n) : runtime_error{"Insufficient funds"}, need{n} {}
};

void test(int k)
{
    if (k == 0) throw 42;
    if (k == 1) throw string{"loi dang string"};
    if (k == 2) throw InsufficientFunds{50};
    if (k == 3) throw invalid_argument{"tham so sai"};
    if (k == 4) throw 3.14;
    cout << "  k = " << k << ": khong throw\n";
}

void rethrow()
{
    try {
        throw logic_error{"tu rethrow"};
    } catch (const exception &e) {
        cout << "  rethrow(): bat duoc \"" << e.what() << "\", nem lai\n";
        throw;
    }
}

int main()
{
    for (int k = 0; k <= 5; ++k) {
        try {
            test(k);
            cout << "  sau test(" << k << ")\n";
        } catch (int e) {
            cout << "  catch int " << e << '\n';
        } catch (const string &e) {
            cout << "  catch string: " << e << '\n';
        } catch (const InsufficientFunds &e) {
            cout << "  catch InsufficientFunds: " << e.what() << ", need = " << e.need << '\n';
        } catch (const exception &e) {
            cout << "  catch exception: " << e.what() << '\n';
        } catch (...) {
            cout << "  catch (...)\n";
        }
    }

    try {
        rethrow();
    } catch (const logic_error &e) {
        cout << "  main bat lai: " << e.what() << '\n';
    }

    try {
        throw InsufficientFunds{10};
    } catch (exception e) {   // bắt theo giá trị → slicing
        cout << "  catch theo gia tri: what() = " << e.what() << '\n';
    }

    try {
        stoi("abc");
    } catch (const invalid_argument &e) {
        cout << "  stoi(\"abc\") -> invalid_argument\n";
    }
    try {
        vector<int> v(3);
        v.at(10) = 1;
    } catch (const out_of_range &e) {
        cout << "  v.at(10) -> out_of_range\n";
    }
    return 0;
}
