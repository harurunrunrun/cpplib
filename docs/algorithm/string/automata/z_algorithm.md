---
title: Z Algorithm (Zアルゴリズム) [zalgorithm]
documentation_of: ../../../../src/algorithm/string/automata/z_algorithm.hpp
---

列の各位置から始まる接尾辞と、列全体との最長共通接頭辞の長さ（Z 配列）を求める。

## 関数

```cpp
template<class Sequence>
std::vector<int> z_algorithm(const Sequence& s)
```

- 戻り値 `z[i]` は、`s` と `s` の位置 `i` から始まる接尾辞との最長共通接頭辞の長さである。
- `z[0] = s.size()` である。空列では空の `std::vector` を返す。

## 時間計算量・空間計算量

$N$ を列の長さとする。

| API | 時間計算量 | 出力領域 |
| --- | --- | --- |
| `z_algorithm(s)` | $O(N)$ | $O(N)$ |

## 注意点

- `Sequence` は `size()`、0-indexed のランダムアクセス、要素の `operator==` に対応している必要がある。
- 長さは `int` に収まることを前提とする。
