---
title: Card Trick Permutation (カードトリック順列) [CTRICK]
documentation_of: ../src/algorithm/other/card_trick_permutation.hpp
---

空き位置を巡回しながらカードを配置する規則で得られる順列を返します。

## API

### `card_trick_permutation(size)`

```cpp
template<int MAX_SIZE = 20000>
vector<int> card_trick_permutation(int size);
```

長さ `size` の配置結果を位置順に返します。

## 時間計算量

$O(size\log MAX\_SIZE)$ です。

## 空間計算量

$O(size\log MAX\_SIZE)$ を上限とします。

## 注意点

- `0 <= size <= MAX_SIZE` が必要です。
- 範囲違反や内部の順序統計失敗では `runtime_error` を送出します。
