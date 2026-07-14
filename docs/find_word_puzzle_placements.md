---
title: Find Word Puzzle Placements (単語パズル配置検索) [WPUZZLES]
documentation_of: ../src/algorithm/string/find_word_puzzle_placements.hpp
---

# WordPuzzlePlacement

```cpp
struct WordPuzzlePlacement {
    int row;
    int column;
    int direction;
};
```

単語の先頭位置と方向を表す。方向0から7は、上、右上、右、右下、下、左下、左、左上に対応する。

# find_word_puzzle_placements

```cpp
vector<WordPuzzlePlacement> find_word_puzzle_placements(
    const vector<string>& grid,
    const vector<string>& words
)
```

## 引数

- `grid`: 大文字英字からなる長方形格子。
- `words`: 検索する非空の大文字英単語。

## 戻り値

各単語について、方向、走査開始位置の順で最初に見つかった配置を返す。見つからない要素は全fieldが `-1`。

## API別の時間計算量・空間計算量

格子のマス数を $G$、単語の総長を $D$ とする。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `find_word_puzzle_placements` | $O(G+D)$ | $O(G+D)$ |

## 注意点

- 空・非長方形の格子、空単語、小文字などの不正文字では `invalid_argument` を送出する。
- 記載した時間計算量の定数には8方向の走査を含む。
