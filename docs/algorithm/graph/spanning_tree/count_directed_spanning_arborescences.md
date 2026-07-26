---
title: Directed Spanning Arborescence Count (有向全域有向木数) [DAGCNT]
documentation_of: ../../../../src/algorithm/graph/spanning_tree/count_directed_spanning_arborescences.hpp
---

重み付き有向グラフについて、根から全辺が外向きの全域有向木を全ての根にわたって数える。
各有向木の重みは採用した辺の重みの積であり、戻り値はその総和である。根が異なる有向木も別に数える。

## 体上の重み付き隣接行列

```cpp
template<class Field>
Field count_directed_spanning_arborescences(
    const vector<vector<Field>>& adjacency
);
```

`adjacency[u][v]` を辺 $u\to v$ の重みとする。値が0の辺は寄与せず、自己ループは無視する。
結果を `Field` 上で返す。

`Field` は0と1を構築でき、比較、単項マイナス、加減乗除が定義された体でなければならない。
組み込み整数型は除算が体の除算にならないため受け付けない。有限体では `Modint<MOD>` を利用できる。

## 整数重みの素数法

```cpp
template<int MOD, class Integer>
int count_directed_spanning_arborescences_mod(
    const vector<vector<Integer>>& adjacency
);
```

組み込み整数型 `Integer` の各重みを素数 `MOD` で剰余化し、総重みを `MOD` で割った余りとして返す。
負の重みと符号なし整数にも対応する。`MOD` が素数でない場合はコンパイル時エラーになる。
頂点数と重みの大きさによる固定上限はないが、必要なメモリに収まらなければならない。

## 二値隣接行列の厳密計数

```cpp
long long count_directed_spanning_arborescences(
    const vector<string>& adjacency
);
```

`adjacency[u][v] == '1'` を辺 $u\to v$、`'0'` を辺なしとして、総数を整数で厳密に返す。
既存コードとの互換用APIであり、頂点数は1以上8以下でなければならない。
答えが `long long` を越える場合は `overflow_error` を送出する。

## API別の時間計算量・空間計算量

頂点数を $N$ とする。

| API | 時間計算量 | 追加空間計算量 |
| --- | --- | --- |
| 体上の `count_directed_spanning_arborescences` | $O(N^3)$ | $O(N^2)$ |
| `count_directed_spanning_arborescences_mod` | $O(N^3+N\log MOD)$ | $O(N^2)$ |
| 二値隣接行列版 `count_directed_spanning_arborescences` | $O(N^3)$ | $O(N^2)$ |

素数法の $N\log MOD$ はpivotの逆元を各列で1回求める費用である。体上の演算を $O(1)$ と数える場合は $O(N^3)$ となる。

## 注意点

- 全APIで隣接行列は空でない正方行列でなければならず、違反時は `runtime_error` を送出する。
- 自己ループは全域有向木に使えないため、対角成分は結果に影響しない。
- 負の重みも代数的には扱える。この場合の結果は個数ではなく辺重み積の符号付き総和である。
- 有向ラプラシアンを $L$ とすると、行列木定理とrank-one更新により
  $\det(L+\boldsymbol{1}e_0^{\mathsf T})$ が全ての根の総重みに等しい。
  頂点数で割らないため、`MOD` が頂点数を割り切る場合も正しく計算できる。
- 体上のAPIは浮動小数点型でも実体化できるが、pivot判定と消去に丸め誤差が入る。厳密な計数には有限体を使う。
- 二値隣接行列版は符号付き128 bitのBareiss法で行列式を厳密に計算する。
