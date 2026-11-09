---
title: Range Majority Query (区間過半数クエリ) [majority_voting]
documentation_of: ../../../../src/algorithm/range/static_query/range_majority_query.hpp
---

点更新を行いながら、半開区間で過半数を占める値を求める。

```cpp
RangeMajorityQuery<T, Hash> query(sequence);
```

各区間の候補は、Boyer--Moore の多数決アルゴリズムの相殺操作をモノイドとしてセグメント木に保持して求める。
候補の出現回数は、値ごとに現在の出現位置を保持する順序統計 treap で検証する。
候補が実際に過半数であることを必ず確認するため、過半数が存在しない区間も正しく判定できる。

## API

### `RangeMajorityQuery(sequence)`

初期列から構築する。`T` は `operator==` による等値比較と、`Hash` によるハッシュ計算が可能でなければならない。
引数なしのデフォルトコンストラクタ（空列）もある。

時間計算量は期待 $O(N\log N)$、空間計算量は $O(N+D)$ である。
ここで $D$ はこれまでに登場した相異なる値の数である。

### `size()`

要素数を返す。時間計算量は $O(1)$。

### `get(index)`

位置 `index` の現在値への const 参照を返す。時間計算量は $O(1)$。

### `set(index, value)`

位置 `index` の値を `value` に変更する。現在値と等しい値への更新は何もしない。

時間計算量は期待 $O(\log N)$。

### `majority(left, right)`

半開区間 `[left, right)` に、区間長の半分より多く現れる値があればそれを返す。
存在しない場合と空区間では `std::nullopt` を返す。

時間計算量は期待 $O(\log N)$。

## 注意点

- 添字が `0 <= index < size()` を満たさない場合、または区間が `0 <= left <= right <= size()` を満たさない場合は `std::runtime_error` を送出する。
- treap の優先度は添字から SplitMix64 で決定する。計算量はその平衡性に関する期待値である。
