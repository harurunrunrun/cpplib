---
title: Segment Li Chao Tree (線分Li Chao木)
documentation_of: ../../../../src/structure/convex_hull_trick/li_chao/segment_li_chao_tree.hpp
---

追加した直線または定義域付きの線分のうち、指定した $x$ で最小となる値を求める。クエリ座標を構築時に座標圧縮するので、計算量は座標の大きさに依存しない。

## 構築

```cpp
explicit SegmentLiChaoTree(std::vector<long long> query_coordinates)
```

クエリに使う全 $x$ 座標を渡す。重複や未整列の入力を許す。以下、$K$ を相異なるクエリ座標の個数とする。

## Line

```cpp
struct Line {
    long long slope;
    long long intercept;
    long long eval(long long x) const;
};
```

直線 `slope * x + intercept` を表す。結果が `long long` に収まらない場合、`eval` は `std::overflow_error` を投げる。

## 直線追加

```cpp
void add_line(long long slope, long long intercept)
void add_line(Line line)
```

全クエリ座標で有効な直線を追加する。

## 線分追加

```cpp
void add_segment(long long l, long long r, long long slope, long long intercept)
void add_segment(long long l, long long r, Line line)
```

半開区間 $[l,r)$ 内の登録済みクエリ座標で有効な直線を追加する。`l == r`、または区間内にクエリ座標がない場合は何もしない。

## 最小値クエリ

```cpp
std::optional<long long> query(long long x) const
```

$x$ での最小値を返す。有効な直線がなければ `std::nullopt` を返す。

## その他

```cpp
int size() const
bool empty() const
const std::vector<long long>& coordinates() const
```

`size` は $K$、`empty` は有効な追加が一度もないかどうか、`coordinates` は整列・重複除去したクエリ座標の列を返す。

## 時間計算量

- 構築: $O(K\log K)$
- `add_line`: $O(\log K)$
- `add_segment`: $O(\log^2 K)$
- `query`: $O(\log K)$
- `size`, `empty`, `coordinates`: $O(1)$

## 空間計算量

- $O(K)$

## 注意点

- `query` の座標はコンストラクタで与えた座標のいずれかでなければならない。未登録の座標は `std::runtime_error` を投げる。
- 線分の有効範囲は座標値の半開区間 $[l,r)$ である。`r < l` は `std::runtime_error` を投げる。
- 一次関数の評価値が `long long` に収まらない場合は `std::overflow_error` を投げる。
- 追加時の再帰スタックは $O(\log K)$、`query` の追加領域は $O(1)$。
