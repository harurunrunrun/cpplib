---
title: Minimum Cyclic Rotation (辞書順最小巡回シフト) [BEADS]
documentation_of: ../src/algorithm/string/minimum_cyclic_rotation.hpp
---

列を巡回させて得られる列のうち、辞書順最小になる開始位置をBooth法で求める。

## `minimum_cyclic_rotation_index(first, last, compare)`

```cpp
std::size_t index = minimum_cyclic_rotation_index(first, last, compare);
```

random access iteratorの半開範囲 `[first,last)` に対して、`index` から始めて末尾の
次を先頭へ戻る長さ $N$ の巡回列が、`compare` の定める辞書順で最小になる
0-indexedの開始位置を返す。最小の巡回列を与える位置が複数ある場合は最小の位置を
返す。

`compare` はstrict weak orderingでなければならない。`last < first` の反転範囲は
`std::invalid_argument`を送出する。空範囲の返り値は `0`。

## `minimum_cyclic_rotation_index(text)`

```cpp
std::size_t index = minimum_cyclic_rotation_index(std::string_view(text));
```

文字の通常の昇順を使う文字列用overload。返り値は0-indexedである。SPOJ BEADSの
出力は1-indexedなので、この返り値に1を加える。

## API別の時間計算量・空間計算量

`N = last - first`または`text.size()`とする。

| API | 時間計算量 | 空間計算量（追加領域） |
| --- | --- | --- |
| iterator版 | $O(N)$ | $O(1)$ |
| `string_view`版 | $O(N)$ | $O(1)$ |

## 注意点

- 入力列を変更せず、巡回列そのものは構築しない。
- 同値判定には `!compare(a,b) && !compare(b,a)` を使う。
- iteratorは同じ有効範囲を指し、計算中に無効化されてはならない。
