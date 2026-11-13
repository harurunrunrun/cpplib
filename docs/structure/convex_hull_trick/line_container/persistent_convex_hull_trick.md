---
title: Persistent Convex Hull Trick (完全永続凸包トリック)
documentation_of: ../../../../src/structure/convex_hull_trick/line_container/persistent_convex_hull_trick.hpp
---

直線の追加が完全永続な Convex Hull Trick。各バージョンの凸包について、ある $x$ における最小値を返す。

各バージョンで追加する直線の傾きは、そのバージョンの最後の直線の傾き以下（単調非増加）でなければならない。
凸包は永続セグメント木で管理し、直線の追加ごとに新しいバージョンを作る。過去のバージョンは変更しない。

## テンプレート引数

```cpp
PersistentConvexHullTrick<MAX_LINE = -1, MAX_VERSION = -1, MAX_NODE = -1>
```

- `MAX_LINE`: 1 つの凸包に保持する直線数の上限
- `MAX_VERSION`: バージョン数の上限（初期バージョン $0$ を含む）
- `MAX_NODE`: 永続セグメント木のノード数の上限（番兵ノード $1$ 個を含む）

いずれも省略（`-1`）すると `std::vector` による可変長になる。可変長の `MAX_LINE` では、各バージョンの索引木は必要になったときだけ範囲を倍に広げ、古い根を左部分木とする新しい根を作る。

```cpp
using Variable = PersistentConvexHullTrick<>;
using Fixed = PersistentConvexHullTrick<100000, 200001, 4000000>;
```

## メンバ関数

### コンストラクタ

```cpp
PersistentConvexHullTrick()
```

空の凸包を持つバージョン $0$ だけがある状態で初期化する。

### fork, versions

```cpp
int fork(int version)
int versions() const
```

`fork` は `version` と同じ凸包を持つ新しいバージョンを作り、その番号を返す。ノードは消費しない。
`versions` は作成済みのバージョン数を返す。

### add_line

```cpp
int add_line(int version, long long a, long long b)
int add_line(int version, Line line)
```

`version` の凸包に直線 $y = ax + b$ を追加した新しいバージョンを作り、その番号を返す。
末尾と同じ傾きで切片が劣る直線を追加した場合は凸包を変えず、`fork(version)` と同じ結果になる。

### query

```cpp
long long query(int version, long long x) const
```

`version` の凸包の $x$ における最小値を返す。

### 状態参照

```cpp
int nodes_used() const
int size(int version) const
bool empty(int version) const
Line line_at(int version, int index) const
```

- `nodes_used`: 使用済みのノード数（番兵を含む）
- `size`: `version` の凸包上の直線数
- `empty`: `version` の凸包が空かどうか
- `line_at`: `version` の凸包の `index` 番目（傾きの降順）の直線

### Line

```cpp
struct Line{ long long a, b; };
__int128 Line::eval128(long long x) const
long long Line::eval(long long x) const
```

`eval128` は $ax + b$ を `__int128` で返し、`eval` はそれを `long long` の範囲に丸めて返す。

## 時間計算量

$N$ を対象バージョンの凸包の直線数、$K$ を `add_line` で末尾から除去される直線数、$L$ を索引木の範囲（固定長版では $\texttt{MAX_LINE}$、可変長版では祖先バージョンを含む最大の凸包長程度）とする。

- コンストラクタ: 固定長版は配列の初期化に $O(\texttt{MAX_NODE}+\texttt{MAX_VERSION})$、可変長版は $O(1)$
- `Line::eval128`、`Line::eval`、`versions`、`nodes_used`、`size`、`empty`、`fork`: $O(1)$
- `line_at`: $O(\log(L+1))$
- `add_line`: 最悪 $O((K+1)\log(L+1))$。直線を保存する場合は $O(\log(L+1))$ 個のノードと 1 個のバージョンを生成する
- `query`: $O(\log(N+1)\log(L+1))$

`query` は凸包上の添字を二分探索し、各比較で永続セグメント木から直線を取得する。
古いバージョンから分岐すると同じ直線を何度も除去できるため、分岐をまたいで $K$ を償却することはできない。
直前のバージョンだけを更新する一本道の使い方では、`add_line` は償却 $O(\log(L+1))$ となる。

## 空間計算量

- 固定長版: $O(\texttt{MAX_NODE}+\texttt{MAX_VERSION})$
- 可変長版: 作成済みのノード数とバージョン数に比例する
- 直線を保存する追加 1 回あたりの新規ノード: $O(\log(L+1))$

## 注意点

- 傾きの単調性に違反する `add_line` は `std::runtime_error` を送出する。
- バージョン番号は $[0, \mathrm{versions}())$、`line_at` の `index` は $[0, \mathrm{size}(\mathrm{version}))$ でなければならない。範囲外は `std::runtime_error` を送出する。
- 空のバージョンへの `query` は `std::runtime_error` を送出する。
- 固定長版で直線数・ノード数・バージョン数の容量を超えると `std::runtime_error` を送出する。可変長版で `int` の表現上限を超える要素数も例外となる。
- 例外で失敗した更新は、既存のバージョンと使用済みノード数を変えない。
- 比較は `__int128` で行い、戻り値だけを `long long` の範囲に丸める。
