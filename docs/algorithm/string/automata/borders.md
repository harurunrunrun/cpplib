---
title: Borders (境界) [SUFEQPRE]
documentation_of: ../../../../src/algorithm/string/automata/borders.hpp
---

列の空でない真の接頭辞であり、同時に接尾辞でもあるもの（border）の長さを列挙する。
接頭辞関数（`prefix_function`）の値を末尾からたどる。

## API

```cpp
template<class Sequence>
std::vector<int> border_lengths(const Sequence& sequence)
```

## 引数・戻り値

- `sequence`: border を列挙する入力列。
- 戻り値: すべての border の長さを狭義単調増加順に格納した `std::vector`。
  空列と長さ 1 の列では空の `std::vector` を返す。

## 時間計算量・空間計算量

$N$ を列長、$K$ を border の個数とし、要素の比較を $O(1)$ とする。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `border_lengths(sequence)` | $O(N)$ | 追加領域 $O(N)$、出力 $O(K)$ |

## 注意点

- 空の接頭辞・空の接尾辞・列全体は border として返さない。
- 接頭辞と接尾辞の位置が重なる border も返す。
- `Sequence` は `size()`、0-indexed のランダムアクセス、要素の `operator==` に対応し、列長は `int` に収まることを前提とする。
