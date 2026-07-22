---
title: Grid Pattern Placements (格子パターン配置検索) [WPUZZLES]
documentation_of: ../src/algorithm/string/automata/find_grid_pattern_placements.hpp
---

長方形格子の直線上に現れる複数の文字列パターンを8方向まとめて検索します。

## API

### `GridPatternPlacement`

```cpp
struct GridPatternPlacement {
    int row;
    int column;
    int direction;
};
```

パターン先頭の行・列と方向を表します。方向0から7は順に、上、右上、右、
右下、下、左下、左、左上です。未発見を表す初期値はすべて `-1` です。

### `find_grid_pattern_placements(grid, patterns)`

```cpp
vector<GridPatternPlacement> find_grid_pattern_placements(
    const vector<string>& grid,
    const vector<string>& patterns
);
```

各 `patterns[i]` の配置を入力順に返します。同じパターンが複数ある場合も、
対応する各要素へ同じ配置を返します。複数の配置がある場合は方向番号を優先し、
その方向の走査順で最初に検出した配置を採用します。

## API別の時間計算量・空間計算量

格子のマス数を $G$、パターンの総長を $D$、パターン数を $P$ とします。
大文字英字26文字を固定アルファベットとして扱います。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `GridPatternPlacement` の構築 | $O(1)$ | $O(1)$ |
| `find_grid_pattern_placements` | $O(G+D+P)$ | $O(D+P)$ |

## 注意点

- `grid` は空でない長方形で、各文字は `A` から `Z` でなければなりません。
- 各パターンは非空で、各文字は `A` から `Z` でなければなりません。
- 不正な形状・文字・空パターンには `invalid_argument`、内部添字を
  `int` で表せない場合は `length_error` を送出します。
