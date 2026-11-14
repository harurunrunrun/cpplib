---
title: Persistent Trie (完全永続トライ)
documentation_of: ../../../../src/structure/trie/string/persistent_trie.hpp
---

完全永続のトライである。文字列の多重集合を版（バージョン）ごとに管理し、同じ文字列を重複して登録できる。構築直後は空の版 0 だけが存在する。

## テンプレート引数

```cpp
PersistentTrie<ALPHABET = 26, MAX_NODES = -1, MAX_VERSIONS = -1, OFFSET = 'a'>
```

- 文字種類数 `ALPHABET`
- ノード数の上限 `MAX_NODES`、版の数の上限 `MAX_VERSIONS`（省略するとデフォルト値 `-1` の可変長版）
- 先頭文字 `OFFSET`

## 版

```cpp
int fork(int version)
int versions()
int node_count()
int root(int version)
```

`fork` は `version` と同じ根を持つ新しい版を作り、その番号を返す。
`versions` は版の数、`node_count` は使用済みのノード数を返す。
`root` は指定した版の根ノードの番号を返す。

更新が容量不足で失敗した場合、版とノードは消費されない。

## `node`

```cpp
int node(int version, string_view s) const
```

`version` で文字列 `s` に対応するノード番号を返す。存在しなければ `-1` を返す。

## `insert` / `erase`

```cpp
int insert(int version, string_view s)
int erase(int version, string_view s)
```

`version` に `s` を 1 個追加 / 削除した新しい版の番号を返す。`erase` は文字列が存在しない場合、ノードを生成せずに同じ内容の新しい版を返す。

## 個数

```cpp
int count(int version, string_view s)
bool contains(int version, string_view s)
int prefix_count(int version, string_view s)
int size(int version)
```

`count` は完全一致する文字列の個数、`contains` は完全一致する文字列の有無、
`prefix_count` は接頭辞 `s` を持つ登録文字列の個数、`size` は重複を含む登録文字列の総数を返す。

## API 別の時間計算量

$L$ を入力文字列の長さとする。

- `PersistentTrie()`: 固定長版は $O(\texttt{ALPHABET}\cdot\texttt{MAX_NODES}+\texttt{MAX_VERSIONS})$、可変長版は $O(\texttt{ALPHABET})$
- `versions`, `node_count`, `root`, `size`, `fork`: $O(1)$。`fork` はノードを増やさず版だけを生成する
- `node`, `count`, `contains`, `prefix_count`: 最悪 $O(L)$
- `insert`: $O(L \cdot \texttt{ALPHABET})$（子配列のコピーを含む）。経路コピーでノード $L+1$ 個と版 1 個を生成する
- `erase`: 存在すれば $O(L \cdot \texttt{ALPHABET})$ で経路コピーのノード $L+1$ 個を、存在しなければ $O(L)$ でノード 0 個を生成する。どちらも版を 1 個生成する

## 空間計算量

- ノード・版の配列: 固定長版は $O(\texttt{ALPHABET}\cdot\texttt{MAX_NODES}+\texttt{MAX_VERSIONS})$、可変長版は使用量に比例
- `insert` / 存在する文字列の `erase` は $O(L)$ 個のノードを追加する。その他の API の追加領域は $O(1)$ である

## 注意点

- 文字は `[OFFSET, OFFSET + ALPHABET)` に含まれる必要がある。版の番号は `[0, versions())` である。
- 文字の範囲、版の番号、ノード・版の容量の違反では `runtime_error` を送出する。
- 容量を指定した固定配列版（`PersistentTrie<ALPHABET, NODES, VERSIONS>`）と可変長版（`PersistentTrie<ALPHABET>`）は同じヘッダで利用できる。可変長版は必要に応じて領域を伸長し、その費用はならして上記の計算量に含まれる。
