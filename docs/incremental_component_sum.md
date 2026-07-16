---
title: Incremental Component Sum (増分連結成分和)
documentation_of: ../src/structure/dsu/incremental_component_sum.hpp
---

頂点への値の加算と無向辺の追加を処理し、各連結成分の頂点値総和を返す。
辺は追加だけを行い、削除しない増分グラフを扱う。

## テンプレート引数

```cpp
IncrementalComponentSum<T, MAX_SIZE>
```

- `T` は `T{}`、加算 `a + b`、代入を利用できる値型
- `MAX_SIZE` は頂点数の上限で、正でなければならない

## コンストラクタ

### `IncrementalComponentSum(n)`

```cpp
explicit IncrementalComponentSum(int n = MAX_SIZE);
```

`n` 頂点を作り、各頂点の値を `T{}` で初期化する。

### `IncrementalComponentSum(values)`

```cpp
explicit IncrementalComponentSum(const vector<T>& values);
```

`values[v]` を頂点 `v` の初期値として作る。

## `add_vertex(vertex, delta)`

頂点 `vertex` の値に `delta` を加算する。負の `delta` も `T` が表現できれば使用できる。

## `add_edge(first, second)`

無向辺を追加し、2頂点の成分が異なれば併合する。実際に併合した場合だけ `true` を返す。
自己ループや同じ成分内の辺では状態を変えず `false` を返す。

## `component_sum(vertex)`

`vertex` を含む連結成分に属する全頂点の現在値の総和を返す。

## `leader(vertex)` / `same(first, second)`

`leader` は成分の代表元、`same` は2頂点が同じ成分かを返す。

## `component_size(vertex)` / `size(vertex)`

`vertex` を含む成分の頂点数を返す。2つの API は同じ値を返す。

## `size()` / `groups()`

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

## API別の時間計算量・空間計算量

頂点数を $N$、逆Ackermann関数を $\alpha$ とする。

| API | 償却時間計算量 | 追加空間計算量 |
| --- | --- | --- |
| 各コンストラクタ | $O(MAX_SIZE)$ | $O(1)$ |
| `add_vertex`, `add_edge` | $O(\alpha(N))$ | $O(\log N)$ |
| `component_sum` | $O(\alpha(N))$ | $O(\log N)$ |
| `leader`, `same` | $O(\alpha(N))$ | $O(\log N)$ |
| `component_size`, `size(vertex)` | $O(\alpha(N))$ | $O(\log N)$ |
| `size()`, `groups()` | $O(1)$ | $O(1)$ |

本体は固定配列として $O(\mathtt{MAX\_SIZE})$ の領域を持つ。追加空間は代表探索の再帰スタックを含む。

## 注意点

- `0 <= n <= MAX_SIZE` でなければコンストラクタは `runtime_error` を送出する。
- 頂点引数はすべて `[0,n)`。範囲外なら `runtime_error` を送出する。
- 辺の削除、頂点値の代入、過去状態への問い合わせは行えない。
- 成分和が型 `T` で表現できることは呼び出し側の前提である。

