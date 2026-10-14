---
title: Persistent Trie (完全永続トライ)
documentation_of: ../../../../src/structure/trie/string/persistent_trie.hpp
---

永続 Trie。重複を持つ。

## テンプレート引数

```cpp
PersistentTrie<ALPHABET, MAX_NODES, MAX_VERSIONS, OFFSET>
```

## version

```cpp
int fork(int version)
int versions()
int node_count()
int root(int version)
```

`fork` は同じ根を持つ新しい version を作る。
`versions`はversion数、`node_count`は使用済みnode数を返す。
`root`は指定versionのroot node番号を返す。


更新が容量不足で失敗した場合、version と node は消費されない。
`version`が`[0,versions())`の範囲外なら例外。
`fork`、`insert`、`erase`でversion数が`MAX_VERSIONS`を超える場合も例外。

## node

```cpp
int node(int version, string_view s) const
```

`version`で文字列`s`に対応するnode番号を返す。存在しなければ`-1`。
文字は`[OFFSET, OFFSET + ALPHABET)`に含まれる必要があり、範囲外なら例外。


## insert / erase

```cpp
int insert(int version, string_view s)
int erase(int version, string_view s)
```

新しい version を返す。`erase` は文字列が存在しない場合、同じ内容の新しい version を返す。

## count

```cpp
int count(int version, string_view s)
bool contains(int version, string_view s)
int prefix_count(int version, string_view s)
int size(int version)
```

## API別の時間計算量

$L$ を入力文字列長とする。

`count`は完全一致数、`contains`は完全一致の有無、
`prefix_count`は接頭辞`s`を持つ登録文字列数、`size`は重複込み総登録数を返す。
`insert`でnode容量を超える場合は例外。`erase`対象が存在しない場合は
nodeを生成せず、同じ内容のversionだけを作る。

- `PersistentTrie()`: $O(\mathrm{ALPHABET}\cdot\mathrm{MAX\_NODES}+\mathrm{MAX\_VERSIONS})$
- `versions`, `node_count`, `root`, `size`, `fork`: $O(1)$。`fork` はnodeを増やさずversionだけを生成する
- `node`, `count`, `contains`, `prefix_count`: worst-case $O(L)$
- `insert`: $O(L)$、path-copy node $L+1$ 個と1 versionを生成する
- `erase`: $O(L)$。存在すればpath-copy node $L+1$ 個、存在しなければ0個。どちらも1 versionを生成する

## 空間計算量

- 固定node/version array: $O(\mathrm{ALPHABET}\cdot\mathrm{MAX\_NODES}+\mathrm{MAX\_VERSIONS})$
- 各操作の追加stack: $O(1)$

## 注意点

文字は `[OFFSET, OFFSET + ALPHABET)`、versionは `[0,versions())`。文字範囲、version、
node・version容量の違反では `runtime_error`。`erase` 対象が存在しない場合も同内容の
新versionを1個作る。
固定保存領域は $O(\mathtt{ALPHABET}\cdot\mathtt{MAX\_NODES}+\mathtt{MAX\_VERSIONS})$。
`insert` / 存在する文字列の `erase` は $O(L)$ nodeを追加し、その他の各APIの追加領域は
$O(1)$。

## 容量を省略した可変長版

容量テンプレートを省略すると既定値 `-1` でvectorを使い、必要な保存領域を明示的に伸長する。
固定容量を指定する既存版も同じヘッダに残る。

```cpp
using Variable = PersistentTrie<Alphabet>;
using Fixed = PersistentTrie<Alphabet, Nodes, Versions>;
```

伸長の費用は追加操作全体で償却され、上記の各操作の計算量を維持する。
領域は確保済みノード・履歴・heap要素などの個数に比例する。
整数添字の表現上限を超える要素数は例外。構造上の文字種類・bit幅・座標域は、件数容量とは別に指定できる。
