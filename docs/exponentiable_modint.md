---
title: Exponent-Tower Modint (累乗塔対応剰余整数)
documentation_of: ../src/structure/modint/exponentiable_modint.hpp
---

指数にも使える modint。

`mod P`, `mod phi(P)`, `mod phi(phi(P))`, ... の値を持つ。`P` 以上の値は `mod P` に落としたあと `P` を足して持つため、指数が小さいか周期に入っているかを区別できる。

## 使い方

```cpp
using mint = ExponentiableModint<998244353>;

mint n = 4;
mint k = 3;
mint m = 2;

auto ans = m.pow(k.pow(n)).val(); // 2^(3^4) mod 998244353
```

## 補助関数

```cpp
constexpr uint32_t exponentiable_modint_totient(uint32_t x)
```

`x >= 1` に対するEulerのtotientを試し割りで求める。
runtimeで呼ぶ場合は $O(\sqrt{x})$、追加空間計算量は $O(1)$。
`next_type` の形成に使う場合はcompile timeに評価される。

## メンバ関数

- `ExponentiableModint(uint64_t x)`
    - `x` から構築する。
- `ExponentiableModint()`
    - `0` として構築する。
- `static constexpr uint32_t get_mod()`
    - mod を返す。
- `ExponentiableModint(value_type value, next_type next)`
    - 現段と次段の内部表現を直接指定する。`MOD > 1` の一般templateに存在する。
- `using value_type = uint32_t`
- `using next_type = ExponentiableModint<phi(MOD)>`
    - `next_type` は `MOD > 1` の一般templateに存在する。
- `uint32_t val() const`
    - `mod P` での値を返す。
- `ExponentiableModint pow(const ExponentiableModint& rhs) const`
    - `this^rhs` を返す。

## 演算

- `+`, `+=`
- `*`, `*=`
- `==`, `!=`

引き算はない。値が 0 か、正か、十分大きいかの情報を保つため。

## 時間計算量

`P_0=P`, `P_{i+1}=phi(P_i)` とし、`P_L=1` になるまでの段数を
`L` とする。

| 操作 | 時間計算量 |
| --- | --- |
| デフォルト構築、`ExponentiableModint(x)`、2引数構築 | $O(L+1)$ |
| コピー・move構築、コピー・move代入 | $O(L+1)$ |
| `get_mod()` | $O(1)$ |
| `val()` | $O(1)$ |
| `+=`, `+` | $O(L+1)$ |
| `*=`, `*` | $O(L+1)$ |
| `==`, `!=` | $O(1)$ |
| `pow(rhs)` | $O(1+\sum_{i=0}^{L-1}\log(P_i+1))$ |

`L=O(\log P)` なので、`pow` は $O(L\log P)=O(\log^2 P)$ とも書ける。

`MOD=1` specializationでは、すべてのconstructor・operator・methodが $O(1)$。

## 空間計算量（API別の追加領域）

get_mod、val、比較は O(1)。一般templateのconstructor、copy/move、加算、乗算、powはtotient chainの再帰stackと返却objectを含め O(L)。MOD=1 specializationの全APIは O(1)。

## 注意点

- template引数は`MOD >= 1`。`val()`は現在段を`MOD`で割った代表値を返す。
- 整数constructorは各totient段へ同じ非負整数を伝播し、周期到達情報も保持する。
- 2引数constructorは内部表現を直接指定する低level APIで、整合性は呼出側が保証する。
- `pow(rhs)`はEulerのtotient列を使い、指数が周期へ達したかを区別して剰余累乗する。
- 算術結果が内部の`uint64_t`乗算と正規化で表せる設計で、例外を送出するAPIはない。
