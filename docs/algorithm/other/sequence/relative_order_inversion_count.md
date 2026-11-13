---
title: Relative Order Inversion Count (基準順序に対する転倒数) [YODANESS]
documentation_of: ../../../../src/algorithm/other/sequence/relative_order_inversion_count.hpp
---

基準となるキーの順序 `reference` に対する、別の列 `order` の転倒数を返す。

## relative_order_inversion_count

```cpp
template<class Key>
long long relative_order_inversion_count(
    const vector<Key>& order,
    const vector<Key>& reference
);
```

各キーを `reference` 内の位置に置き換えた列の転倒数を返す。

## 時間計算量

要素数を $N$ とし、ハッシュと比較を平均 $O(1)$ として、平均 $O(N\log N)$。

## 空間計算量

$O(N)$。

## 注意点

- 2 つの列の長さは等しくなければならない。
- `reference` のキーは相異なり、`order` の全キーが `reference` に存在しなければならない。
- `Key` は `unordered_map` のキーとして使用できなければならない。
- 以上の前提に違反すると `runtime_error` を送出する。
