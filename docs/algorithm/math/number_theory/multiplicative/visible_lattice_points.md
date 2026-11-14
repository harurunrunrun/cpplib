---
title: Visible Lattice Points (可視格子点数) [VLATTICE]
documentation_of: ../../../../../src/algorithm/math/number_theory/multiplicative/visible_lattice_points.hpp
---

3 次元の立方格子 $\lbrace 0, \ldots, n \rbrace^3$ のうち、原点から途中に別の格子点を挟まずに見える点（原点以外）の個数を求めるための前計算表。

## `VisibleLatticePointsTable`

```cpp
explicit math::VisibleLatticePointsTable(int limit);
```

$1$ から `limit` までのメビウス関数の累積和（メルテンス関数）を線形篩で構築する。

## `limit`

```cpp
int limit() const;
```

表の上限を返す。

## `value`

```cpp
std::uint64_t value(int n) const;
```

各座標が $0$ 以上 $n$ 以下の 3 次元格子点のうち、原点以外で原点から見える点の個数を返す。座標軸上の $3$ 点、座標平面上の互いに素な組 $3 \sum_d \mu(d) \lfloor n/d \rfloor^2$、内部の互いに素な 3 つ組 $\sum_d \mu(d) \lfloor n/d \rfloor^3$ の和として、商ごとにまとめて計算する。

## 時間計算量・空間計算量

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| コンストラクタ | $O(L)$（$L$ は `limit`） | $O(L)$ |
| `limit` | $O(1)$ | $O(1)$ |
| `value` | $O(\sqrt n)$ | $O(1)$ |

## 注意点

- `limit >= 1` が必要で、`limit <= 0`（負の値を含む）では表を確保する前に `std::invalid_argument` を送出する。
- `value` は $1 \le n \le \mathrm{limit}$ が必要で、範囲外では `std::out_of_range` を送出する。
