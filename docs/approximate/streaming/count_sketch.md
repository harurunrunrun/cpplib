---
title: Count Sketch (Countスケッチ)
documentation_of: ../../../src/approximate/streaming/count_sketch.hpp
---

## Count Sketch (Countスケッチ)

正負の更新を許す turnstile stream の点頻度を、符号付き hash と median で近似する。

## API

- `CountSketch(seed)`: 空で構築する。時間計算量は $O(Width\,Depth)$。 追加空間計算量は $O(1)$。
- `add(key, delta)`: `delta` を追加する。時間計算量は $O(Depth)$。 追加空間計算量は $O(1)$。
- `estimate(key)`: row ごとの符号補正値の median を返す。時間計算量は $O(Depth)$。 追加空間計算量は $O(Depth)$。
- `clear()`: 全 counter を 0 にする。時間計算量は $O(Width\,Depth)$。 追加空間計算量は $O(1)$。
- `width()`, `depth()`: 時間計算量は $O(1)$。 追加空間計算量は $O(1)$。


## 注意点

独立な index hash と符号 hash の理論モデルでは、1 row の標準偏差は
$O(\lVert f_{\ne x}\rVert_2/\sqrt{Width})$。median を取ることで定数誤差の
失敗確率は $e^{-\Theta(Depth)}$ まで下がる。保存に必要な空間計算量は $O(Width\,Depth)$。
counter は表現範囲で飽和する。理論保証は飽和しない更新列に対するもの。

## 容量を省略する可変長版

~~~cpp
CountSketch<> sketch(1024, 5, 123); // 幅、深さ、seed
sketch.add(42, -3);
auto frequency = sketch.estimate(42);
~~~

幅 $W$ と深さ $D$ を実行時に指定し、counter を `vector` に保存する。
構築・`clear` は $O(WD)$ 時間、追加は $O(D)$ 時間、推定は $O(D\log D)$ 時間。
保存領域は $O(WD)$、推定時の一時領域は $O(D)$、追加時は $O(1)$。
容量を明示した固定配列版も同じ header で利用できる。
