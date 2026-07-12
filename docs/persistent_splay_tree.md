---
title: Persistent Splay Tree
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

lower_bound と upper_bound は std::set<Key, Compare> と同じ意味。
max_leq は upper_bound(x) の直前、max_less は lower_bound(x) の直前の要素を返す。
該当する要素がなければ nullopt を返す。

読み取り操作は version を作らないため splay を行わない。

# 計算量

対象 version の木の高さを $H$、要素数を $N$ とする。

- insert, erase: 時間 $O(H)$、追加ノード $O(H)$
- contains, order_of_key, order_of_key_upper, kth, 境界探索: $O(H)$
- size, empty, versions: $O(1)$
- to_vector: $O(N)$

常に最新 version から更新を続ける場合、insert と erase は償却 $O(\log N)$。
任意の古い version から分岐できるため、操作列全体では最悪 $O(N)$ となる。
