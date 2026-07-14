---
title: Range Majority Query (区間多数決クエリ) [majority_voting]
documentation_of: ../src/algorithm/range/range_majority_query.hpp
---

点更新を行いながら、半開区間で過半数を占める値を求める。

```cpp
RangeMajorityQuery<T, Hash> query(sequence);
```

区間候補はBoyer--Mooreの相殺をmonoidとしてsegment treeに保持する。
候補の出現回数は、値ごとに現在の添字を保持するorder-statistics treapで検証する。
候補が実際に過半数であることを必ず確認するため、過半数が存在しない区間も正しく判定する。

## API

### `RangeMajorityQuery(sequence)`

初期列から構築する。`T` は等値比較と `Hash` によるhash計算が可能でなければならない。

時間計算量は期待 $O(N\log N)$、空間計算量は $O(N+D)$。
ここで $D$ は現在までに登場した異なる値の数。

### `size()`

要素数を返す。時間計算量は $O(1)$。

### `get(index)`

`index` の現在値へのconst参照を返す。時間計算量は $O(1)$。

### `set(index, value)`

`index` の値を `value` に変更する。同じ値への更新は何もしない。

時間計算量は期待 $O(\log N)$。

### `majority(left, right)`

半開区間 $[left,right)$ の長さの半分より多く現れる値を返す。
存在しない場合と空区間では `nullopt` を返す。

時間計算量は期待 $O(\log N)$。

## 注意点

添字または区間が範囲外なら `runtime_error` を送出する。
Treapのpriorityは添字からSplitMix64で決定し、計算量はその平衡性に対する期待値。
