---
title: Rope (ロープ)
documentation_of: ../../../src/structure/string/rope.hpp
---

乱択平衡二分木（暗黙の treap）で可変長の文字列を管理する。
任意位置への文字列の挿入、区間の削除、一文字の参照・変更、部分文字列の取得を扱う。

## API

~~~cpp
Rope()
Rope(string_view text)
bool empty() const
size_t size() const
char at(size_t index) const
void insert(size_t position, string_view text)
void erase(size_t position, size_t length)
void set(size_t position, char value)
string substr(size_t position, size_t length)
string str() const
~~~

- `insert` は `position` 番目の文字の直前（`position == size()` なら末尾）に `text` を挿入する。
- `erase` と `substr` は `[position, position + length)` を対象とする。
- `substr` は内部で木を分割・併合するため非 `const` である。

## 時間計算量

現在の文字数を $N$、挿入文字数を $M$、削除・出力する文字数を $K$ とする。

- `empty`, `size`: $O(1)$
- `at`, `set`: 期待 $O(\log(N+1))$
- `erase`: 期待 $O(\log(N+1)+K)$（削除したノードの解放を含む）
- `insert`: 期待 $O(M+\log(N+M+1))$（挿入する文字列はデカルト木の線形時間構築で木にする）
- 文字列からのコンストラクタ: $O(M)$
- `substr`: 期待 $O(\log(N+1)+K)$
- `str`: $O(N)$

空間計算量は $O(N)$ である。treap の偏りによる最悪計算量は線形である。

## 注意点

- コピー不可、ムーブ可能である。
- `position` や `length` が範囲外なら `out_of_range` を送出する。
