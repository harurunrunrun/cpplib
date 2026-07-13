---
title: Greedy Graph Coloring
documentation_of: ../src/approximate/graph/coloring.hpp
---

`approximate::graph` 名前空間にある無向グラフの貪欲頂点彩色。`ColoringResult::colors[v]` が頂点色、`color_count` が使用色数である。平行辺は1辺として扱い、自己ループには `std::invalid_argument` を送出する。

# greedy_coloring

指定した頂点順に利用可能な最小色を割り当てる。空の順序は頂点番号順を表す。

- 時間計算量: $O(N+E\log N)$
- 追加空間計算量: $O(N+E)$

# largest_first_order / largest_first_coloring

次数降順、頂点番号昇順の順序を作り、その順で貪欲彩色する。

- `largest_first_order`: 時間 $O(N\log N+E\log N)$、追加空間 $O(N+E)$
- `largest_first_coloring`: 時間 $O(N\log N+E\log N)$、追加空間 $O(N+E)$

# welsh_powell_coloring

largest-first順の貪欲彩色を行う。

- 時間計算量: $O(N\log N+E\log N)$
- 追加空間計算量: $O(N+E)$

# smallest_last_order / smallest_last_coloring

最小次数頂点を順に除去し、除去順を反転したsmallest-last順序を使う。

- `smallest_last_order`: 時間 $O((N+E)\log N)$、追加空間 $O(N+E)$
- `smallest_last_coloring`: 時間 $O((N+E)\log N)$、追加空間 $O(N+E)$

# dsatur_coloring

隣接済み色数、元の次数、頂点番号の順にDSATURの次頂点を決める。

- 時間計算量: $O(N^2+E\log N)$
- 追加空間計算量: $O(N+E)$

不正な順列には `std::invalid_argument`、範囲外の端点には `std::out_of_range` を送出する。
