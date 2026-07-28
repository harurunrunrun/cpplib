---
title: Nagamochi--Ibaraki Sparsification (Nagamochi--Ibaraki疎化)
documentation_of: ../../../../../src/algorithm/graph/connectivity/decomposition/nagamochi_ibaraki_sparsification.hpp
---

無向多重グラフから高々 $k(N-1)$ 本の辺を選び、すべてのカット容量を $k$ まで保存する疎な証明部分グラフを作る。

## API

```cpp
vector<int> nagamochi_ibaraki_sparsification(
    int vertex_count,
    const vector<pair<int, int>>& edges,
    int connectivity
)
```

選ばれた入力辺のIDを昇順で返す。任意の頂点集合が定めるカットについて、元の横断辺数を $c$、戻り値だけの横断辺数を $c'$ とすると、`min(connectivity, c) == min(connectivity, c')` が成り立つ。

## 時間計算量

頂点数を $N$、自己ループを除く辺数を $M$、`connectivity` を $k$ とする。

- 入力検査: $O(N+M)$
- 残余グラフ上の最大森を高々 $k$ 回抽出: $O(kM\alpha(N))$
- 辺IDの整列: $O(kN\log(kN+1))$
- 全体: $O(N+kM\alpha(N)+kN\log(kN+1))$

## 空間計算量

$O(N+M)$。戻り値は高々 $\min(M,k(N-1))$ 本。

## 注意点

辺の端点は $[0,N)$、`connectivity` は非負でなければならない。違反時は `runtime_error` を送出する。多重辺は別々に扱い、カットに寄与しない自己ループは戻り値に含めない。`connectivity == 0` なら空列を返す。
