---
title: Partially Persistent BitOverwrite RangeSum (部分永続ビット上書き・区間和)
documentation_of: ../../../../src/structure/segtree/partially_persistent/partially_persistent_bitoverwrite_rangesum.hpp
---

部分永続なbit列の区間1上書き・区間和。詳細は `Partially Persistent Bit Range Structures` を参照。

## 時間計算量

- constructor: $O(\mathtt{MAX\_NODES}+\mathtt{MAX\_VERSIONS}+\mathtt{MAX\_CHANGES})$
- updateと最新versionのquery: $O(\log \mathtt{MAX\_SIZE})$
- 過去versionのquery: $O(\log \mathtt{MAX\_SIZE}\log \mathtt{MAX\_CHANGES})$
- version・容量情報: $O(1)$

## API

公開APIのsignature・最新版更新・過去version query・半開区間・容量・例外契約は
`Partially Persistent Bit Range Structures` と同じ。各操作の計算量は下表の通り。

`set(version,l,r)` と `set(l,r)` は指定versionまたは最新版の半開区間を1で上書きし、
新version番号を返す。

## 公開API

- constructor：長さ `n` の0列をversion 0として作る。
- `size`, `versions`, `latest_version`, `nodes_used`, `changes_used`：状態情報を返す。
- `fork(version)`：最新版と同内容の新versionを返す。
- `set` / `set_one`：最新版の `[l,r)` を1へ代入する。version省略形もある。
- `assign` / `flip`：最新版の `[l,r)` を代入／反転する。version省略形もある。
- `sum(version,l,r)` / `get(version,k)`：任意versionを参照する。

更新と最新版queryは $O(\log \mathtt{MAX\_SIZE})$、過去version queryは
$O(\log \mathtt{MAX\_SIZE}\log \mathtt{MAX\_CHANGES})$。更新1回は同対数個のnode・履歴、
queryは $O(\log \mathtt{MAX\_SIZE})$ stackを使う。

## 注意点

型名上の主操作は1への上書きだが、public継承により `assign` と `flip` も利用できる。
更新元は最新版だけ。半開区間・点・version・最新版条件・各固定容量の違反では
`runtime_error`。

## 容量を省略する場合

容量引数の既定値は `-1`。`PartiallyPersistentBitOverwriteRangeSum<> tree(n);` のように容量を省略すると、同じヘッダ内の可変長版を使用する。既存の非負容量を指定する固定配列版も利用できる。 モノイド・係数型・剰余の数・分岐数など、容量以外のテンプレート引数は引き続き指定する。

木の幅・高さは実行時の入力から求め、通常の密な木は要素数に比例する記憶領域を確保する。疎な木のノード、永続版の版情報、更新・rollbackの履歴は使用量に応じてvectorを拡張する。固定配列版と同じ更新・照会アルゴリズムを用い、拡張に伴う再配置は償却計算量に含める。整数添字の表現範囲と実メモリ容量は上限となる。
