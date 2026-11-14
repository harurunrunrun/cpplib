---
title: Discrete Root Modulo a Prime (素数法の離散乗根)
documentation_of: ../../../../../src/algorithm/math/number_theory/modular/discrete_root_mod_prime.hpp
---

素数 $p$ に対して $x^k \equiv a \pmod p$ を満たす $x$ を 1 つ求める。

## API

```cpp
std::optional<math::u64> math::discrete_root_mod_prime(
    math::u64 value,
    math::u64 exponent,
    math::u64 prime
);
```

`value` $= a$、`exponent` $= k$、`prime` $= p$ とし、解が存在すれば $[0, p)$ における最小の解を、存在しなければ `std::nullopt` を返す。$a \equiv 0$ なら $0$ を返す。

## アルゴリズム

最小の原始根 $g$ を求め、baby-step giant-step で $a = g^L$ となる $L$ を求めた後、指数に関する一次合同式 $kX \equiv L \pmod{p-1}$ を解く。解は $d = \gcd(k, p-1)$ 個あり、その中の最小値を返す。全解列挙 API には依存しない独立した実装である。

## 時間計算量

$d = \gcd(k, p-1)$ とする。

- 原始根の計算: $p-1$ の Pollard–Rho 素因数分解と候補の検査（`modular_arithmetic.hpp` の `primitive_root` を参照）
- 原始根を底とする離散対数: 期待 $O(\sqrt p)$ 時間、$O(\sqrt p)$ 空間
- 指数の一次合同式: $O(\log p)$ 時間、$O(1)$ 空間
- $d$ 個の候補から最小解を選ぶ処理: 1 の原始 $d$ 乗根 $g^{(p-1)/d}$ を順に掛けて候補を列挙するため $O(d + \log p)$ 時間、$O(1)$ 空間
- 全体: 期待 $O(\sqrt p + d + \log p)$ 時間（原始根の計算を除く）、$O(\sqrt p)$ 空間

## 注意点

- `prime` は素数（Miller–Rabin で判定）、`exponent` は正でなければならず、違反時は `std::runtime_error` を送出する。
- `value` は関数内で `prime` を法として正規化する。
