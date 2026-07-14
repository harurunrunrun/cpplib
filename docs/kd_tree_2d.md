---
title: Static 2D KD Tree (静的二次元KD木) [DSL_2_C]
documentation_of: ../src/structure/other/kd_tree_2d.hpp
---

静的な2次元点集合に対する直交長方形range search。
同じ点集合に対するEuclid距離の最近傍探索にも対応する。

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
KdTreeNearestResult2D nearest_neighbor(x, y) const
~~~

`range_search` は閉長方形 `[min_x,max_x] x [min_y,max_y]` に含まれるpoint idを列挙する。戻り値の順序は規定しない。

nearest_neighborはEuclid距離が最小の点を返す。同距離ならidが最小の点を選ぶ。

~~~cpp
struct KdTreeNearestResult2D {
    int id;
    long double squared_distance;
    bool found;
    bool exists() const;
};
~~~

空集合ではfoundとexists()がfalseになる。空でない場合、idは選ばれた点のid、squared_distanceは二乗距離である。idが-1の点も格納できるため、有無はexists()で判定する。

# 時間計算量

$N$ を点数、$K$ を列挙する点数とする。座標比較を $O(1)$ とする。

- default constructor: 固定長storage初期化 $O(\mathrm{MAX\_POINTS})$
- vector constructor: 固定長storage初期化とbuildを含めて expected $O(\mathrm{MAX\_POINTS}+N\log(N+1))$
- `build`: `nth_element` の平均計算量に基づき expected $O(N\log(N+1))$
- `size`, `empty`, `clear`: $O(1)$
- `range_search`: worst-case $O(N+K)$、戻り値構築 $O(K)$

- nearest_neighbor: worst-case O(N)。均衡KD-treeと一般的な点分布では平均O(log(N+1))が目安。結果memberとexists()はO(1)

均衡KD-treeの2次元直交range searchは、一般的な分布では $O(\sqrt N+K)$ が目安になるが、worst-case保証は線形である。

# 空間計算量

- 固定storage: $O(\mathrm{MAX\_POINTS})$
- 構築の再帰stack: $O(\log(N+1))$
- `range_search`: 再帰stackと戻り値を含めて $O(\log(N+1)+K)$
- nearest_neighbor: 通常の均衡時は再帰stack O(log(N+1))、worst-case O(N)

## 注意点

- 座標型はlong doubleへ変換でき、格納点の座標は有限値であること。nearest_neighborのquery座標が非有限値ならinvalid_argument。
- 最近傍の二乗距離はlong doubleで計算する。極端に大きい座標や非常に近い浮動小数点座標では丸め誤差を含み得る。
- 点数が`MAX_POINTS`を超えるconstructorまたは`build`は例外。
- `range_search(min_x,max_x,min_y,max_y)`は各下限が上限以下であること。逆転時は例外。
- 戻り値はpointの`id`であり、idの一意性や範囲は構造側では検査しない。
- `clear`は$O(1)$で論理的に空にし、次の`build`で固定storageを再利用する。
