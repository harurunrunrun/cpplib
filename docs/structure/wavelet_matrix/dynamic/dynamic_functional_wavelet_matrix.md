---
title: Dynamic Functional Wavelet Matrix (動的多機能ウェーブレット行列)
documentation_of: ../../../../src/structure/wavelet_matrix/dynamic/dynamic_functional_wavelet_matrix.hpp
---

`DynamicWeightedWaveletMatrix<T, Sum, MAX_SIZE, BIT_WIDTH>` の各要素の重みを、常にその値自身に保つ派生型。列全体を再構築せず、値の挿入・削除・置換を動的ビットベクトルの木に反映する。

## テンプレート引数

```cpp
DynamicFunctionalWaveletMatrix<T, MAX_SIZE = -1, Sum = long long, BIT_WIDTH = (T の bit 数)>
```

- `T`: 値の整数型。`Sum` へ変換できる必要がある。
- `MAX_SIZE`: 列長の上限。省略時（`-1`）は上限なし（`int` の範囲内）。内部は B+ 木なので、指定しても確保する領域は変わらない。
- `Sum`: 和の型
- `BIT_WIDTH`: 値に使う bit 数。省略時は `T` の bit 数。符号付きの `T` では省略しなければならない。

```cpp
using Variable = DynamicFunctionalWaveletMatrix<T>;
using Fixed = DynamicFunctionalWaveletMatrix<T, N>;
```

## 構築

```cpp
DynamicFunctionalWaveletMatrix()
explicit DynamicFunctionalWaveletMatrix(const vector<T>& values)
explicit DynamicFunctionalWaveletMatrix(const array<T, N>& values)
```

各要素の重みを `static_cast<Sum>(values[k])` として構築する。

## 更新

```cpp
void set(int k, T value)
void set_value(int k, T value)
void insert(int position, T value)
T erase(int position)
void push_back(T value)
T pop_back()
```

`set` と `set_value` は値と重みをともに `value` に置き換える。`insert` と `push_back` も重みに値自身を設定する。`erase` と `pop_back` は削除した値を返す。

## クエリ

`size`, `access`, `operator[]`, `weight`, `rank`, `select`, `range_freq`, `kth_smallest`, `kth_largest`, `prev_value`, `next_value`, `sum`, `range_sum`, `sum_k_smallest`, `sum_k_largest` を基底型から継承する。意味は Dynamic Weighted Wavelet Matrix と同じで、`sum` 系は値の総和を `Sum` 型で返す。

## 時間計算量

現在の列長を $N$、$D = \texttt{BIT_WIDTH}$ とする。基底型の動的ビットベクトルは決定的な平衡 B+ 木なので、以下は最悪計算量である。

- コンストラクタ（`vector` / `array`）: $O(DN)$
- デフォルトコンストラクタ、`size`: $O(1)$
- `weight`, `sum`: $O(\log N)$
- その他の公開されている更新・クエリ: $O(D \log N)$

## 空間計算量

$O(DN)$。

## 注意点

- 添字・出現番号は 0-indexed、区間は半開区間である。
- `MAX_SIZE` の超過、`BIT_WIDTH` bit に収まらない値、範囲外の添字・区間・`k`、空列からの `pop_back` では `std::runtime_error` を送出する。
- 基底型の `set_weight` は隠されておらず呼び出せるが、呼ぶと「重み = 値」の不変条件が崩れるため使わないこと。
