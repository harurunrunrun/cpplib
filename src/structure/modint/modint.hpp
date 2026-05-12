

template<int mod>
struct Modint{
    static_assert(mod >= 1);
    private:
        long long _val;

        static constexpr bool is_prime_constexpr(int n){
            if (n <= 1) return false;
            if (n == 2) return true;
            if (n % 2 == 0) return false;

            for (long long i = 3; i * i <= n; i += 2) {
                if (n % i == 0) return false;
            }
            return true;
        }

        static long long inv_gcd(long long a, long long m){
            a %= m;
            if (a < 0) a += m;
            long long b = m;
            long long u = 1, v = 0;
            while (b) {
                long long t = a / b;
                a -= t * b;
                std::swap(a, b);
                u -= t * v;
                std::swap(u, v);
            }
            if(a != 1)[[unlikely]]{
                throw std::runtime_error("library assertion fault: gcd is not 1 (inv).");
            }
            u %= m;
            if (u < 0) u += m;
            return u;
        }

    public:

        Modint(long long v = 0){
            _val = v % mod;
            if(_val < 0){
                _val += mod;
            }
        }

        Modint pow(long long n)const{
            if(n < 0) [[unlikely]] {
                throw std::runtime_error("library assertion fault: negative exponent (pow).");
            }
            Modint res(1);
            Modint a(_val);
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
            if constexpr (is_prime_constexpr(mod)){
                if(_val == 0)[[unlikely]]{
                    throw std::runtime_error("library assertion fault: existence violation (inv).");
                }
                return pow(mod - 2);
            }else{
                return Modint(inv_gcd(_val, mod));
            }
        }


        Modint& operator+=(const Modint& rhs){
            _val += rhs._val;
            if(_val >= mod){
                _val -= mod;
            }
            return *this;
        }

        Modint& operator-=(const Modint& rhs){
            _val -= rhs._val;
            if(_val < 0){
                _val += mod;
            }
            return *this;
        }

        Modint& operator*=(const Modint& rhs){
            _val = _val * rhs._val % mod;
            return *this;
        }

        Modint& operator/=(const Modint& rhs){
            return *this *= rhs.inv();
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

        int val()const{
            return _val;
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
