---
title: Persistent BitOverwrite RangeSum (完全永続ビット上書き・区間和)
documentation_of: ../src/structure/segtree/persistent_bitoverwrite_rangesum.hpp
---

完全永続なbit列の区間1上書き・区間和。APIと計算量は `Persistent Bit Range Structures` を参照。

# 時間計算量

- constructor: $O(\mathtt{MAX\_NODES}+\mathtt{MAX\_VERSIONS})$
- `assign`, `flip`, `set_one`, `set`, `sum`, `get`: $O(\log \mathtt{MAX\_SIZE})$
- `fork`とversion・容量情報: $O(1)$

# API

公開APIのsignature・version・半開区間・容量・例外契約は `Persistent Bit Range Structures` と同じ。
この型で利用できる各操作の計算量は下表の通り。

## 公開API

- constructor：長さ `n` の0列をversion 0として作る。
- `size`, `versions`, `latest_version`, `nodes_used`：列長と使用量を返す。
- `fork(version)`：指定versionと同内容の新versionを返す。
- `set(version,l,r)` / `set_one(version,l,r)`：`[l,r)` を1へ代入した新versionを返す。
- `assign(version,l,r,value)`：`[l,r)` を0または1へ代入した新versionを返す。
- `flip(version,l,r)`：`[l,r)` を反転した新versionを返す。
- `sum(version,l,r)` / `get(version,k)`：1の個数／1点を返す。

`set`, `set_one`, `assign`, `flip`, `sum`, `get` は
$O(\log \mathtt{MAX\_SIZE})$、`fork` と状態情報は $O(1)$。更新1回は
$O(\log \mathtt{MAX\_SIZE})$ node、queryは同量の再帰stackを使う。

## 注意点

型名上の主操作は1への上書きだが、public継承により `assign`, `flip`, `set_one` も利用できる。
任意の有効versionから分岐できる。`0 < n <= MAX_SIZE`、半開区間・点・versionが
有効でなければならず、範囲・node/version容量違反では `runtime_error`。
