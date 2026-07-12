#pragma once

#include <cstdint>
#include <istream>
#include <ostream>
#include <stdexcept>
#include <utility>

template<int mod>
struct Modint{
    static_assert(mod >= 1);

    private:
        using u32 = std::uint32_t;
        using u64 = std::uint64_t;

        u32 _val;

        static constexpr u32 umod(){
            return static_cast<u32>(mod);
        }

        static constexpr bool is_prime_constexpr(int n){
            if(n <= 1) return false;
            if(n == 2) return true;
            if(n % 2 == 0) return false;
            for(long long i = 3; i * i <= n; i += 2){
                if(n % i == 0) return false;
            }
            return true;
        }

        static long long inv_gcd(long long a, long long m){
            a %= m;
            if(a < 0) a += m;
            long long b = m;
            long long u = 1, v = 0;
            while(b){
                long long t = a / b;
                a -= t * b;
                std::swap(a, b);
                u -= t * v;
                std::swap(u, v);
            }
            if(a != 1) [[unlikely]] {
                throw std::runtime_error("library assertion fault: gcd is not 1 (inv).");
            }
            u %= m;
            if(u < 0) u += m;
            return u;
        }

    public:
        static constexpr int get_mod(){
            return mod;
        }

        Modint() : _val(0) {}

        Modint(long long v){
            long long x = v % mod;
            if(x < 0){
                x += mod;
            }
            _val = static_cast<u32>(x);
        }

        static Modint raw(int v){
            Modint x;
            x._val = static_cast<u32>(v);
            return x;
        }

        Modint pow(long long n)const{
            if(n < 0) [[unlikely]] {
                throw std::runtime_error("library assertion fault: negative exponent (pow).");
            }
            Modint res(1);
            Modint a = *this;
            while(n > 0){
                if(n & 1){
                    res *= a;
                }
                a *= a;
                n >>= 1;
            }
            return res;
        }

        Modint inv()const{
            if constexpr(is_prime_constexpr(mod)){
                if(_val == 0) [[unlikely]] {
                    throw std::runtime_error("library assertion fault: existence violation (inv).");
                }
                return pow(mod - 2);
            }else{
                return Modint(inv_gcd(_val, mod));
            }
        }

        Modint& operator+=(const Modint& rhs){
            _val += rhs._val;
            if(_val >= umod()){
                _val -= umod();
            }
            return *this;
        }

        Modint& operator-=(const Modint& rhs){
            if(_val < rhs._val){
                _val += umod();
            }
            _val -= rhs._val;
            return *this;
        }

        Modint& operator*=(const Modint& rhs){
            _val = static_cast<u32>(static_cast<u64>(_val) * rhs._val % umod());
            return *this;
        }

        Modint& operator/=(const Modint& rhs){
            return *this *= rhs.inv();
        }

        Modint operator+()const{
            return *this;
        }

        Modint operator-()const{
            return Modint(0) - *this;
        }

        Modint& operator++(){
            _val++;
            if(_val == umod()){
                _val = 0;
            }
            return *this;
        }

        Modint& operator--(){
            if(_val == 0){
                _val = umod();
            }
            _val--;
            return *this;
        }

        Modint operator++(int){
            Modint res = *this;
            ++*this;
            return res;
        }

        Modint operator--(int){
            Modint res = *this;
            --*this;
            return res;
        }

        friend Modint operator+(Modint lhs, const Modint& rhs){
            return lhs += rhs;
        }

        friend Modint operator-(Modint lhs, const Modint& rhs){
            return lhs -= rhs;
        }

        friend Modint operator*(Modint lhs, const Modint& rhs){
            return lhs *= rhs;
        }

        friend Modint operator/(Modint lhs, const Modint& rhs){
            return lhs /= rhs;
        }

        friend bool operator==(const Modint& lhs, const Modint& rhs){
            return lhs._val == rhs._val;
        }

        friend bool operator!=(const Modint& lhs, const Modint& rhs){
            return !(lhs == rhs);
        }

        int val()const{
            return static_cast<int>(_val);
        }

        friend std::istream& operator>>(std::istream& is, Modint& x){
            long long v;
            if(is >> v){
                x = Modint(v);
            }else{
                throw std::runtime_error("library assertion fault: input fails (istream).");
            }
            return is;
        }

        friend std::ostream& operator<<(std::ostream& os, const Modint& x){
            os << x.val();
            return os;
        }
};

using Modint998244353 = Modint<998244353>;
