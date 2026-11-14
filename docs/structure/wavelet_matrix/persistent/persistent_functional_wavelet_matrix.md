---
title: Persistent Functional Wavelet Matrix (完全永続多機能ウェーブレット行列)
documentation_of: ../../../../src/structure/wavelet_matrix/persistent/persistent_functional_wavelet_matrix.hpp
---

値自身を重みとする完全永続ウェーブレット行列。`PersistentWeightedWaveletMatrix<T, Sum, MAX_SIZE, MAX_VERSION, BIT_WIDTH>` を継承する。任意の版から `set(version, k, value)` で値と重みを同時に更新でき、すべての過去の版で頻度・順序統計・値域和を求められる。

## テンプレート引数

```cpp
PersistentFunctionalWaveletMatrix<T, MAX_SIZE = -1, MAX_VERSION = -1, Sum = long long, BIT_WIDTH = (T の bit 数)>
```

- `T`: `bool` 以外の整数型
- `MAX_SIZE`, `MAX_VERSION`: 列長・版数の上限（`-1` なら上限なし）。意味は基底型と同じで、指定しても確保する領域は変わらない。
- `Sum`: 和の型。`T` から変換でき、`Sum{}`、加算、減算を使える必要がある。
- `BIT_WIDTH`: 値を表す bit 数。省略時は `T` の bit 数。

```cpp
using Variable = PersistentFunctionalWaveletMatrix<T>;
using Fixed = PersistentFunctionalWaveletMatrix<T, N, Versions>;
```

## API

```cpp
PersistentFunctionalWaveletMatrix()
explicit PersistentFunctionalWaveletMatrix(const vector<T>& values)
explicit PersistentFunctionalWaveletMatrix(const array<T, N>& values)
int set(int version, int k, T value)
```

コンストラクタは空列、または各要素の重みを `static_cast<Sum>(values[k])` とした列を版 0 として構築する。`set` は値と重みをともに `value` にした新しい版を作り、その番号を返す。

ほかに基底型の `size`, `versions`, `latest_version`, `access`, `weight`, `fork`, `rank`, `select`, `kth_smallest`, `kth_largest`, `range_freq`, `sum`, `range_sum`, `sum_k_smallest`, `sum_k_largest`, `prev_value`, `next_value` を利用できる。

## 時間計算量

$N$ を列長、$D = \texttt{BIT_WIDTH}$、$H = O(\log(N + 1))$ とする。

- 構築: $O(DN)$
- `size`, `versions`, `latest_version`: $O(1)$、`fork`: $O(D)$
- `weight`, `sum`: $O(H)$
- `set` とその他のクエリ: $O(DH)$

更新 1 回あたりの追加メモリは $O(DH)$ である。

## 注意点

- 版・添字・半開区間・bit 幅・容量・例外時のロールバックの規約は基底型と同じである。
- 継承した `set_value` / `set_weight` は隠されておらず呼び出せるが、単独で使うと「重み = 値」の不変条件が保たれないため使わないこと。
