---
title: Exact Cover (完全被覆)
documentation_of: ../../../../../src/algorithm/other/dynamic_programming/assignment_subset/exact_cover.hpp
---

各列をちょうど 1 回ずつ覆う入力行の部分集合を 1 つ求める。探索中は、選択可能な行が最も少ない未被覆の列を分岐の対象にする。

## API

```cpp
std::optional<std::vector<int>> exact_cover(
    int column_count,
    const std::vector<std::vector<int>>& rows
);
```

解があれば選んだ入力行の番号（0-indexed）を返し、なければ `std::nullopt` を返す。
列数が $0$ なら空の解を返す。空行はどの列も覆わないため、戻り値には含めない。

## 時間計算量

行数を $R$、列数を $C$、全行に含まれる列番号の総数を $Z$ とする。

- 入力の検査と、列から行への索引の構築: $O(C+Z)$ 時間、$O(C+Z)$ 空間
- 1 つの探索節点での分岐列の選択: $O(CZ)$ 時間
- 探索全体: 最悪 $O(2^R C Z)$ 時間
- 再帰の深さ: $O(C)$、探索用の追加空間: $O(C+R)$

## 注意点

- `column_count` は非負で、各列番号は列数 $C$ に対して $[0, C)$ の範囲でなければならない。
- 同じ行に同じ列を複数回指定してはならない。違反時は `std::runtime_error` を送出し、行数が `int` の範囲を超える場合は `std::length_error` を送出する。
- 返す解は 1 つだけであり、辞書順最小であることは保証しない。
