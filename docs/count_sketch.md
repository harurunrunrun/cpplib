---
title: Count Sketch (Countスケッチ)
documentation_of: ../src/approximate/streaming/count_sketch.hpp
---

正負の更新を許す turnstile stream の点頻度を、符号付き hash と median で近似する。

- `CountSketch(seed)`: 空で構築する。$O(Width\,Depth)$。
- `add(key, delta)`: `delta` を追加する。$O(Depth)$。
- `estimate(key)`: row ごとの符号補正値の median を返す。$O(Depth)$。
- `clear()`: 全 counter を 0 にする。$O(Width\,Depth)$。
- `width()`, `depth()`: $O(1)$。

独立な index hash と符号 hash の理論モデルでは、1 row の標準偏差は
$O(\lVert f_{\ne x}\rVert_2/\sqrt{Width})$。median を取ることで定数誤差の
失敗確率は $e^{-\Theta(Depth)}$ まで下がる。保存領域は $O(Width\,Depth)$。
counter は表現範囲で飽和する。理論保証は飽和しない更新列に対するもの。
