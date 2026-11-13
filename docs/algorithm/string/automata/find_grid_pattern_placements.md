---
title: Grid Pattern Placements (格子パターン配置検索) [WPUZZLES]
documentation_of: ../../../../src/algorithm/string/automata/find_grid_pattern_placements.hpp
---

長方形の格子の直線上に現れる複数の文字列パターンを、8 方向まとめて Aho--Corasick 法で検索する。

## API

### `GridPatternPlacement`

```cpp
struct GridPatternPlacement {
    int row;
    int column;
    int direction;
};
```

パターンの先頭文字の行・列と、読み進める方向を表す。方向 0 から 7 は順に、上、右上、右、
右下、下、左下、左、左上である。未発見を表す初期値はすべて `-1` である。

### `find_grid_pattern_placements(grid, patterns)`

```cpp
vector<GridPatternPlacement> find_grid_pattern_placements(
    const vector<string>& grid,
    const vector<string>& patterns
);
```

各 `patterns[i]` の配置を入力順に返す。同じパターンが複数ある場合も、
対応する各要素に同じ配置を返す。配置が複数ある場合は方向番号の小さいものを優先し、
その方向の走査順で最初に検出した配置を採用する。見つからないパターンの要素はすべて `-1` のままである。

## 時間計算量・空間計算量

格子のマス数を $G$、パターンの長さの合計を $D$、パターン数を $P$ とする。
大文字の英字 26 文字を固定のアルファベットとして扱い、その大きさは定数とみなす。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `GridPatternPlacement` の構築 | $O(1)$ | $O(1)$ |
| `find_grid_pattern_placements` | $O(G+D+P)$ | $O(D+P)$ |

## 注意点

- `grid` は空でない長方形で、各文字は `A` から `Z` でなければならない。
- 各パターンは空でなく、各文字は `A` から `Z` でなければならない。
- 形状・文字が不正な場合や空パターンには `std::invalid_argument` を、内部の添字を `int` で表せない場合は `std::length_error` を送出する。
