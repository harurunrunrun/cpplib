---
title: Permutation Reconstruction from Larger-Before Counts (前方大要素数からの順列復元) [ORDERS]
documentation_of: ../../../../src/algorithm/other/sequence/reconstruct_permutation_from_larger_before.hpp
---

各位置について「それより前にある、より大きな値の個数」が与えられたとき、$1$ から $N$ の順列を復元する。

## API

### `reconstruct_permutation_from_larger_before(larger_before)`

```cpp
template<int MAX_SIZE = -1>
vector<int> reconstruct_permutation_from_larger_before(
    const vector<unsigned int>& larger_before
);
```

有効な入力に対し、条件を満たす順列を返す。末尾から順に、残っている値のうち `larger_before[i]` 番目（0-indexed）に大きい値を位置 `i` に割り当てる。

`MAX_SIZE` を省略すると、残りの値の管理に `unsigned int` の範囲を扱う可変長の `Int_Set`（分岐数 16 の trie）を使う。指定すると全域が $[0, \texttt{MAX_SIZE}]$ の固定長版になる。

```cpp
auto result = reconstruct_permutation_from_larger_before(larger_before);
auto fixed = reconstruct_permutation_from_larger_before<100000>(larger_before);
```

## 時間計算量

trie の深さを $W$（固定長版は $O(\log\texttt{MAX_SIZE})$、可変長版は `unsigned int` の範囲に対する定数）として $O(NW)$。

## 空間計算量

$O(NW)$ 以下。

## 注意点

- 固定長版では $N\le\texttt{MAX_SIZE}$ でなければならない。違反時は `runtime_error` を送出する。
- 条件を満たす順列が存在しない入力では `runtime_error` を送出する。
