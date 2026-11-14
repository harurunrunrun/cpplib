---
title: Trie (トライ)
documentation_of: ../../../../src/structure/trie/string/trie.hpp
---

文字列の多重集合を管理するトライである。同じ文字列を重複して登録できる。

## テンプレート引数

```cpp
Trie<ALPHABET = 26, MAX_NODES = -1, OFFSET = 'a'>
```

- 文字種類数 `ALPHABET`
- ノード数の上限 `MAX_NODES`（省略するとデフォルト値 `-1` の可変長版）
- 先頭文字 `OFFSET`

## `insert` / `erase`

```cpp
void insert(string_view s)
bool erase(string_view s)
```

`insert` は `s` を 1 個登録する。`erase` は `s` を 1 個削除して `true` を返し、文字列が存在しない場合は `false` を返す。ノードは回収しない。

## 個数

```cpp
int count(string_view s)
bool contains(string_view s)
int prefix_count(string_view s)
int node(string_view s)
int size()
int node_count()
bool empty()
```

`count` は文字列の個数を返す。`size` は重複を含む登録数、`node_count` は使用ノード数を返す。

`prefix_count` は接頭辞 `s` を持つ登録文字列の個数を返す。

`node` は対応するノード番号を返す。存在しない場合 `-1` を返す。

## ノード遷移

```cpp
int next_node(int state, char c)
int terminal_count(int state)
```

`next_node` はノード `state` から文字 `c` で遷移した先のノード番号を返す。遷移が存在しない場合は `-1` を返す。

`terminal_count` はノード `state` で終わる文字列の登録個数を返す。

## `clear`

```cpp
void clear()
```

登録済みの文字列とノードをすべて破棄し、空の Trie に戻す。

## `is_prefix_free`

```cpp
bool is_prefix_free() const
```

登録されたどの文字列も、別の登録文字列の接頭辞でなければ `true` を返す。
同じ文字列が複数回登録されている場合も `false` を返す。

## API 別の時間計算量

$L$ を入力文字列の長さとする。

- `Trie()`: 固定長版は $O(\texttt{ALPHABET}\cdot\texttt{MAX_NODES})$（ノード配列を初期化する）、可変長版は $O(\texttt{ALPHABET})$
- `size`, `node_count`, `empty`: $O(1)$
- `next_node`, `terminal_count`: $O(1)$
- `is_prefix_free`: $O(V)$。$V$ は `node_count()`
- `node`, `count`, `contains`, `prefix_count`: 最悪 $O(L)$
- `insert`: $O(L + K \cdot \texttt{ALPHABET})$。$K \le L$ は新しく作るノードの個数（各ノードの子配列を初期化する）
- `erase`: $O(L)$。ノードは回収しない
- `clear`: $O(V\cdot\texttt{ALPHABET})$。$V$ は呼び出し直前の `node_count()`

## 空間計算量

- ノード配列: 固定長版は $O(\texttt{ALPHABET}\cdot\texttt{MAX_NODES})$、可変長版は使用ノード数に比例
- 各操作の追加領域: $O(1)$

## 注意点

- 各文字は `[OFFSET, OFFSET + ALPHABET)` に含まれる必要があり、範囲外なら `runtime_error` を送出する。
- `insert` で必要なノード数が `MAX_NODES` を超える場合は `runtime_error` を送出する。
- `next_node(state, c)` と `terminal_count(state)` は、`state` が現在のノード番号の範囲外なら例外を送出する。
- `is_prefix_free()` の結果は挿入順に依存しない。空のトライと、空文字列だけを 1 個登録したトライは `true` である。
- `clear()` 後は `size() == 0` かつ `node_count() == 1` となり、それ以前に得たノード番号は無効になる。
- 空文字列も通常の文字列として根ノードに登録できる。
- 容量を指定した固定配列版（`Trie<ALPHABET, NODES>`）と可変長版（`Trie<ALPHABET>`）は同じヘッダで利用できる。可変長版は必要に応じて領域を伸長し、その費用はならして上記の計算量に含まれる。
