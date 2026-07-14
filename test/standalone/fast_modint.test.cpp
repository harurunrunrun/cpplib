// competitive-verifier: STANDALONE

#include <cassert>
#include <iostream>
#include <random>
#include <sstream>
#include <stdexcept>
#include "../../src/structure/modint/fast_modint.hpp"

using mint = FastModint998244353;

long long mod_pow(long long a, long long n, long long mod){
    long long result = 1;
    while(n){
        if(n & 1) result = result * a % mod;
        a = a * a % mod;
        n >>= 1;
    }
    return result;
}

void self_test(){
    assert(mint::get_mod() == 998244353);
    assert(mint().val() == 0);
    assert(mint(-1).val() == 998244352);
    assert(mint::raw(17).val() == 17);
    mint compound = 10;
    compound += 5;
    compound -= 3;
    compound *= 4;
    compound /= 6;
    assert(compound == 8);
    assert(+compound == compound);
    assert((-compound).val() == 998244345);
    assert(compound != mint(9));
    assert((++compound).val() == 9);
    assert((compound++).val() == 9);
    assert((--compound).val() == 9);
    assert((compound--).val() == 9);
    assert(compound.val() == 8);

    std::stringstream stream("-3");
    mint parsed;
    stream >> parsed;
    assert(parsed.val() == 998244350);
    std::stringstream output;
    output << parsed;
    assert(output.str() == "998244350");

    bool thrown = false;
    try{ (void)mint(2).pow(-1); }
    catch(const std::runtime_error&){ thrown = true; }
    assert(thrown);
    thrown = false;
    try{ (void)mint(0).inv(); }
    catch(const std::runtime_error&){ thrown = true; }
    assert(thrown);
    thrown = false;
    try{ (void)FastModint<12>(6).inv(); }
    catch(const std::runtime_error&){ thrown = true; }
    assert(thrown);

    assert((mint(2) + mint(3)).val() == 5);
    assert((mint(2) - mint(3)).val() == 998244352);
    assert((mint(123456789) * mint(987654321)).val() == 263684735);
    assert((mint(5) / mint(3) * mint(3)).val() == 5);
    assert(mint(2).pow(10).val() == 1024);
    std::mt19937 rng(20260917);
    for(int step = 0; step < 10000; step++){
        long long a = static_cast<long long>(rng());
        long long b = static_cast<long long>(rng());
        mint x = a;
        mint y = b;
        assert((x + y).val() == (a + b) % 998244353);
        assert((x - y).val() == (a - b % 998244353 + 998244353) % 998244353);
        assert((x * y).val() == (a % 998244353) * (b % 998244353) % 998244353);
        int e = static_cast<int>(rng() % 1000);
        assert(x.pow(e).val() == mod_pow(a % 998244353, e, 998244353));
        if(a % 998244353 != 0){
            assert((x * x.inv()).val() == 1);
        }
    }
}

int main(){
    long long a, b, e;
    if(!(std::cin >> a >> b >> e)){
        self_test();
        return 0;
    }
    mint x = a;
    mint y = b;
    std::cout << (x + y).val() << ' ' << (x - y).val() << ' ' << (x * y).val() << ' ' << x.pow(e).val() << '\n';
}
