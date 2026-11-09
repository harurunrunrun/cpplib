---
title: Counting Eulerian Circuits (オイラー閉路数え上げ) [counting_eulerian_circuits]
documentation_of: ../../../../src/algorithm/graph/traversal/counting_eulerian_circuits.hpp
---

BEST 定理と有向グラフの行列木定理を用いて、有向多重グラフのオイラー閉路の個数を体 `T` 上で求める。辺はラベル付きとして区別し、巡回列の巡回シフトは同一視する。すなわち、入力の 0 番目の辺を先頭に固定したオイラー閉路の個数を返す。

## オイラー閉路の個数

```cpp
template<class T, size_t MAX_VERTEX = static_cast<size_t>(-1)>
T count_eulerian_circuits(
    int vertex_count,
    const vector<pair<int, int>>& directed_edges
)
```

いずれかの頂点で入次数と出次数が等しくない場合、または孤立していない頂点からなる部分が強連結でない場合は `T()` を返す。自己ループと多重辺を許し、孤立頂点は無視する。辺が空の場合は、先頭に固定する辺が存在しないため `T()` を返す。

`T` は行列木定理の実装が要求する体でなければならない。典型的には素数を法とする modint を使う。`MAX_VERTEX` を指定した場合、孤立していない頂点の個数は `MAX_VERTEX` 以下でなければならない。

## 時間計算量

$N$ を孤立頂点を含む頂点数、$A$ を孤立していない頂点の個数、$M$ を辺数とする。

- 固定容量版: $O(N + M + \texttt{MAX_VERTEX}^2 + A^3)$

次数の階乗の構築を含めて $O(N+M)$、固定容量の行列の初期化が $O(\texttt{MAX_VERTEX}^2)$、有向ラプラシアンの小行列式の計算が $O(A^3)$ である。

## 空間計算量

- 固定容量版: $O(N + M + \texttt{MAX_VERTEX}^2)$

## 容量を省略する可変長版

容量テンプレート引数を省略すると、実行時のサイズに合わせた `std::vector` を使う。容量を明示する固定長版も同じヘッダにある。

```cpp
auto value = count_eulerian_circuits<Mint>(vertex_count, edges);
auto fixed_value = count_eulerian_circuits<Mint, 1000>(vertex_count, edges);
```

可変長版は $O(N+M+A^3)$ 時間、$O(N+M+A^2)$ 空間である。

## 注意点

`vertex_count` が負、辺の端点が範囲外、または孤立していない頂点の個数が `MAX_VERTEX` を超える場合は `runtime_error` を送出する。
