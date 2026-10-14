---
title: Persistent Red Black Tree (完全永続赤黒木)
documentation_of: ../../../../src/structure/bbst/map/persistent_red_black_tree.hpp
---

path-copy left-leaning red-black tree による完全永続構造。

永続平衡二分探索木による集合。

同じキーは 1 個だけ保持する。更新は新しい version を返す。

## テンプレート引数

```cpp
PersistentRedBlackSet<Key, MAX_NODE, MAX_VERSION, Compare>
```

## コンストラクタ

```cpp
PersistentRedBlackSet set
```

version `0` は空集合。

## insert / erase / contains

```cpp
int set.insert(x, version)
int set.erase(x, version)
bool set.contains(x, version)
```

`insert` と `erase` は更新後の version を返す。

## 順序

```cpp
int set.order_of_key(x, version)
int set.order_of_key_upper(x, version)
optional<Key> set.kth(k, version)
```

## 探索

```cpp
optional<Key> set.lower_bound(x, version)
optional<Key> set.upper_bound(x, version)
optional<Key> set.max_leq(x, version)
optional<Key> set.max_less(x, version)
```

## API別の時間計算量

$N$ を対象versionの要素数とし、比較と `Key` のコピーを $O(1)$ とする。

- `PersistentRedBlackSet(compare)`: $O(\mathrm{MAX\_NODE}+\mathrm{MAX\_VERSION})$
- `versions`, `size`, `empty`: $O(1)$
- `contains`, `order_of_key`, `order_of_key_upper`, `kth`, `lower_bound`, `upper_bound`, `max_leq`, `max_less`: worst-case $O(\log(N+1))$
- `insert`, `erase`: worst-case $O(\log(N+1))$、新規version $O(1)$、path-copy node $O(\log(N+1))$
- `to_vector`: 出力要素数 $N$ に対して $O(N)$

重複 `insert` / 存在しない `erase` も探索後に新しいversionを作るが、nodeは複製しない。

## 空間計算量

- 固定arenaとroot table: $O(\mathrm{MAX\_NODE}+\mathrm{MAX\_VERSION})$
- `to_vector` の戻り値とstack: $O(N)$

## 注意点

- `Compare`はstrict weak ordering、versionは`[0,versions())`。違反時は例外。
- updateでversion/node容量を超える場合は例外で、既存versionとnode数を変えない。
- 同値キーは1個だけ保持し、内容が変わらないupdateもversionを1個作る。
- `kth`と境界探索は該当なしなら`nullopt`。

## 容量を省略した可変長版

容量のテンプレート引数 `MAX_NODE`, `MAX_VERSION` は省略でき、既定値 `-1` ではvectorを使う。
容量を指定した既存の固定配列版も同じヘッダで利用できる。

```cpp
using Variable = PersistentRedBlackSet<Key>;
using Fixed = PersistentRedBlackSet<Key, Nodes, Versions>;
```

可変長版は必要なノード・版・履歴を明示的に追加し、保存要素数を $K$ として追加領域は $O(K)$。
再確保の費用は追加操作全体で償却され、上記各操作の計算量を維持する。
整数添字の表現上限は残り、不正な添字や上限超過は検証する。
内部vectorの再確保をまたいでノード参照を保持せず、ノード番号から取り直す。
