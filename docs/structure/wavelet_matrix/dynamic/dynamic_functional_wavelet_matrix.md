---
title: Dynamic Functional Wavelet Matrix (動的多機能ウェーブレット行列)
documentation_of: ../../../../src/structure/wavelet_matrix/dynamic/dynamic_functional_wavelet_matrix.hpp
---

`DynamicWeightedWaveletMatrix` の各重みを値自身に保つ派生型。
列全体の再構築を行わず、値の挿入・削除・置換を動的 bitvector の木へ反映する。

## 構築

```cpp
DynamicFunctionalWaveletMatrix()
DynamicFunctionalWaveletMatrix(const vector<T>& values)
DynamicFunctionalWaveletMatrix(const array<T, N>& values)
```

## 更新

```cpp
void set(int k, T value)
void set_value(int k, T value)
void insert(int position, T value)
T erase(int position)
void push_back(T value)
T pop_back()
```

`set` と `set_value` は値と重みをともに `value` へ置換する。
`insert` と `push_back` も重みに値自身を設定し、`erase` と `pop_back` は
削除した値を返す。

## クエリ

`size`, `access`, `operator[]`, `weight`, `rank`, `select`,
`range_freq`, `kth_smallest`, `kth_largest`, `prev_value`,
`next_value`, `sum`, `range_sum`, `sum_k_smallest`,
`sum_k_largest` を基底型から継承する。

## 時間計算量

$D = BIT_WIDTH$、現在の列長を $N$ とする。基底型の動的 bitvector は
決定的な平衡 B+ 木なので、以下は worst-case 計算量である。

- vector/array constructor: $O(DN)$
- default constructor, `size`: $O(1)$
- `weight`, `sum`: $O(\log N)$
- その他の公開更新・クエリ: $O(D\log N)$
- 空間計算量: $O(DN)$

## 注意点

添字・出現番号は0-indexed、区間は半開区間。`MAX_SIZE`、bit幅、添字、
区間、順序の違反では `runtime_error` を送出する。
基底型の `set_weight` を明示的に呼ぶと「重み=値」の不変条件を崩せるため、
多機能版では通常使用しない。

## 容量を省略した可変長版

容量のテンプレート引数 `MAX_SIZE` は省略でき、既定値 `-1` ではvectorを使う。
容量を指定した既存の固定配列版も同じヘッダで利用できる。

```cpp
using Variable = DynamicFunctionalWaveletMatrix<T>;
using Fixed = DynamicFunctionalWaveletMatrix<T, N>;
```

可変長版は入力長 $n$ とビット幅 $B$ に応じた配列だけを確保し、構築・照会の計算量は上記と同じ。
長さに依存する固定容量の領域は $O(n)$ に、ビット段ごとの領域は $O(Bn)$ になる。
整数添字の表現上限は残り、不正な添字や上限超過は検証する。
