---
title: Minimum Cyclic Rotation (辞書順最小巡回シフト) [BEADS] [MINMOVE]
documentation_of: ../../../../src/algorithm/string/sequence/minimum_cyclic_rotation.hpp
---

列を巡回シフトして得られる列のうち、辞書順で最小になるものの開始位置を Booth 法で求める。

## `minimum_cyclic_rotation_index(first, last, compare)`

```cpp
template<std::random_access_iterator Iterator, class Compare = std::less<>>
std::size_t minimum_cyclic_rotation_index(Iterator first, Iterator last, Compare compare = Compare{});
```

ランダムアクセスイテレータの半開範囲 `[first, last)` について、位置 `index` から始めて末尾の
次は先頭に戻る長さ $N$ の巡回列が、`compare` の定める辞書順で最小になるような
0-indexed の開始位置を返す。最小の巡回列を与える位置が複数ある場合は、最小の位置を
返す。空範囲の返り値は `0` である。

## `minimum_cyclic_rotation_index(text)`

```cpp
std::size_t index = minimum_cyclic_rotation_index(std::string_view(text));
```

文字の通常の昇順を使う文字列用のオーバーロードである。返り値は 0-indexed である。

## 時間計算量・空間計算量

$N$ を `last - first` または `text.size()` とする。

| API | 時間計算量 | 空間計算量（追加領域） |
| --- | --- | --- |
| イテレータ版 | $O(N)$ | $O(1)$ |
| `string_view` 版 | $O(N)$ | $O(1)$ |

## 注意点

- `compare` は狭義弱順序でなければならない。同値判定には `!compare(a, b) && !compare(b, a)` を使う。
- `last < first` の反転した範囲では `std::invalid_argument` を送出する。
- 入力の列を変更せず、巡回列そのものは構築しない。
- イテレータは同じ有効な範囲を指し、計算中に無効化されてはならない。
