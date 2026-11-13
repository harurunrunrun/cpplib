---
title: Rake–Compress Tree Contraction (レイク・コンプレス木縮約)
documentation_of: ../../../../src/algorithm/tree/decomposition/rake_compress_tree_contraction.hpp
---

葉の除去（rake）と次数 2 の頂点の縮約（compress）を、互いに独立な操作からなるラウンドとして列挙する。

## API

```cpp
struct TreeContractionStep{
    enum class Kind { rake, compress };
    Kind kind;
    int vertex;
    int left;
    int right;
};
std::vector<std::vector<TreeContractionStep>>
rake_compress_tree_contraction(const std::vector<std::vector<int>>& graph);
```

- `TreeContractionStep`: 操作の種類 `kind`、除去する頂点 `vertex`、接続先を保持する。rake では `left` が接続先（なければ `-1`）、`right` は `-1` である。compress では `left`, `right` が両隣の頂点で、両者の間に辺が張られる。
- `rake_compress_tree_contraction(graph)`: 頂点が 1 つになるまでの縮約ラウンドの列を返す。

## 時間計算量

$O(N\log N)$ 時間、$O(N\log N)$ 空間である。

## 注意点

- 入力は単純な無向木とする。
- 辺の端点が範囲外なら `std::out_of_range`、ラウンドで操作が 1 つも選べない（木でない）場合は `std::invalid_argument` を送出する。
