---
title: Segment Li Chao Tree (線分Li Chao木)
documentation_of: ../src/structure/convex_hull_trick/segment_li_chao_tree.hpp
---

追加した直線または定義域付き線分のうち、指定した$x$で最小となる値を求める。query座標を構築時に圧縮するため、座標の大きさには依存しない。

# 構築

```cpp
SegmentLiChaoTree tree(vector<long long> query_coordinates)
```

queryに使う全$x$座標を渡す。重複や未整列の入力を許す。

## 時間計算量

- $O(K\log K)$

$K$は異なるquery座標数。

# Line

```cpp
struct Line {
    long long slope;
    long long intercept;
    long long eval(long long x) const;
};
```

`slope * x + intercept`を表す。結果が`long long`に収まらない場合、`eval`は例外を送出する。

## 直線追加

```cpp
void add_line(long long slope, long long intercept)
void add_line(Line line)
```

全query座標で有効な直線を追加する。

## 時間計算量

- $O(\log K)$

# 線分追加

```cpp
void add_segment(long long l, long long r, long long slope, long long intercept)
void add_segment(long long l, long long r, Line line)
```

半開区間$[l,r)$内の登録済みquery座標で有効な直線を追加する。`l == r`、または区間内にquery座標がない場合は何もしない。`r < l`は例外。

## 時間計算量

- $O(\log^2 K)$

# 最小値query

```cpp
optional<long long> query(long long x) const
```

`x`での最小値を返す。有効な直線がなければ`nullopt`。構築時に登録していない座標は例外を送出する。

## 時間計算量

- $O(\log K)$

# その他

```cpp
int size() const
bool empty() const
const vector<long long>& coordinates() const
```

すべて$O(1)$。`empty`は有効な追加が一度もないかを返す。

# 空間計算量

- $O(K)$

## 注意点

- query座標はconstructorで与えた離散座標のいずれかでなければならない。未登録座標は `runtime_error`。
- 線分の有効範囲は座標値の半開区間 `[left,right)`。`right < left` は `runtime_error`。
- 有効な直線がない点の `query` は `nullopt` を返す。
- 一次関数の評価値が `long long` に収まらない場合は `overflow_error`。
- 追加時の再帰stackは $O(\log K)$、queryの追加領域は $O(1)$。
