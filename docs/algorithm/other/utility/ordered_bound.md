---
title: Ordered Bound Search (順序付き境界探索)
documentation_of: ../../../../src/algorithm/other/utility/ordered_bound.hpp
---

昇順の列と、`lower_bound` / `upper_bound` memberを持つ順序付きcontainerに
共通の境界探索APIを提供する。const containerに対してはconst iteratorを返す。

## `ordered_lower_bound`

```cpp
auto ordered_lower_bound(Range& range, const Value& value);
```

`value` 以上である最初の要素を返す。存在しない場合は `end(range)` を返す。

- 時間計算量: ordered containerとrandom-access列では $O(\log n)$、
  それ以外の列ではiterator移動を含めて $O(n)$
- 空間計算量: $O(1)$

## `ordered_upper_bound`

```cpp
auto ordered_upper_bound(Range& range, const Value& value);
```

`value` より大きい最初の要素を返す。存在しない場合は `end(range)` を返す。

- 時間計算量: ordered containerとrandom-access列では $O(\log n)$、
  それ以外の列ではiterator移動を含めて $O(n)$
- 空間計算量: $O(1)$

## `ordered_last_less_equal`

```cpp
auto ordered_last_less_equal(Range& range, const Value& value);
```

`value` 以下である最後の要素を返す。存在しない場合は `end(range)` を返す。

- 時間計算量: `ordered_upper_bound` と同じ
- 空間計算量: $O(1)$

## `ordered_last_less`

```cpp
auto ordered_last_less(Range& range, const Value& value);
```

`value` 未満である最後の要素を返す。存在しない場合は `end(range)` を返す。

- 時間計算量: `ordered_lower_bound` と同じ
- 空間計算量: $O(1)$

## 注意点

member境界探索を持たない `Range` は、比較関数 `operator<` に関して昇順で
なければならない。member境界探索を持つcontainerでは、そのcontainerの
比較規則を用いる。前任要素が存在しない場合も `end(range)` を返すため、
dereference前に結果を確認する必要がある。
