---
title: Square Root Modulo a Prime (素数を法とする平方根) [sqrt_mod]
documentation_of: ../../../../../src/algorithm/math/number_theory/modular/sqrt_mod.hpp
---

素数を法とする平方根を Tonelli–Shanks 法で求める。

## `sqrt_mod`

```cpp
std::optional<std::uint64_t> math::sqrt_mod(std::uint64_t value, std::uint64_t prime);
```

$x^2 \equiv \mathrm{value} \pmod{\mathrm{prime}}$ を満たす $x$ が存在すれば、2 つの平方根のうち小さい方を返す。存在しなければ `std::nullopt` を返す。`value == 0` と `prime == 2` も扱う。$\mathrm{prime} \equiv 3 \pmod 4$ のときは $\mathrm{value}^{(\mathrm{prime}+1)/4}$ で直接求める。

`prime` は素数で $2 \le \mathrm{prime}$、かつ $\mathrm{value} < \mathrm{prime}$ でなければならない。範囲の条件を満たさない場合は `std::invalid_argument` を送出する。素数判定は行わない。

## 時間計算量・空間計算量

$\mathrm{prime} - 1 = q \cdot 2^s$（$q$ は奇数）とし、$Z$ を $2$ から昇順に探して最初の平方非剰余が見つかるまでに調べる候補の個数とする。

| 操作 | 時間計算量 | 空間計算量（追加領域） |
| --- | --- | --- |
| 剰余乗算 | $O(1)$ | $O(1)$ |
| 剰余累乗 | $O(\log \mathrm{prime})$ | $O(1)$ |
| `sqrt_mod` | $O(s^2 + Z \log \mathrm{prime})$ | $O(1)$ |

## 注意点

- 乗算には 128 bit 整数を使うので、64 bit の法でもオーバーフローしない。
- 平方非剰余は $2$ から決定的に昇順で探すので、結果は決定的である。
- `prime` が素数でない場合の動作は未定義である（平方非剰余の探索が終わらない可能性がある）。
