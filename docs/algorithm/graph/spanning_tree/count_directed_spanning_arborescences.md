---
title: Directed Spanning Arborescence Count (有向全域有向木数) [DAGCNT]
documentation_of: ../../../../src/algorithm/graph/spanning_tree/count_directed_spanning_arborescences.hpp
---

重み付き有向グラフについて、根から全辺が外向きの全域有向木を、すべての根にわたって数える。各有向木の重みは採用した辺の重みの積であり、戻り値はその総和である。根が異なる有向木は別のものとして数える。

## 体上の重み付き隣接行列

```cpp
template <class Field>
Field count_directed_spanning_arborescences(
    const std::vector<std::vector<Field>>& adjacency
);
```

`adjacency[u][v]` を辺 $u \to v$ の重みとする。値が $0$ の辺は寄与せず、自己ループは無視する。結果を `Field` の値として返す。

`Field` は $0$ と $1$ を構築でき、等値比較、単項マイナス、加減乗除が定義された体でなければならない。組み込み整数型は除算が体の除算にならないため受け付けない（コンパイルエラーになる）。有限体には `Modint<MOD>` を使える。

## 整数重みの素数法

```cpp
template <int MOD, class Integer>
int count_directed_spanning_arborescences_mod(
    const std::vector<std::vector<Integer>>& adjacency
);
```

組み込み整数型 `Integer` の各重みを素数 `MOD` で割った余りに直し、総重みを `MOD` で割った余りとして返す。負の重みと符号なし整数型にも対応する。`MOD` が素数でない場合はコンパイルエラーになる。頂点数や重みの大きさに固定の上限はないが、必要なメモリに収まらなければならない。

## 二値隣接行列の厳密計数

```cpp
long long count_directed_spanning_arborescences(
    const std::vector<std::string>& adjacency
);
```

`adjacency[u][v] == '1'` を辺 $u \to v$、`'0'` を辺なしとして、総数を整数で厳密に返す。既存コードとの互換用の API であり、頂点数は 1 以上 8 以下でなければならない。`'0'`, `'1'` 以外の文字を含む場合は `runtime_error` を送出する。答えが `long long` の範囲を超える場合は `overflow_error` を送出する。

## 計算量

頂点数を $N$ とする。

| API | 時間計算量 | 追加空間計算量 |
| --- | --- | --- |
| 体上の `count_directed_spanning_arborescences` | $O(N^3)$ | $O(N^2)$ |
| `count_directed_spanning_arborescences_mod` | $O(N^3 + N \log \mathrm{MOD})$ | $O(N^2)$ |
| 二値隣接行列版 `count_directed_spanning_arborescences` | $O(N^3)$ | $O(N^2)$ |

素数法版の $N \log \mathrm{MOD}$ は、ピボットの逆元を各列で 1 回求める費用である。体上の演算を $O(1)$ と数える場合は $O(N^3)$ となる。

## 注意点

- すべての API で、隣接行列は空でない正方行列でなければならない。違反時は `runtime_error` を送出する。
- 自己ループは全域有向木に使えないため、対角成分は結果に影響しない。
- 負の重みも代数的には扱える。この場合の結果は個数ではなく辺重み積の符号付き総和である。
- 有向ラプラシアンを $L$ とすると、行列木定理とランク 1 更新により $\det(L + \boldsymbol{1} e_0^{\mathsf T})$ がすべての根についての総重みに等しい。頂点数で割らないため、`MOD` が頂点数を割り切る場合も正しく計算できる。
- 体上の API は浮動小数点型でも実体化できるが、ピボットの判定と消去に丸め誤差が入る。厳密に数えるには有限体を使う。
- 二値隣接行列版は、符号付き 128 ビット整数上の Bareiss 法で行列式を厳密に計算する。
