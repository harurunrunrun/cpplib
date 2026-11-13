---
title: Incremental Component Sum (増分連結成分和)
documentation_of: ../../../../src/structure/dsu/specialized/incremental_component_sum.hpp
---

頂点への値の加算と無向辺の追加を処理し、各連結成分の頂点値の総和を返す。
辺は追加だけを行い、削除しない増分グラフを扱う。

## テンプレート引数

```cpp
IncrementalComponentSum<T, MAX_SIZE = -1>
```

- `T`: `T{}`、加算 `a + b`、代入が使える値の型
- `MAX_SIZE`: 頂点数の上限（正）。省略（`-1`）すると、実行時の頂点数に合わせて `std::vector` を確保する可変長版になる

```cpp
IncrementalComponentSum<long long> dsu(values);  // 可変長版
```

## コンストラクタ

```cpp
explicit IncrementalComponentSum(int n = MAX_SIZE);
explicit IncrementalComponentSum(const vector<T>& values);
```

1 つ目は $n$ 頂点を作り、各頂点の値を `T{}` で初期化する。可変長版では `n` のデフォルトは $0$ である。
2 つ目は `values[v]` を頂点 $v$ の初期値として作る。

## add_vertex

```cpp
void add_vertex(int vertex, const T& delta)
```

頂点 `vertex` の値に `delta` を加算する。`T` が表現できれば負の `delta` も使える。

## add_edge

```cpp
bool add_edge(int first, int second)
```

無向辺を追加し、2 頂点の成分が異なれば併合する。実際に併合した場合だけ `true` を返す。
自己ループや同じ成分内の辺では状態を変えず、`false` を返す。

## component_sum

```cpp
T component_sum(int vertex)
```

`vertex` を含む連結成分に属するすべての頂点の現在値の総和を返す。

## leader / same

```cpp
int leader(int vertex)
bool same(int first, int second)
```

`leader` は成分の代表元を、`same` は 2 頂点が同じ成分に属するかを返す。

## component_size / size(vertex)

```cpp
int component_size(int vertex)
int size(int vertex)
```

`vertex` を含む成分の頂点数を返す。2 つの関数は同じ値を返す。

## size() / groups()

```cpp
int size() const
int groups() const
```

引数なしの `size()` は全頂点数、`groups()` は現在の連結成分数を返す。

## 使用例

```cpp
IncrementalComponentSum<long long, 100> graph(
    vector<long long>{3, -1, 4}
);
graph.add_edge(0, 2);
graph.add_vertex(2, 5);
assert(graph.component_sum(0) == 12);
```

## 時間計算量

頂点数を $N$、逆アッカーマン関数を $\alpha$ とする。

| API | 時間計算量（償却） | 追加空間計算量 |
| --- | --- | --- |
| 各コンストラクタ | $O(N)$（固定長版は配列の初期化に $O(\texttt{MAX_SIZE})$） | $O(1)$ |
| `add_vertex`, `add_edge` | $O(\alpha(N))$ | $O(\log N)$ |
| `component_sum` | $O(\alpha(N))$ | $O(\log N)$ |
| `leader`, `same` | $O(\alpha(N))$ | $O(\log N)$ |
| `component_size`, `size(vertex)` | $O(\alpha(N))$ | $O(\log N)$ |
| `size()`, `groups()` | $O(1)$ | $O(1)$ |

追加空間は、代表探索の再帰スタックである。

## 空間計算量

固定長版は $O(\texttt{MAX_SIZE})$、可変長版は $O(N)$ の領域を持つ。

## 注意点

- $0 \le n \le \texttt{MAX_SIZE}$ でなければ、コンストラクタは `std::runtime_error` を送出する。
- 頂点引数はすべて $[0, n)$ でなければならない。範囲外なら `std::runtime_error` を送出する。
- 辺の削除、頂点値の代入、過去の状態への問い合わせは行えない。
- 成分和が型 `T` で表現できることは、呼び出し側の前提である。
