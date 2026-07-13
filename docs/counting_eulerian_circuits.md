---
title: Counting Eulerian Circuits (オイラー閉路数え上げ)
documentation_of: ../src/algorithm/graph/counting_eulerian_circuits.hpp
---

BEST theoremと有向Matrix-Tree theoremで、有向多重グラフのEuler circuit数を体 `T` 上で求める。辺はラベル付きとして区別し、巡回列のcyclic shiftは同一視する。すなわち、入力の0番目の辺を先頭に固定したEuler tour数を返す。

# Euler circuit数

```cpp
template<class T, size_t MAX_VERTEX>
T count_eulerian_circuits(
    int vertex_count,
    const vector<pair<int, int>>& directed_edges
)
```

各非孤立頂点で入次数と出次数が等しくない場合、または非孤立部分がstrongly connectedでない場合は `T()` を返す。自己loopと多重辺を許し、孤立頂点は無視する。辺が空の場合は、先頭に固定する辺が存在しないため `T()` を返す。

`T` は既存のMatrix-Tree theoremが要求する体でなければならない。典型的には素数modのmodintを使う。非孤立頂点数は `MAX_VERTEX` 以下でなければならない。

## 時間計算量

- `O(M + MAX_VERTEX^2 + A^3)`

A は非孤立頂点数。次数階乗の構築を含めて O(M)、固定容量行列の初期化が O(MAX_VERTEX^2)、有向Laplacian minorの行列式が O(A^3)。

## 空間計算量

- `O(N + M + MAX_VERTEX^2)`

## 注意点

`vertex_count`または辺端点が範囲外、あるいは非孤立頂点数が `MAX_VERTEX` を超える場合は `runtime_error` を送出する。
