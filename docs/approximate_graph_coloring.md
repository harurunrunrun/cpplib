---
title: Graph Coloring Heuristics (グラフ彩色ヒューリスティック)
documentation_of: ../src/approximate/graph/coloring.hpp
---

`approximate::graph` 名前空間にある無向グラフの頂点彩色。頂点番号は $[0,N)$ である。平行辺と逆向きに重複した辺は1辺として扱う。自己ループを含むグラフには `std::invalid_argument`、範囲外の端点には `std::out_of_range` を送出する。

# ColoringResult

`colors[v]` は頂点 $v$ の色、`color_count` は使用色数を表す。色は $[0,\mathtt{color\_count})$ に正規化される。

# ColoringSearchResult

探索法が返す結果を表す。

- `colors`: 探索中に得た最良の彩色
- `conflicts`: `tabucol` では同色辺数、`partialcol` では未彩色頂点数、`kempe_chain_interchange` では常に0
- `iterations`: 実行した反復回数
- `converged`: 指定した色数以下の完全な適正彩色を得たか

探索法の `target_color_count` は、$N>0$ なら $[1,N]$、$N=0$ なら0を指定する。この範囲外では `std::invalid_argument` を送出する。同じ入力と `seed` には同じ結果を返す。

# greedy_coloring

指定した頂点順に利用可能な最小色を割り当てる。空の順序は頂点番号順を表す。

- 時間計算量: $O(N+E\log N)$
- 追加空間計算量: $O(N+E)$

# largest_first_order / largest_first_coloring

次数降順、頂点番号昇順の順序を作り、その順で貪欲彩色する。

- `largest_first_order`: 時間計算量は $O(N\log N+E\log N)$、追加空間 $O(N+E)$
- `largest_first_coloring`: 時間計算量は $O(N\log N+E\log N)$、追加空間 $O(N+E)$

# welsh_powell_coloring

largest-first順の貪欲彩色を行う。

- 時間計算量: $O(N\log N+E\log N)$
- 追加空間計算量: $O(N+E)$

# smallest_last_order / smallest_last_coloring

最小次数頂点を順に除去し、除去順を反転したsmallest-last順序を使う。

- `smallest_last_order`: 時間計算量は $O((N+E)\log N)$、追加空間 $O(N+E)$
- `smallest_last_coloring`: 時間計算量は $O((N+E)\log N)$、追加空間 $O(N+E)$

# dsatur_coloring

隣接済み色数、元の次数、頂点番号の順にDSATURの次頂点を決める。

- 時間計算量: $O(N^2+E\log N)$
- 追加空間計算量: $O(N+E)$

不正な順列には `std::invalid_argument` を送出する。

# recursive_largest_first_coloring

Recursive Largest First（RLF）法で彩色する。未彩色部分で次数最大の頂点から色クラスを開始し、現在の色クラスに隣接する未彩色頂点を最も多く持つ頂点を追加する。同値の場合は残りの候補内の次数、頂点番号の順で決める。

- 時間計算量: $O(E\log N+N(N+E))$
- 追加空間計算量: $O(N+E)$

# kempe_chain_interchange

```cpp
ColoringSearchResult kempe_chain_interchange(
    size_t vertex_count,
    const vector<pair<size_t, size_t>>& edges,
    size_t target_color_count,
    uint64_t seed,
    size_t maximum_iterations = 100,
    vector<size_t> initial_colors = {}
);
```

最高色の頂点を低い色へ移す。直接移せない場合は2色で誘導される連結成分をKempe交換してから再彩色する。`initial_colors` を省略するとRLF彩色を初期解に使う。指定する場合は適正彩色でなければならず、色番号は内部で正規化される。返す彩色は、目標達成の有無にかかわらず適正彩色である。

$I=\mathtt{maximum\_iterations}$、探索中の使用色数を $C$ とする。

- 時間計算量: $O(E\log N+N(N+E)+I N C^2(N+E))$
- 追加空間計算量: $O(N+E)$

# tabucol

```cpp
ColoringSearchResult tabucol(
    size_t vertex_count,
    const vector<pair<size_t, size_t>>& edges,
    size_t target_color_count,
    uint64_t seed,
    size_t maximum_iterations = 10000,
    size_t tabu_tenure = 7,
    bool aspiration = true,
    vector<size_t> initial_colors = {}
);
```

同色辺数を目的値とするTabuCol法。各反復で、衝突に接する頂点の色変更を差分評価する。`aspiration=true` なら、それまでの最良値を更新するtabu手を許す。`initial_colors` の各色は $[0,K)$ であり、衝突を含んでもよい。省略時はRLF彩色を $K$ 色へ写したものを初期解にする。最良解を返す。

$K=\mathtt{target\_color\_count}$、$I=\mathtt{maximum\_iterations}$ とする。

- 時間計算量: 初期解省略時 $O(E\log N+N(N+E)+INK)$、指定時 $O(E\log N+INK)$
- 追加空間計算量: $O(NK+N+E)$

# partialcol

```cpp
ColoringSearchResult partialcol(
    size_t vertex_count,
    const vector<pair<size_t, size_t>>& edges,
    size_t target_color_count,
    uint64_t seed,
    size_t maximum_iterations = 10000,
    size_t tabu_tenure = 7,
    bool aspiration = true,
    vector<size_t> initial_colors = {}
);
```

未彩色頂点数を目的値とするPartialCol法。未彩色頂点に色を付け、同色になる隣接頂点を未彩色へ戻す手を差分評価する。未彩色は色番号 $K$ で表す。`initial_colors` を指定する場合、各値は $[0,K]$ であり、彩色済み部分が適正彩色でなければならない。省略時はランダム順の貪欲部分彩色を使う。最良解を返す。

$K=\mathtt{target\_color\_count}$、$I=\mathtt{maximum\_iterations}$ とする。

- 時間計算量: $O(E\log N+NK+I(NK+E))$
- 追加空間計算量: $O(NK+N+E)$

`initial_colors` のサイズ・色・適正性が不正なら `std::invalid_argument` を送出する。$NK$ が `size_t` で表せない場合は `std::length_error`、tabu期限が `size_t` で表せない場合は `std::overflow_error` を送出する。

## 注意点

頂点番号は指定した頂点数の範囲内でなければならない。自己loop・平行辺・重みの扱いは各APIの記載に従い、明記した保証を除いて最適解を返すとは限らない。
