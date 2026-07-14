---
title: Permutation Reconstruction from Larger-Before Counts (前方大要素数からの順列復元) [ORDERS]
documentation_of: ../src/algorithm/other/reconstruct_permutation_from_larger_before.hpp
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
