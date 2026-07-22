---
title: Offline Rectangle Add Point Get (オフライン長方形加算一点取得) [rectangle_add_point_get]
documentation_of: ../src/algorithm/other/offline_query/offline_rectangle_add_point_get.hpp
---

長方形加算と一点取得からなる操作列を登録し、まとめて評価する。
更新長方形の全頂点収集、座標圧縮、構築、時系列 replay は `solve()` が行う。

## 初期長方形の登録

```cpp
void add_initial_rectangle(Coordinate x1, Coordinate y1,
                           Coordinate x2, Coordinate y2,
                           const T& value)
```

すべての操作より前に半開長方形 $[x1,x2)\times[y1,y2)$ へ `value` を加える更新を登録する。
各軸の端点が逆順なら入れ替えて扱い、空長方形は何も加えない。

## 長方形加算

```cpp
void add_rectangle(Coordinate x1, Coordinate y1,
                   Coordinate x2, Coordinate y2,
                   const T& value)
```

この呼出位置で行う長方形加算を操作列へ登録する。

## 一点取得の登録

```cpp
size_t get_point(Coordinate x, Coordinate y)
```

現在までの更新値の総和を点 $(x,y)$ で取得する操作を登録し、`solve()` の返り値における添字を返す。
取得点の事前登録は不要。

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

$R$ を初期更新と通常更新の総数、$Q$ を取得数とする。

| API | 時間計算量 |
| --- | --- |
| `add_initial_rectangle(...)` | 償却 $O(1)$ |
| `add_rectangle(...)` | 償却 $O(1)$ |
| `get_point(x,y)` | 償却 $O(1)$ |
| `query_count()` | $O(1)$ |
| `solve()` | $O((R+Q)\log^2(R+1))$ |

保持領域と `solve()` の追加空間は $O(R\log(R+1)+Q)$。

## 注意点

- `get_point` はその場で値を計算せず、取得を操作列へ登録する。
- 初期長方形は登録順に関係なく、すべて通常操作より前に適用される。
- `T` は零初期化、加算、減算、単項マイナスを提供する必要がある。
