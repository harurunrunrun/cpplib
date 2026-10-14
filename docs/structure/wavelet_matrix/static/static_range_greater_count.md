---
title: Static Range Greater Count (静的区間の閾値超過要素数) [KQUERYO]
documentation_of: ../../../../src/structure/wavelet_matrix/static/static_range_greater_count.hpp
---

静的な列に対し、半開区間内で指定した閾値より大きい要素の個数を数える。
座標圧縮したwavelet matrixを内部に持つため、値域の大きさに依存せず利用できる。

## API

### `StaticRangeGreaterCount<T, MAX_SIZE, ID_BIT_WIDTH>(values)`

列 `values` を前処理する。
`ID_BIT_WIDTH` の既定値は `numeric_limits<unsigned>::digits`。

### `size()`

列の長さを返す。

### `count_greater(left, right, threshold)`

0-indexed半開区間 `[left, right)` に含まれる
`threshold` より大きい要素の個数を返す。
空区間では0を返す。この関数は状態を変更しない。

## 時間計算量

`N = values.size()`、異なる値の数を `D`、
`H = ID_BIT_WIDTH` とする。

- 構築: $O(N \log N + H(N + \lceil MAX\_SIZE / 64 \rceil))$
- `size`: $O(1)$
- `count_greater`: $O(\log D + H)$

使用メモリは $O(N + H \cdot MAX\_SIZE)$。

## 注意点

- `0 <= N <= MAX_SIZE` が必要。
- `T` は全順序で比較できる型でなければならない。
- 座標圧縮後のIDが `ID_BIT_WIDTH` bitに収まる必要がある。
- `0 <= left <= right <= N` に違反すると `runtime_error` を送出する。
- 列は構築後に変更できず、各クエリは互いに独立である。

## 容量を省略した可変長版

容量のテンプレート引数 `MAX_SIZE` は省略でき、既定値 `-1` ではvectorを使う。
容量を指定した既存の固定配列版も同じヘッダで利用できる。

```cpp
using Variable = StaticRangeGreaterCount<T>;
using Fixed = StaticRangeGreaterCount<T, N>;
```

可変長版は入力長 $n$ とビット幅 $B$ に応じた配列だけを確保し、構築・照会の計算量は上記と同じ。
長さに依存する固定容量の領域は $O(n)$ に、ビット段ごとの領域は $O(Bn)$ になる。
整数添字の表現上限は残り、不正な添字や上限超過は検証する。
