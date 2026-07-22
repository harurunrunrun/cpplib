#ifndef CPPLIB_SRC_STRUCTURE_MODINT_EXPONENTIABLE_MODINT_HPP_INCLUDED
#define CPPLIB_SRC_STRUCTURE_MODINT_EXPONENTIABLE_MODINT_HPP_INCLUDED

#include <cstdint>

constexpr std::uint32_t exponentiable_modint_totient(std::uint32_t x){
    std::uint32_t res = x;
    for(std::uint32_t p = 2; static_cast<std::uint64_t>(p) * p <= x; p++){
        if(x % p == 0){
            res /= p;
            res *= p - 1;
            do{
                x /= p;
            }while(x % p == 0);
        }
    }
    if(x != 1){
        res /= x;
        res *= x - 1;
    }
    return res;
}

template<std::uint32_t MOD>
struct ExponentiableModint{
    static_assert(1 <= MOD);
    static_assert(MOD < 0x80000000U);

    template<std::uint32_t>
    friend struct ExponentiableModint;

    using value_type = std::uint32_t;
    using next_type = ExponentiableModint<exponentiable_modint_totient(MOD)>;

    private:
        value_type _value;
        next_type _next;

        static constexpr std::uint64_t extended_mod(){
            return static_cast<std::uint64_t>(MOD) * 2;
        }

        static value_type normalize(std::uint64_t x){
            if(x < extended_mod()){
                return static_cast<value_type>(x);
            }
            return static_cast<value_type>(x % MOD + MOD);
        }

        static value_type mul_raw(value_type a, value_type b){
            return normalize(static_cast<std::uint64_t>(a) * b);
        }

        static value_type pow_raw(value_type a, value_type n){
            value_type res = 1;
            while(n > 0){
                if(n & 1){
                    res = mul_raw(res, a);
                }
                a = mul_raw(a, a);
                n >>= 1;
            }
            return res;
        }

    public:
        static constexpr value_type get_mod(){
            return MOD;
        }

        ExponentiableModint() : _value(0), _next(0) {}

        ExponentiableModint(std::uint64_t x) : _value(normalize(x)), _next(x) {}

        ExponentiableModint(value_type value, next_type next) : _value(value), _next(next) {}

        value_type val()const{
            if(_value < MOD){
                return _value;
            }
            return _value - MOD;
        }

        ExponentiableModint pow(const ExponentiableModint& rhs)const{
            return ExponentiableModint(pow_raw(_value, rhs._next._value), _next.pow(rhs._next));
        }

        ExponentiableModint& operator+=(const ExponentiableModint& rhs){
            std::uint64_t value = static_cast<std::uint64_t>(_value) + rhs._value;
            if(value >= extended_mod()){
                value -= MOD;
            }
            if(value >= extended_mod()){
                value -= MOD;
            }
            _value = static_cast<value_type>(value);
            _next += rhs._next;
            return *this;
        }

        ExponentiableModint& operator*=(const ExponentiableModint& rhs){
            _value = mul_raw(_value, rhs._value);
            _next *= rhs._next;
            return *this;
        }

        friend ExponentiableModint operator+(ExponentiableModint lhs, const ExponentiableModint& rhs){
            return lhs += rhs;
        }

        friend ExponentiableModint operator*(ExponentiableModint lhs, const ExponentiableModint& rhs){
            return lhs *= rhs;
        }

        friend bool operator==(const ExponentiableModint& lhs, const ExponentiableModint& rhs){
            return lhs.val() == rhs.val();
        }

        friend bool operator!=(const ExponentiableModint& lhs, const ExponentiableModint& rhs){
            return !(lhs == rhs);
        }
};

template<>
struct ExponentiableModint<1>{
    template<std::uint32_t>
    friend struct ExponentiableModint;

    using value_type = std::uint32_t;

    private:
        value_type _value;

    public:
        static constexpr value_type get_mod(){
            return 1;
        }

        ExponentiableModint() : _value(0) {}

        ExponentiableModint(std::uint64_t x) : _value(x != 0) {}

        value_type val()const{
            return 0;
        }

        ExponentiableModint pow(const ExponentiableModint& rhs)const{
            return ExponentiableModint(_value || !rhs._value);
        }

        ExponentiableModint& operator+=(const ExponentiableModint& rhs){
            _value = _value || rhs._value;
            return *this;
        }

        ExponentiableModint& operator*=(const ExponentiableModint& rhs){
            _value = _value && rhs._value;
            return *this;
        }

        friend ExponentiableModint operator+(ExponentiableModint lhs, const ExponentiableModint& rhs){
            return lhs += rhs;
        }

        friend ExponentiableModint operator*(ExponentiableModint lhs, const ExponentiableModint& rhs){
            return lhs *= rhs;
        }

        friend bool operator==(const ExponentiableModint&, const ExponentiableModint&){
            return true;
        }

        friend bool operator!=(const ExponentiableModint& lhs, const ExponentiableModint& rhs){
            return !(lhs == rhs);
        }
};

#endif  // CPPLIB_SRC_STRUCTURE_MODINT_EXPONENTIABLE_MODINT_HPP_INCLUDED
