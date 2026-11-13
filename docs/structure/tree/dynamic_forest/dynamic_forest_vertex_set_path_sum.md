---
title: Dynamic Forest Vertex Set Path Sum (動的森の頂点代入パス和) [OTOCI]
documentation_of: ../../../../src/structure/tree/dynamic_forest/dynamic_forest_vertex_set_path_sum.hpp
---

`long long` の頂点値を持つ動的森に対し、辺の追加・削除、頂点値の代入、連結判定、パス和を扱う。
内部では `LinkCutTree` を使う。`MAX_SIZE` は省略でき、デフォルト値 `-1` では可変長版になる。

## `DynamicForestVertexSetPathSum<MAX_SIZE>`

### コンストラクタ

```cpp
explicit DynamicForestVertexSetPathSum(
    const vector<long long>& values
);
```

`values` を初期頂点値とする孤立頂点の森を作る。

### `size()`

頂点数を返す。

### `connected(left, right)`

2 頂点が同じ木に属するかを返す。

### `link(left, right)`

2 頂点が別の木なら辺を追加して `true` を返す。
既に連結なら変更せず `false` を返す。

### `cut(left, right)`

2 頂点を直接結ぶ辺があれば削除して `true` を、なければ `false` を返す。

### `set(vertex, value)`

1 頂点の値を `value` に代入する。

### `get(vertex)`

1 頂点の現在値を返す。

### `path_sum(left, right)`

連結なら両端を含む単純パス上の頂点値の総和を、非連結なら `nullopt` を返す。

## API 別の時間計算量・空間計算量

頂点数を $N$ とする。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| コンストラクタ | 固定長版 $O(\texttt{MAX_SIZE})$、可変長版 $O(N)$ | 固定長版 $O(\texttt{MAX_SIZE})$、可変長版 $O(N)$ |
| `size` | $O(1)$ | $O(1)$ |
| `connected` | ならし $O(\log N)$ | $O(1)$ |
| `link` | ならし $O(\log N)$ | $O(1)$ |
| `cut` | ならし $O(\log N)$ | $O(1)$ |
| `set` | ならし $O(\log N)$ | $O(1)$ |
| `get` | ならし $O(\log N)$ | $O(1)$ |
| `path_sum` | ならし $O(\log N)$ | $O(1)$ |

## 注意点

- `values.size() <= MAX_SIZE` かつ頂点番号が $[0,N)$ でなければならない。
- `link` は閉路を作らず、`cut` は直接の辺だけを削除する。
- パス和と頂点値は `long long` で計算される。オーバーフローしないことを呼び出し側が保証する。
- 範囲違反時は `runtime_error` を送出する。
- 容量を指定した固定配列版（`DynamicForestVertexSetPathSum<N>`）と可変長版（`DynamicForestVertexSetPathSum<>`）は同じヘッダで利用できる。
