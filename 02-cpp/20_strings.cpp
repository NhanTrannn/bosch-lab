/*
 * Bài: C-style string và std::string
 * Chủ đề: C++ cơ bản (Udemy section 10)
 * Dự đoán trước khi chạy: sizeof(s) và strlen(s) của char s[] = "Hello"? s1 == s2 (mảng char) so sánh gì? substr, find, npos? "Zebra" < "apple"? stoi("42abc")? cin >> đọc "Frank Smith" được gì?
 * Ý chính: C-string là mảng char kết thúc bằng '\0' (sizeof đếm cả '\0', strlen thì không). == trên mảng char so sánh ĐỊA CHỈ; phải dùng strcmp. std::string so sánh nội dung, theo thứ tự từ điển (mã ASCII: chữ hoa < chữ thường). >> dừng ở khoảng trắng, getline đọc cả dòng.
 * Thử thêm: Viết string d = "Hello" + " World"; để xem lỗi (hai const char* không cộng được). Gọi e[10] thay vì e.at(10).
 */
#include <cctype>
#include <cstring>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
using namespace std;

int main()
{
    char s1[] = "Hello";
    char s2[] = "Hello";
    cout << "sizeof(s1) = " << sizeof(s1) << ", strlen(s1) = " << strlen(s1) << '\n';
    cout << "s1 == s2 ? " << (static_cast<char *>(s1) == static_cast<char *>(s2)) << ", strcmp(s1, s2) == 0 ? " << (strcmp(s1, s2) == 0) << '\n';

    char buf[20];
    strcpy(buf, "Frank");
    strcat(buf, " Smith");
    cout << "buf = " << buf << ", strlen = " << strlen(buf) << '\n';

    string a = "Hello", b = "Hello";
    cout << "string a == b ? " << (a == b) << '\n';
    string c = a + " " + "World";
    cout << "c = " << c << ", substr(6, 3) = " << c.substr(6, 3) << ", find(\"lo\") = " << c.find("lo")
         << ", find(\"xyz\") == npos ? " << (c.find("xyz") == string::npos) << '\n';

    string e = "abc";
    e += 'd';
    cout << "e = " << e << ", size = " << e.size() << ", length = " << e.length() << '\n';
    for (char &ch : e) ch = toupper(ch);
    cout << "toupper: " << e << '\n';
    e.insert(0, ">>");
    e.erase(2, 1);
    cout << "insert/erase: " << e << '\n';

    cout << "string(3, '*') = " << string(3, '*') << '\n';
    cout << "\"apple\" < \"banana\" ? " << (string("apple") < "banana") << ", \"Zebra\" < \"apple\" ? " << (string("Zebra") < "apple") << '\n';

    try {
        cout << e.at(10) << '\n';
    } catch (const out_of_range &ex) {
        cout << "e.at(10) -> out_of_range\n";
    }

    cout << "stoi(\"42abc\") = " << stoi("42abc") << ", to_string(3.5) = " << to_string(3.5) << '\n';
    cout << "isdigit('7') ? " << bool(isdigit('7')) << ", isalpha('7') ? " << bool(isalpha('7')) << ", isspace(' ') ? " << bool(isspace(' ')) << '\n';

    istringstream in("Frank Smith\nTom Jones\n");
    string first, rest, line;
    in >> first;
    getline(in, rest);
    getline(in, line);
    cout << "in >> first = [" << first << "], getline rest = [" << rest << "], line = [" << line << "]\n";
    return 0;
}
