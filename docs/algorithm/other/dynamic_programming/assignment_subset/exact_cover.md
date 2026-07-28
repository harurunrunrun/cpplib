---
title: Exact Cover (完全被覆)
documentation_of: ../../../../../src/algorithm/other/dynamic_programming/assignment_subset/exact_cover.hpp
---

各列をちょうど1回覆う入力行の部分集合を1つ求める。

## API

```cpp
optional<vector<int>> exact_cover(
    int column_count,
    const vector<vector<int>>& rows
);
```

解があれば入力行番号を返し、なければ `nullopt` を返す。

## 時間計算量

Dancing LinksによるAlgorithm Xを用いる。入力構築は非零要素数 $Z$ に対して $O(Z)$、探索は最悪 $O(2^R)$ だが、各段階で残存要素数が最小の列を選ぶ。

## 空間計算量

$O(C+Z)$ と再帰深さ $O(C)$。

## 注意点

入力条件と例外は `DancingLinks::add_row` と同じである。
