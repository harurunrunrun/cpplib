---
title: Offline Segment Add Get Min (オフライン線分追加一点最小値) [segment_add_get_min]
documentation_of: ../../../../src/structure/convex_hull_trick/li_chao/offline_segment_add_get_min.hpp
---

定義域付き一次関数の追加と一点最小値取得からなる操作列を登録し、まとめて評価する。内部で全取得座標を収集して `SegmentLiChaoTree` を構築するので、呼び出し側で座標圧縮や操作列の再生を行う必要はない。

## 初期線分の登録

```cpp
void add_initial_segment(long long left, long long right,
                         long long slope, long long intercept)
```

すべての操作より前から存在する一次関数 $f(x)=\mathrm{slope}\cdot x+\mathrm{intercept}$ を登録する。定義域は半開区間 $[\mathrm{left},\mathrm{right})$ である。`right < left` なら `std::invalid_argument` を投げる。空区間は許す。

## 線分追加

```cpp
void add_segment(long long left, long long right,
                 long long slope, long long intercept)
```

この呼び出し以降で有効な定義域付き一次関数を操作列に追加する。区間の扱いは初期線分と同じ。

## 最小値取得の登録

```cpp
size_t get_min(long long x)
```

その時点までに追加された線分のうち $x$ を定義域に含むものの最小値の取得を登録し、`solve()` の戻り値におけるその結果の添字を返す。該当する線分がない場合、その要素は `std::nullopt` になる。

## 評価

```cpp
std::vector<std::optional<long long>> solve() const
```

初期線分を適用した後、登録順に追加・取得を処理して、取得結果を登録順に返す。複数回呼び出しても登録内容は変化しない。一次関数の評価値が `long long` に収まらない場合は `std::overflow_error` を投げる。

## その他

```cpp
size_t query_count() const
```

登録済みの取得数を返す。

## 時間計算量

$I$ を初期線分数、$U$ を追加数、$Q$ を取得数、$K$ を異なる取得座標数とする。

| API | 時間計算量 |
| --- | --- |
| `add_initial_segment(...)` | 償却 $O(1)$ |
| `add_segment(...)` | 償却 $O(1)$ |
| `get_min(x)` | 償却 $O(1)$ |
| `query_count()` | $O(1)$ |
| `solve()` | $O(Q\log Q+(I+U)\log^2 K+Q\log K)$ |

`solve()` の追加空間は $O(I+U+Q+K)$。

## 注意点

- `get_min` はその場で値を計算せず、取得を操作列へ登録する。
- 初期線分は登録順に関係なく、すべて通常操作より前に適用される。
- 同じ取得座標や同じ線分を複数回登録してよい。
