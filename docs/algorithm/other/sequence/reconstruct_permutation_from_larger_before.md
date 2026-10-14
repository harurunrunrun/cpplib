---
title: Permutation Reconstruction from Larger-Before Counts (前方大要素数からの順列復元) [ORDERS]
documentation_of: ../../../../src/algorithm/other/sequence/reconstruct_permutation_from_larger_before.hpp
---

各位置より前にある大きな値の個数から、$1$ から $N$ の順列を復元します。

## API

### `reconstruct_permutation_from_larger_before(larger_before)`

```cpp
template<int MAX_SIZE = 200000>
vector<int> reconstruct_permutation_from_larger_before(
    const vector<unsigned int>& larger_before
);
```

有効な入力に対し、条件を満たす順列を返します。

## 時間計算量

$O(N\log MAX\_SIZE)$ です。

## 空間計算量

$O(N\log MAX\_SIZE)$ を上限とします。

## 注意点

- `N <= MAX_SIZE` が必要です。
- 条件を満たす順列が存在しない入力では `runtime_error` を送出します。

## 容量を省略する可変長版

容量テンプレート引数をすべて省略すると、実行時サイズに合わせた `std::vector` を使う。容量を明示する固定長版も同じヘッダにある。

```cpp
auto result = reconstruct_permutation_from_larger_before(larger_before);
auto fixed = reconstruct_permutation_from_larger_before<100000>(larger_before);
```

$N$ 要素に対して座標 bit 幅を $W$ として $O(NW)$ 時間・$O(NW)$ 空間（分岐数16の trie）。整数座標の範囲は `unsigned int` の定義に従う。
