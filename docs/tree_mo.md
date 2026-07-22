---
title: Tree Mo (木上のMo)
documentation_of: ../src/algorithm/tree/decomposition/tree_mo.hpp
---

静的な無向木の頂点path queryを、Euler tour上のMoのアルゴリズムへ変換してoffline処理する。
path上の相異なる値の個数、値の出現回数、可換な重み和など、頂点の追加と削除で更新できる情報に使える。

## 型と構築

```cpp
TreeMo<MAX_SIZE> mo(n);
mo.add_edge(u, v);
mo.build(root);
```

`MAX_SIZE` は頂点容量、`n` は実頂点数で、`0 <= n <= MAX_SIZE`。
`add_edge(u,v)` で0-indexedの無向辺をちょうど `n-1` 本追加し、`build(root)` でEuler tourとbinary lifting tableを反復DFSにより構築する。
再帰を使わないため、path状の深い木でもcall stackに依存しない。

`n == 0` では辺なしの `build()` が成功し、`root() == -1` になる。
`TreeMo` のcopy構築・copy代入は削除されている。move構築・move代入は定数時間で、move後の移動元objectは再利用しない。


## metadata

```cpp
mo.size();
mo.edge_count();
mo.root();
mo.parent(v);
mo.depth(v);
mo.lca(u, v);
```

- `size()` は実頂点数を返す。
- `edge_count()` は追加済みの無向辺数を返す。
- `root()` は直近の `build` で指定した根を返す。
- `parent(v)` は根付き木での親を返す。根では `-1`。
- `depth(v)` は根からの辺数を返す。
- `lca(u,v)` は指定した根に関するlowest common ancestorを返す。

`root`, `parent`, `depth`, `lca` は成功した `build` の後だけ呼べる。

## query登録

```cpp
mo.reserve_queries(count);
int id = mo.add_query(u, v);
mo.query_count();
mo.clear_queries();
```

- `reserve_queries(count)` はquery格納領域を予約する。大量queryを登録する場合の再確保を避けられる。
- `add_query(u,v)` は端点を含む単純path `u-v` を登録し、0から始まるquery idを返す。
- `query_count()` は現在登録されているquery数を返す。
- `clear_queries()` は全queryを削除するが、確保済み容量と構築済みの木は保持する。

`add_query` は成功した `build` の後だけ呼べる。

## solve

```cpp
mo.solve(add, erase, answer);
mo.solve(add, erase, answer, block_size);
```

callbackのsignatureは次の通り。

```cpp
void add(int vertex);
void erase(int vertex);
void answer(int query_id);
```

callback側の管理状態は呼出し前に空でなければならない。
`answer(id)` の時点で、`add` 済みかつ `erase` されていない頂点は、`add_query` で登録した対応pathの頂点集合と正確に一致する。
`add` と `erase` は頂点の順序に依存しない集合情報について互いに逆操作でなければならない。

全queryを処理した後、activeな全頂点へ `erase` を呼ぶため、callback側の状態は呼出し前と同じ空状態へ戻る。
登録queryは変更されず、同じobjectで `solve` を繰り返し呼べる。

`block_size > 0` では指定した幅を使う。
`block_size <= 0` では、Euler列長 `2n` とquery数 `Q` から `max(1, floor(2n / sqrt(Q)))` を自動選択する。
queryが0件ならcallbackを呼ばずに終了する。

## API別の時間計算量・空間計算量

`n` を実頂点数、`M = 2n` をEuler列長、`Q` をquery数、`B` をblock sizeとする。
`A`, `E`, `R` をそれぞれ `add`, `erase`, `answer` 1回の時間計算量とする。

| API | 時間計算量 |
| --- | --- |
| `TreeMo(n)` | `O(MAX_SIZE log(MAX_SIZE + 1))` |
| move構築・move代入 | `O(1)` |
| `size()` / `edge_count()` / `query_count()` | `O(1)` |
| `reserve_queries(count)` | 再確保時 `O(Q)`、再確保がなければ `O(1)` |
| `add_edge(u,v)` | `O(1)` |
| `build(root)` | `O(n log(MAX_SIZE + 1))` |
| `root()` / `parent(v)` / `depth(v)` | `O(1)` |
| `lca(u,v)` | `O(log(MAX_SIZE + 1))` |
| `add_query(u,v)` | 償却 `O(log(MAX_SIZE + 1))` |
| `clear_queries()` | `O(Q)` |
| `solve(..., B)` | `O(Q log Q + (QB + M^2/B + M)(A+E+1) + QR + n(E+1))` |
| `solve(...)` 自動幅 | `O(Q log Q + M sqrt(Q)(A+E+1) + QR + n(E+1))` |

`answer` は各queryについてちょうど1回呼ばれる。
非ancestor間のpathではLCAを一時的に追加・削除するため、上表の移動回数とは別に最大 `Q` 回ずつ `add` と `erase` が呼ばれる。

objectは固定長の木・Euler列・binary lifting tableとquery列を保持し、`O(MAX_SIZE log(MAX_SIZE+1) + Q)` 領域。
`solve` の追加領域はquery順とactive flagの `O(n+Q)`。

## 注意点

頂点番号は `[0,n)`。
空木を除き、入力は自己loop・多重辺のない無向連結木でなければならない。
constructorの容量違反、端点・root・query予約数の範囲違反、辺容量超過、辺数違反、cycle、非連結入力、未構築でのmetadata・query・solve呼出しでは `runtime_error` を送出する。
`build` 後の `add_edge` は許可されない。

callbackが例外を送出した場合、その例外は呼出し元へ伝播する。
この場合は途中まで更新されたcallback側状態のcleanupを保証しない。
