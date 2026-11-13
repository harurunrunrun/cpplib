---
title: Tree Mo (木上のMo)
documentation_of: ../../../../src/algorithm/tree/decomposition/tree_mo.hpp
---

静的な無向木の頂点パスクエリを、Euler tour 上の Mo のアルゴリズムに変換してオフラインで処理する。
パス上の相異なる値の個数、値の出現回数、可換な重みの和など、頂点の追加と削除で更新できる情報に使える。

## 型と構築

```cpp
TreeMo<> mo(n);              // 可変長版
// TreeMo<100000> mo(n);     // 固定容量版
mo.add_edge(u, v);
mo.build(root);
```

テンプレート引数 `MAX_SIZE` は頂点の容量である。省略する（`-1` にする）と、実行時のサイズに合わせて `std::vector` を使う。
`n` は実際の頂点数で、`0 <= n`（固定容量版では `n <= MAX_SIZE` も）を満たす必要がある。
`add_edge(u, v)` で 0-indexed の無向辺をちょうど $n-1$ 本追加し、`build(root)` で Euler tour とダブリングの表を反復的な DFS により構築する。
再帰を使わないため、パス状の深い木でもコールスタックに依存しない。

$n = 0$ では辺なしの `build()` が成功し、`root() == -1` になる。
`TreeMo` のコピー構築・コピー代入は削除されている。ムーブ構築・ムーブ代入は定数時間で、ムーブ後の移動元のオブジェクトは再利用しない。

## メタデータ

```cpp
mo.size();
mo.edge_count();
mo.root();
mo.parent(v);
mo.depth(v);
mo.lca(u, v);
```

- `size()` は実際の頂点数を返す。
- `edge_count()` は追加済みの無向辺の本数を返す。
- `root()` は直近の `build` で指定した根を返す。
- `parent(v)` は根付き木での親を返す。根では `-1`。
- `depth(v)` は根からの辺数を返す。
- `lca(u, v)` は指定した根に関する最小共通祖先を返す。

`root`, `parent`, `depth`, `lca` は `build` が成功した後にだけ呼べる。

## クエリの登録

```cpp
mo.reserve_queries(count);
int id = mo.add_query(u, v);
mo.query_count();
mo.clear_queries();
```

- `reserve_queries(count)` はクエリの格納領域を予約する。大量のクエリを登録する場合の再確保を避けられる。
- `add_query(u, v)` は端点を含む単純パス `u-v` を登録し、0 から始まるクエリ ID を返す。
- `query_count()` は現在登録されているクエリ数を返す。
- `clear_queries()` はすべてのクエリを削除するが、確保済みの容量と構築済みの木は保持する。

`add_query` は `build` が成功した後にだけ呼べる。

## solve

```cpp
mo.solve(add, erase, answer);
mo.solve(add, erase, answer, block_size);
```

コールバックのシグネチャは次のとおりである。

```cpp
void add(int vertex);
void erase(int vertex);
void answer(int query_id);
```

コールバック側で管理する状態は、呼び出し前に空でなければならない。
`answer(id)` の時点で、`add` 済みかつ `erase` されていない頂点の集合は、`add_query` で登録した対応するパスの頂点集合と正確に一致する。
`add` と `erase` は、頂点の順序に依存しない集合の情報について互いに逆操作でなければならない。

すべてのクエリを処理した後、アクティブなすべての頂点に `erase` を呼ぶため、コールバック側の状態は呼び出し前と同じ空の状態に戻る。
登録したクエリは変更されず、同じオブジェクトで `solve` を繰り返し呼べる。

`block_size > 0` では指定した幅を使う。
`block_size <= 0` では、Euler 列の長さ $2n$ とクエリ数 $Q$ から $\max(1, \lfloor 2n / \sqrt{Q} \rfloor)$ を自動で選ぶ。
クエリが 0 件ならコールバックを呼ばずに終了する。

## API 別の時間計算量・空間計算量

$n$ を実際の頂点数、$M = 2n$ を Euler 列の長さ、$Q$ をクエリ数、$B$ をブロックの幅とする。
$A$, $E$, $R$ をそれぞれ `add`, `erase`, `answer` の 1 回あたりの時間計算量とする。
$C$ を容量（固定容量版では $\texttt{MAX_SIZE}$、可変長版では $n$）とする。

| API | 時間計算量 |
| --- | --- |
| `TreeMo(n)` | $O(C \log(C + 1))$ |
| ムーブ構築・ムーブ代入 | $O(1)$ |
| `size()` / `edge_count()` / `query_count()` | $O(1)$ |
| `reserve_queries(count)` | 再確保時 $O(Q)$、再確保がなければ $O(1)$ |
| `add_edge(u, v)` | $O(1)$ |
| `build(root)` | $O(n \log(C + 1))$ |
| `root()` / `parent(v)` / `depth(v)` | $O(1)$ |
| `lca(u, v)` | $O(\log(C + 1))$ |
| `add_query(u, v)` | 償却 $O(\log(C + 1))$ |
| `clear_queries()` | $O(Q)$ |
| `solve(..., B)` | $O(Q \log Q + (QB + M^2/B + M)(A+E+1) + QR + n(E+1))$ |
| `solve(...)`（自動幅） | $O(Q \log Q + M \sqrt{Q}(A+E+1) + QR + n(E+1))$ |

`answer` は各クエリについてちょうど 1 回呼ばれる。
祖先・子孫の関係にない 2 頂点間のパスでは LCA を一時的に追加・削除するため、上表の移動回数とは別に、最大 $Q$ 回ずつ `add` と `erase` が呼ばれる。

オブジェクトは木・Euler 列・ダブリングの表とクエリ列を保持し、$O(C \log(C+1) + Q)$ の領域を使う。
`solve` の追加領域は、クエリの順序とアクティブフラグの $O(n+Q)$ である。

## 注意点

- 頂点番号は $[0, n)$ である。空木を除き、入力は自己ループ・多重辺のない無向連結木でなければならない。
- コンストラクタの容量違反、端点・根・クエリ予約数の範囲違反、辺の容量超過、辺数の違反、閉路、非連結な入力、未構築でのメタデータ・クエリ・`solve` の呼び出しでは `std::runtime_error` を送出する。表のサイズが `int` の範囲を超える場合やクエリ数が多すぎる場合は `std::length_error` を送出する。
- `build` の後の `add_edge` は許可されない。
- コールバックが例外を送出した場合、その例外は呼び出し元へ伝播する。この場合、途中まで更新されたコールバック側の状態の後始末は保証しない。
