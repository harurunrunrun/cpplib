---
title: Persistent Dynamic Li Chao Tree (完全永続動的Li Chao木)
documentation_of: ../../../../src/structure/convex_hull_trick/li_chao/persistent_dynamic_li_chao_tree.hpp
---

完全永続な動的 Li Chao Tree。整数座標の閉区間 $[\texttt{X_MIN},\texttt{X_MAX}]$ 上で、指定したバージョンに直線を追加した新しいバージョンを作る。バージョン 0 は直線のない状態である。

## テンプレート引数

```cpp
PersistentDynamicLiChaoTree<X_MIN, X_MAX, MAX_VERSION = -1>
```

- `X_MIN`, `X_MAX`: 座標の下限・上限（`long long`）
- `MAX_VERSION`: バージョン 0 から追加できるバージョン数の上限。`-1`（省略時）なら上限なしの可変長版

`MAX_VERSION` を指定すると、根の表と全更新分のノード（1 回の更新につき $D$ 個）をオブジェクト内に固定確保する。省略すると `std::vector` で必要に応じて確保する（伸長コストは償却され、各操作の計算量は変わらない）。

## add_line / fork

```cpp
int add_line(int version, long long a, long long b)
int add_line(int version, Line line)
int fork(int version)
```

`add_line` はバージョン `version` に直線 $ax+b$ を追加した新しいバージョンを作り、その番号を返す。`fork` は同じ状態を指す新しいバージョンを作る。

## query

```cpp
long long query(int version, long long x) const
```

バージョン `version` での $x$ における最小値を返す。

直線が 1 本もない場合は `PersistentDynamicLiChaoTree::INF` を返す。直線がある場合は、`INF` 以上の値も正しく返す。

$ax+b$ は `__int128` で評価し、`long long` の範囲外になった場合だけ上下限に丸める。

## その他

```cpp
bool empty(int version) const
int versions() const
int latest_version() const
int nodes_used() const
```

順に、指定したバージョンに直線がないかどうか、バージョン数、最後に作ったバージョンの番号、使用ノード数を返す。

## 時間計算量

$W=\texttt{X_MAX}-\texttt{X_MIN}+1$、$D=\lceil\log_2 W\rceil+1$ を整数区間の木の最大の深さとする。

- コンストラクタ: 固定長版は根の表とノード配列の初期化で $O(\texttt{MAX_VERSION}\cdot D)$、可変長版は $O(1)$
- `Line::eval128`, `Line::eval`, `versions`, `latest_version`, `nodes_used`, `empty`: $O(1)$
- `fork`: $O(1)$。ノードを増やさずバージョンだけを作る
- `add_line`（2 つのオーバーロードとも）: $O(D)$。新しいノードを $O(D)$ 個、新しいバージョンを 1 個作る
- `query`: $O(D)$

## 空間計算量

- 固定長版の根の表と全更新用のノード配列: $O((\texttt{MAX_VERSION}+1)D)$。可変長版は作ったノード数とバージョン数に比例
- 各操作の再帰スタック: `add_line` は $O(D)$、その他は $O(1)$

## 注意点

- コンパイル時に `X_MIN <= X_MAX`、`MAX_VERSION >= 0`（または `-1`）が必要である（`static_assert`）。
- バージョンは $[0,\texttt{versions()})$、`query` の座標は $[\texttt{X_MIN},\texttt{X_MAX}]$ になければならない。違反すると `std::runtime_error` を投げる。
- `add_line` と `fork` でバージョン数・ノード数の上限を超える場合は `std::runtime_error` を投げ、既存のバージョンと使用ノード数は変えない。
