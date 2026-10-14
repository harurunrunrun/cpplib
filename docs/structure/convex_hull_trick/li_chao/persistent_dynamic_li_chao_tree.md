---
title: Persistent Dynamic Li Chao Tree (完全永続動的Li Chao木)
documentation_of: ../../../../src/structure/convex_hull_trick/li_chao/persistent_dynamic_li_chao_tree.hpp
---

完全永続 Dynamic Li Chao Tree。

整数座標の閉区間 `[X_MIN, X_MAX]` 上で、指定したバージョンに直線を追加した新しいバージョンを作る。

## テンプレート引数

```cpp
PersistentDynamicLiChaoTree<X_MIN, X_MAX, MAX_VERSION>
```

- 座標の下限 `X_MIN`
- 座標の上限 `X_MAX`
- 追加できるバージョン数の上限 `MAX_VERSION`

## add_line / fork

```cpp
int add_line(int version, long long a, long long b)
int add_line(int version, Line line)
int fork(int version)
```

`add_line` は `version` に直線 `a x + b` を追加した新しいバージョン番号を返す。
`fork` は同じ状態を指す新しいバージョンを作る。

## 時間計算量

- `add_line`: $O(\log (X\_MAX - X\_MIN + 1))$
- `fork`: $O(1)$

## query

```cpp
long long query(int version, long long x)
```

指定したバージョンでの `x` における最小値を返す。

空の場合は `PersistentDynamicLiChaoTree::INF` を返す。非空の場合の値は `INF` 以上でも扱える。

`a * x + b` は `__int128` で評価し、`long long` の範囲外だけを上下限へ丸める。node、version の容量例外が発生した場合、既存 version と使用済み node 数は変化しない。

## 時間計算量

- $O(\log (X\_MAX - X\_MIN + 1))$

## その他

```cpp
bool empty(int version)
int versions()
int latest_version()
int nodes_used()
```

## 時間計算量

- $O(1)$

## API別計算量

$W=X\_MAX-X\_MIN+1$、$D=\lceil\log_2 W\rceil+1$ を整数区間treeの最大level数とする。

- `PersistentDynamicLiChaoTree()`: root table初期化 $O(\mathrm{MAX\_VERSION})$
- `Line::eval128`, `Line::eval`: $O(1)$
- `versions`, `latest_version`, `nodes_used`, `empty`: $O(1)$
- `fork`: $O(1)$。nodeを増やさずversionだけを生成する
- 2つの `add_line` overload: $O(D)$、新規node $O(D)$、新規version $O(1)$
- `query`: $O(D)$

## 空間計算量

- root tableと全update用固定arena: $O((\mathrm{MAX\_VERSION}+1)D)$
- 各操作の再帰stack: `add_line` は $O(D)$、その他は $O(1)$

## 注意点

- compile時に`X_MIN <= X_MAX`, `MAX_VERSION >= 0`が必要。
- versionは`[0,versions())`、query座標は`[X_MIN,X_MAX]`。違反時は例外。
- `add_line`と`fork`でversion/node容量を超える場合は例外で、既存状態を変えない。
- `latest_version`は最後のversion番号、`nodes_used`は使用node数を返す。
- `empty(version)`は指定versionに直線がないかを返す。

## 容量を省略した可変長版

容量テンプレートを省略すると既定値 `-1` でvectorを使い、必要な保存領域を明示的に伸長する。
固定容量を指定する既存版も同じヘッダに残る。

```cpp
using Variable = PersistentDynamicLiChaoTree<X_MIN, X_MAX>;
using Fixed = PersistentDynamicLiChaoTree<X_MIN, X_MAX, Versions>;
```

伸長の費用は追加操作全体で償却され、上記の各操作の計算量を維持する。
領域は確保済みノード・履歴・heap要素などの個数に比例する。
整数添字の表現上限を超える要素数は例外。構造上の文字種類・bit幅・座標域は、件数容量とは別に指定できる。
