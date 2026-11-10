---
title: Nagamochi--Ibaraki Sparsification (Nagamochi--Ibaraki疎化)
documentation_of: ../../../../../src/algorithm/graph/connectivity/decomposition/nagamochi_ibaraki_sparsification.hpp
---

無向多重グラフから高々 $k(N - 1)$ 本の辺を選び、容量 $k$ までのすべてのカットを保存する疎な証明部分グラフを作る。残った辺から極大全域森を取り出す操作を $k$ 回繰り返し、それらの和集合を返す。

## API

```cpp
vector<int> nagamochi_ibaraki_sparsification(
    int vertex_count,
    const vector<pair<int, int>>& edges,
    int connectivity
)
```

選んだ入力辺の ID を昇順で返す。任意の頂点集合が定めるカットについて、元のグラフでそのカットを横断する辺の数を $c$、戻り値の辺だけで横断する辺の数を $c'$ とすると、$\min(k, c) = \min(k, c')$ が成り立つ（$k$ は `connectivity`）。

## 時間計算量

頂点数を $N$、自己ループを除く辺数を $M$、`connectivity` を $k$、戻り値の辺数を $S$ とする。

- 入力検査: $O(N + M)$
- 残りの辺から極大全域森を高々 $k$ 回抽出: $O(k(N + M \alpha(N)))$（毎回 Union-Find を初期化する）
- 辺 ID のソート: $O(S \log (S + 1))$
- 全体: $O(k(N + M \alpha(N)) + S \log (S + 1))$

## 空間計算量

$O(N + M)$。戻り値は高々 $S \le \min(M, k(N - 1))$ 本。

## 注意点

- `vertex_count` と `connectivity` は非負、辺の端点は $[0, N)$ でなければならない。違反時は `std::runtime_error` を送出する。
- 多重辺は別々に扱う。カットに寄与しない自己ループは戻り値に含めない。
- `connectivity == 0` なら空列を返す。
