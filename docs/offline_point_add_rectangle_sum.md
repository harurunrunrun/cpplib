---
title: Offline Point Add Rectangle Sum (オフライン点加算長方形和) [point_add_rectangle_sum]
documentation_of: ../src/algorithm/other/offline_query/offline_point_add_rectangle_sum.hpp
---

点加算と長方形和取得からなる操作列を登録し、まとめて評価する。
全更新点の収集、座標圧縮、構築、時系列 replay は `solve()` が行う。

## 初期点の登録

```cpp
void add_initial_point(Coordinate x, Coordinate y, const T& value)
```

すべての操作より前に点 $(x,y)$ へ `value` を加える更新を登録する。同じ点を複数回登録してよい。

## 点加算

```cpp
void add_point(Coordinate x, Coordinate y, const T& value)
```

この呼出位置で行う点加算を操作列へ登録する。

## 長方形和取得の登録

```cpp
size_t rectangle_sum(Coordinate x1, Coordinate y1,
                     Coordinate x2, Coordinate y2)
```

現在までの点重みについて半開長方形 $[x1,x2)\times[y1,y2)$ の和を取得する操作を登録し、`solve()` の返り値における添字を返す。
`x2 < x1` または `y2 < y1` は `invalid_argument`。幅または高さが0の長方形は許す。

## 評価

```cpp
vector<T> solve() const
```

初期更新を適用した後、登録順に更新・取得を処理し、取得結果を登録順に返す。
複数回呼び出しても登録内容は変化しない。

## その他

```cpp
size_t query_count() const
```

登録済みの取得数を返す。

## 時間計算量

$P$ を初期更新と通常更新の総数、$Q$ を取得数とする。

| API | 時間計算量 |
| --- | --- |
| `add_initial_point(...)` | 償却 $O(1)$ |
| `add_point(...)` | 償却 $O(1)$ |
| `rectangle_sum(...)` | 償却 $O(1)$ |
| `query_count()` | $O(1)$ |
| `solve()` | $O((P+Q)\log^2(P+1))$ |

保持領域と `solve()` の追加空間は $O(P\log(P+1)+Q)$。

## 注意点

- `rectangle_sum` はその場で値を計算せず、取得を操作列へ登録する。
- 初期点は登録順に関係なく、すべて通常操作より前に適用される。
- `T` は零初期化、加算、減算を提供する必要がある。
