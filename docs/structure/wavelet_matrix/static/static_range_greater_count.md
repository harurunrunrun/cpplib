---
title: Static Range Greater Count (静的区間の閾値超過要素数) [KQUERYO]
documentation_of: ../../../../src/structure/wavelet_matrix/static/static_range_greater_count.hpp
---

静的な列に対し、半開区間内で指定した閾値より大きい要素の個数を数える。座標圧縮したウェーブレット行列（`CompressedWaveletMatrix`）を内部に持つため、値域の大きさに依存せず利用できる。

## テンプレート引数

```cpp
StaticRangeGreaterCount<T, MAX_SIZE = -1, ID_BIT_WIDTH = 32>
```

- `T`: 値の型。`<` による全順序で比較でき、`!=` を持つ必要がある。
- `MAX_SIZE`: 列長の上限。省略時（`-1`）は `std::vector` を使う可変長版になり、入力長に応じた領域だけを確保する。非負の値を指定すると固定長配列版になる。
- `ID_BIT_WIDTH`: 圧縮後の ID に使う bit 数。既定値は `numeric_limits<unsigned>::digits`。

```cpp
using Variable = StaticRangeGreaterCount<T>;
using Fixed = StaticRangeGreaterCount<T, N>;
```

## API

```cpp
explicit StaticRangeGreaterCount(const vector<T>& values)
int size() const
int count_greater(int left, int right, const T& threshold) const
```

- コンストラクタ: 列 `values` を前処理する。
- `size`: 列の長さを返す。
- `count_greater`: 0-indexed の半開区間 $[\mathit{left}, \mathit{right})$ に含まれる、`threshold` より大きい要素の個数を返す。空区間では 0 を返す。

## 時間計算量

$N$ を列長、$D$ を相異なる値の個数、$H = \texttt{ID_BIT_WIDTH}$ とし、$W$ を固定長版では $\lceil \texttt{MAX_SIZE} / 64 \rceil$、可変長版では $\lceil N / 64 \rceil$ とする。

- 構築: $O(N \log N + H(N + W))$（固定長版では配列の初期化に $O(\texttt{MAX_SIZE})$ が加わる）
- `size`: $O(1)$
- `count_greater`: $O(\log D + H)$

## 空間計算量

$O(N + H W)$ ワード（固定長版では作業配列に $O(\texttt{MAX_SIZE})$ が加わる）。

## 注意点

- $0 \le N \le \texttt{MAX_SIZE}$（固定長版）が必要である。
- 相異なる値の個数が $2^{\texttt{ID_BIT_WIDTH}}$ 以下である必要がある。
- $0 \le \mathit{left} \le \mathit{right} \le N$ に違反すると `std::runtime_error` を送出する。
- 列は構築後に変更できず、各クエリは互いに独立である。
