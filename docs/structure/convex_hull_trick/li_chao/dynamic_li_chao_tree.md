---
title: Dynamic Li Chao Tree (動的Li Chao木) [line_add_get_min]
documentation_of: ../../../../src/structure/convex_hull_trick/li_chao/dynamic_li_chao_tree.hpp
---

動的 Li Chao Tree。整数座標の閉区間 $[\texttt{X_MIN},\texttt{X_MAX}]$ 上で直線を追加し、各点での最小値を求める。ノードは必要になった区間にだけ作る。

## テンプレート引数

```cpp
DynamicLiChaoTree<X_MIN, X_MAX, MAX_NODE = -1>
```

- `X_MIN`, `X_MAX`: 座標の下限・上限（`long long`）
- `MAX_NODE`: ノード数の上限。`-1`（省略時）なら上限なしの可変長版

`MAX_NODE` を指定するとノードを `std::array` で固定確保し、省略するとノードを `std::vector` に必要に応じて確保する（伸長コストは償却され、各操作の計算量は変わらない）。直線の追加だけを行うので、必要なノード数は追加した直線の本数以下である。

## add_line

```cpp
void add_line(long long a, long long b)
void add_line(Line line)
```

直線 $ax+b$ を追加する。`Line` は `long long a, b` をメンバに持つ構造体である。

## query

```cpp
long long query(long long x) const
```

追加された直線の $x$ における最小値を返す。

直線が 1 本もない場合は `DynamicLiChaoTree::INF`（`std::numeric_limits<long long>::max() / 4`）を返す。直線がある場合は、`INF` 以上の値も正しく返す。

$ax+b$ は `__int128` で評価し、`long long` の範囲外になった場合だけ上下限に丸める（`Line::eval`）。`Line::eval128` は丸めない値を返す。

## その他

```cpp
bool empty() const
int nodes_used() const
```

直線が 0 本かどうかと、使用しているノード数を返す。

## 時間計算量

$W=\texttt{X_MAX}-\texttt{X_MIN}+1$、$D=\lceil\log_2 W\rceil+1$ を整数区間の木の最大の深さとする。

- コンストラクタ: 固定長版は $O(\texttt{MAX_NODE})$（配列の初期化）、可変長版は $O(1)$
- `nodes_used`, `empty`, `Line::eval128`, `Line::eval`: $O(1)$
- `add_line`（2 つのオーバーロードとも）: $O(D)$。1 回の追加で新しいノードは高々 1 個
- `query`: $O(D)$

## 空間計算量

- 固定長版のノード配列: $O(\texttt{MAX_NODE})$。可変長版は使用ノード数に比例
- 各操作の再帰スタック: $O(D)$

## 注意点

- コンパイル時に `X_MIN <= X_MAX`、`MAX_NODE >= 0`（または `-1`）が必要である（`static_assert`）。
- `query(x)` は `X_MIN <= x <= X_MAX` を要求し、範囲外なら `std::runtime_error` を投げる。
- ノード数の上限を超える `add_line` は `std::runtime_error` を投げ、既存の木と使用ノード数は変えない。
