---
title: Persistent Splay Tree (完全永続スプレー木)
documentation_of: ../src/structure/bbst/persistent_splay_tree.hpp
---

path copying により各 version の root を保持する永続 Splay Set。
同じキーは 1 個だけ保持する。version 0 は空集合である。

MAX_NODE は新規ノードとコピーしたノードの総数、MAX_VERSION は version 0 を含む version 数の上限。
容量超過時は例外を送出し、既存の version と使用ノード数は更新前の状態に保たれる。

# コンストラクタ

~~~cpp
PersistentSplaySet<Key, MAX_NODE, MAX_VERSION, Compare> set(compare);
~~~

Compare による順序でキーを保持する。

# insert / erase

~~~cpp
int set.insert(x, version)
int set.erase(x, version)
~~~

version から更新した新しい version 番号を返す。
既に存在するキーの insert、存在しないキーの erase も探索結果を splay した新しい version を作る。

更新時は探索経路だけをコピーし、新しい version 上で splay する。
古い version のノードは変更しない。

# size / empty / versions

~~~cpp
int set.size(version)
bool set.empty(version)
int set.versions()
~~~

# contains

~~~cpp
bool set.contains(x, version)
~~~

# 順序

~~~cpp
int set.order_of_key(x, version)
int set.order_of_key_upper(x, version)
optional<Key> set.kth(k, version)
vector<Key> set.to_vector(version)
~~~

order_of_key は Compare の順序で x より前にある要素数、
order_of_key_upper は x と等価な要素も含めた数を返す。
kth は 0-indexed。範囲外では nullopt を返す。
to_vector は Compare の順序で全要素を返す。

# 境界探索

~~~cpp
optional<Key> set.lower_bound(x, version)
optional<Key> set.upper_bound(x, version)
optional<Key> set.max_leq(x, version)
optional<Key> set.max_less(x, version)
~~~

`lower_bound` と `upper_bound` は `std::set<Key, Compare>` と同じ意味。
max_leq は upper_bound(x) の直前、max_less は lower_bound(x) の直前の要素を返す。
該当する要素がなければ nullopt を返す。

読み取り操作は version を作らないため splay を行わない。

# 時間計算量

対象versionの木の高さを $H$、要素数を $N$ とする。$H$ は worst-case $N$。

- `PersistentSplaySet(compare)`: 固定arena初期化 $O(\mathrm{MAX\_NODE}+\mathrm{MAX\_VERSION})$
- `versions`, `size`, `empty`: $O(1)$
- `contains`, `order_of_key`, `order_of_key_upper`, `kth`, `lower_bound`, `upper_bound`, `max_leq`, `max_less`: $O(H)$
- `insert`, `erase`: $O(H)$、新規version $O(1)$、path-copy node $O(H)$
- `to_vector`: 出力要素数 $N$ に対して $O(N)$

読み取り操作は木を変形しないため償却評価の対象外。各更新が直前に作ったversionを使う一本道では更新は償却 $O(\log(N+1))$。古いversionから分岐する操作列にはpotentialがtelescopingしないため、各更新の保証は $O(H)$ である。

## 空間計算量

- 固定arenaとroot table: $O(\mathrm{MAX\_NODE}+\mathrm{MAX\_VERSION})$
- `to_vector` の戻り値とstack: $O(N)$

## 注意点

- `Compare`はstrict weak ordering、versionは`[0,versions())`。範囲外versionは例外。
- updateでversion数またはnode数が容量を超える場合は例外で、既存versionを変えない。
- `kth`と境界探索は該当なしなら`nullopt`。
- updateは常にversionを1個作り、読み取り操作はversion/nodeを消費しない。
