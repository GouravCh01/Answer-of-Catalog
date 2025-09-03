#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <cctype>

using namespace std;

struct BigInt {
    string digits;
    bool negative;

    BigInt(string s = "0") {
        negative = false;
        if (s.empty()) {
            digits = "0";
            return;
        }
        if (s[0] == '-') {
            negative = true;
            s = s.substr(1);
        }
        int i = 0;
        while (i + 1 < (int)s.size() && s[i] == '0') i++;
        digits = s.substr(i);
        reverse(digits.begin(), digits.end());
        if (digits == "0") negative = false;
    }

    void removeLeadingZeros() {
        while (digits.size() > 1 && digits.back() == '0')
            digits.pop_back();
        if (digits.size() == 1 && digits[0] == '0')
            negative = false;
    }

    static BigInt addUnsigned(const BigInt& a, const BigInt& b) {
        BigInt result;
        result.digits.clear();
        int carry = 0;
        size_t n = max(a.digits.size(), b.digits.size());
        for (size_t i = 0; i < n; i++) {
            int d1 = i < a.digits.size() ? a.digits[i] - '0' : 0;
            int d2 = i < b.digits.size() ? b.digits[i] - '0' : 0;
            int sum = d1 + d2 + carry;
            carry = sum / 10;
            result.digits.push_back((sum % 10) + '0');
        }
        if (carry) result.digits.push_back(carry + '0');
        return result;
    }

    static int absCompare(const BigInt& a, const BigInt& b) {
        if (a.digits.size() != b.digits.size())
            return (a.digits.size() > b.digits.size()) ? 1 : -1;
        for (int i = (int)a.digits.size() - 1; i >= 0; i--) {
            if (a.digits[i] != b.digits[i])
                return (a.digits[i] > b.digits[i]) ? 1 : -1;
        }
        return 0;
    }

    static BigInt subtractUnsigned(const BigInt& a, const BigInt& b) {
        BigInt result;
        result.digits.clear();
        int borrow = 0;
        for (size_t i = 0; i < a.digits.size(); i++) {
            int d1 = a.digits[i] - '0';
            int d2 = i < b.digits.size() ? b.digits[i] - '0' : 0;
            int diff = d1 - d2 - borrow;
            if (diff < 0) {
                diff += 10;
                borrow = 1;
            } else borrow = 0;
            result.digits.push_back(diff + '0');
        }
        result.removeLeadingZeros();
        return result;
    }

    BigInt operator+(const BigInt& other) const {
        if (!negative && !other.negative)
            return addUnsigned(*this, other);
        if (negative && other.negative) {
            BigInt res = addUnsigned(*this, other);
            res.negative = true;
            return res;
        }
        int cmp = absCompare(*this, other);
        if (!negative && other.negative) {
            if (cmp >= 0) {
                BigInt res = subtractUnsigned(*this, other);
                res.negative = false;
                return res;
            } else {
                BigInt res = subtractUnsigned(other, *this);
                res.negative = true;
                return res;
            }
        }
        if (negative && !other.negative) {
            if (cmp > 0) {
                BigInt res = subtractUnsigned(*this, other);
                res.negative = true;
                return res;
            } else {
                BigInt res = subtractUnsigned(other, *this);
                res.negative = false;
                return res;
            }
        }
        return BigInt("0");
    }

    BigInt operator-(const BigInt& other) const {
        BigInt negOther = other;
        negOther.negative = !other.negative;
        return *this + negOther;
    }

    BigInt multiplyDigit(int digit) const {
        if (digit == 0) return BigInt("0");
        BigInt result;
        result.digits.clear();
        int carry = 0;
        for (char d : digits) {
            int prod = (d - '0') * digit + carry;
            carry = prod / 10;
            result.digits.push_back((prod % 10) + '0');
        }
        if (carry) result.digits.push_back(carry + '0');
        result.negative = negative && digit != 0 ? false : false;
        return result;
    }

    BigInt operator*(int digit) const {
        return multiplyDigit(digit);
    }

    BigInt operator*(const BigInt& other) const {
        BigInt result("0");
        for (size_t i = 0; i < other.digits.size(); i++) {
            BigInt partial = multiplyDigit(other.digits[i] - '0');
            partial.digits.insert(partial.digits.begin(), i, '0');
            result = result + partial;
        }
        result.negative = (negative != other.negative);
        result.removeLeadingZeros();
        return result;
    }

    void mulBaseAdd(int base, int digit) {
        *this = *this * base + BigInt(to_string(digit));
    }

    void print() const {
        if (negative && digits != "0") cout << '-';
        for (auto it = digits.rbegin(); it != digits.rend(); it++)
            cout << *it;
    }
};

BigInt convertToDecimal(const string& value, int base) {
    BigInt num("0");
    for (char c : value) {
        int digit;
        if (isdigit(c)) digit = c - '0';
        else digit = 10 + (tolower(c) - 'a');
        num.mulBaseAdd(base, digit);
    }
    return num;
}

vector<BigInt> multiplyPoly(const vector<BigInt>& poly, const BigInt& root) {
    vector<BigInt> result(poly.size() + 1, BigInt("0"));
    for (size_t i = 0; i < poly.size(); i++) {
        result[i] = result[i] + poly[i];
        result[i + 1] = result[i + 1] - (poly[i] * root);
    }
    return result;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int n, k;
    cin >> n >> k;

    vector<pair<int, string>> roots;
    for (int i = 0; i < n; i++) {
        int b; string val;
        cin >> b >> val;
        roots.emplace_back(b, val);
    }

    vector<BigInt> rootValues;
    for (int i = 0; i < k; i++) {
        rootValues.push_back(convertToDecimal(roots[i].second, roots[i].first));
    }

    vector<BigInt> poly = {BigInt("1")};

    for (auto& r : rootValues) {
        poly = multiplyPoly(poly, r);
    }

    cout << "Polynomial coefficients: ";
    for (auto& c : poly) {
        c.print();
        cout << " ";
    }
    cout << "\n";
    return 0;
}