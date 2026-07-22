---
title: Relative Order Inversion Count (基準順序に対する転倒数) [YODANESS]
documentation_of: ../src/algorithm/other/sequence/relative_order_inversion_count.hpp
---

キーの基準順序に対する別の列の転倒数を返す。

## relative_order_inversion_count

```cpp
template<class Key>
long long relative_order_inversion_count(
    const vector<Key>& order,
    const vector<Key>& reference
);
```

## 時間計算量

要素数を $N$ とし、hashと比較を平均 $O(1)$ として平均 $O(N\log N)$。

## 空間計算量

$O(N)$。

## 注意点

- 2列の長さは等しくなければならない。
- `reference` のキーは相異なり、`order` の全キーが `reference` に存在する必要がある。
- `Key` は `unordered_map` のキーとして使用できなければならない。
- 前提違反時は `runtime_error` を送出する。
