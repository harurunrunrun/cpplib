---
title: Exact Cover (完全被覆)
documentation_of: ../../../../../src/algorithm/other/dynamic_programming/assignment_subset/exact_cover.hpp
---

各列をちょうど1回覆う入力行の部分集合を1つ求める。探索中は、選択可能な行が
最も少ない未被覆列を分岐対象にする。

## API

```cpp
optional<vector<int>> exact_cover(
    int column_count,
    const vector<vector<int>>& rows
);
```

解があれば選んだ入力行番号を返し、なければ `nullopt` を返す。
列数が $0$ なら空の解を返す。空行はどの列も覆わないため返り値には含めない。

## 時間計算量

行数を $R$、列数を $C$、全行に含まれる列番号の総数を $Z$ とする。

- 入力検査と列から行への索引構築: $O(C+Z)$ 時間、$O(C+Z)$ 空間
- 1探索節点での分岐列選択: $O(CZ)$ 時間
- 探索全体: 最悪 $O(2^R C Z)$ 時間
- 再帰深さ: $O(C)$、探索用追加空間: $O(C+R)$

## 注意点

`column_count` は非負で、各列番号は $[0,column_count)$ でなければならない。
同じ行に同じ列を複数回指定してはならない。違反時は `runtime_error`、行数が `int` の範囲を超える場合は `length_error` を送出する。
返す解は1つだけであり、辞書順最小性は保証しない。
