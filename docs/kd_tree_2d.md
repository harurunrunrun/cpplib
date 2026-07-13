---
title: Static 2D KD Tree
documentation_of: ../src/structure/other/kd_tree_2d.hpp
---

静的な2次元点集合に対する直交長方形range search。

# point

~~~cpp
KdTreePoint2D<T>{x, y, id}
~~~

# 構築

~~~cpp
KdTree2D<T, MAX_POINTS>()
KdTree2D<T, MAX_POINTS>(const vector<Point>& points)
void build(const vector<Point>& points)
~~~

各depthで軸を交互に選び、medianで分割する。各部分木はbounding boxを保持する。

# 操作

~~~cpp
int size() const
bool empty() const
void clear()
vector<int> range_search(min_x, max_x, min_y, max_y) const
~~~

`range_search` は閉長方形 `[min_x,max_x] x [min_y,max_y]` に含まれるpoint idを列挙する。戻り値の順序は規定しない。

# 計算量

$N$ を点数、$K$ を列挙する点数とする。座標比較を $O(1)$ とする。

- default constructor: 固定長storage初期化 $O(\mathrm{MAX\_POINTS})$
- vector constructor: 固定長storage初期化とbuildを含めて expected $O(\mathrm{MAX\_POINTS}+N\log(N+1))$
- `build`: `nth_element` の平均計算量に基づき expected $O(N\log(N+1))$
- `size`, `empty`, `clear`: $O(1)$
- `range_search`: worst-case $O(N+K)$、戻り値構築 $O(K)$

均衡KD-treeの2次元直交range searchは、一般的な分布では $O(\sqrt N+K)$ が目安になるが、worst-case保証は線形である。

# 空間計算量

- 固定storage: $O(\mathrm{MAX\_POINTS})$
- 構築の再帰stack: $O(\log(N+1))$
- `range_search`: 再帰stackと戻り値を含めて $O(\log(N+1)+K)$

# 前提・例外・容量

- 点数が`MAX_POINTS`を超えるconstructorまたは`build`は例外。
- `range_search(min_x,max_x,min_y,max_y)`は各下限が上限以下であること。逆転時は例外。
- 戻り値はpointの`id`であり、idの一意性や範囲は構造側では検査しない。
- `clear`は$O(1)$で論理的に空にし、次の`build`で固定storageを再利用する。
