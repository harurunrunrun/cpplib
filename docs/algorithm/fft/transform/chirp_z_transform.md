---
title: Chirp Z-Transform (チャープZ変換)
documentation_of: ../../../../src/algorithm/fft/transform/chirp_z_transform.hpp
---

多項式 $f$ を等比数列上の点 $z, zr, zr^2, \ldots$ で一括評価する。

## API

```cpp
template<int MOD>
std::vector<Modint<MOD>> chirp_z_transform(
    const std::vector<Modint<MOD>>& coefficients,
    Modint<MOD> initial_point,
    Modint<MOD> ratio,
    std::size_t count
)
```

`coefficients` を $f$ の係数列（昇冪順）とし、$k = 0, \ldots, \texttt{count} - 1$ に対する $f(z r^k)$ を返す。ここで $z$ は `initial_point`、$r$ は `ratio` である。

## 計算量

$n$ を `coefficients` の長さ、$m = \texttt{count}$、長さ $n + m$ 程度の多項式乗算（middle product）の時間を $M(n + m)$ とすると、$O(M(n + m) + n + m)$ 時間、$O(n + m)$ の追加領域を使う。

## 注意点

- `ratio` が $0$ でないときは逆元を用いるため、`ratio` が可逆でなければならない。
- `ratio` が $0$ の場合は別に処理し、先頭は $f(z)$、以降はすべて $f(0)$ を返す。
- `count == 0` なら空列を、`coefficients` が空なら長さ `count` の零列を返す。
