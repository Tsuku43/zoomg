[![PyPI version](https://badge.fury.io/py/zoomg.svg)](https://badge.fury.io/py/zoomg) [![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)

# zoomg

バーチャル背景適用済み動画からバーチャル背景を復元するライブラリ

![icon](https://github.com/Tsuku43/zoomg/blob/master/images/icon.png?raw=true)

## Installing / インストール

```shell=
pip install zoomg
```

## System Requirements / 動作保証環境

- python 3.6.4
- pip 20.2.4
- OS
  - macOS Catalina 10.15.7 (x86_64)
  - macOS Bug Sur 11.4 (aarch64)
  - Ubuntu 20.04.1 LTS / 18.04.4 LTS

**Windows では動作保証をしていません**

## How to use / 使用方法

### 簡単な使用方法

```sh
python3 sample.py sample_video.mp4
```

```python
import zoomg
import cv2
import sys
import numpy


# ビデオ読み込みの初期設定
filename = sys.argv[1]
video = cv2.VideoCapture(filename)
height = int(video.get(cv2.CAP_PROP_FRAME_HEIGHT))
width = int(video.get(cv2.CAP_PROP_FRAME_WIDTH))

# zoomgの初期化
zoom = zoomg.Zoomg(height, width)

while True:
    # フレーム読み込み
    ret, frame = video.read()
    if not ret:
        break
    # zoomgに画像を追加
    zoom.add_image(frame)

# 背景画像を生成
zoom.generate_image()
# 背景画像を取得
image = zoom.get_image()
# 背景画像を保存
cv2.imwrite("room.png", numpy.array(image))
```

### Zoomg クラス

背景画像の生成を行います.

#### **コンストラクタ**

```python
zoom = zoomg.Zoomg(height, width)
```

- 引数

| パラメータ | 型  | キーワード引数 | 省略 | 説明           |
| ---------- | --- | -------------- | ---- | -------------- |
| height     | int | 可(h)          | -    | 入力画像の高さ |
| width      | int | 可(w)          | -    | 入力画像の幅   |

- 返り値

| パラメータ | 型    | 説明               |
| ---------- | ----- | ------------------ |
| zoom       | Zoomg | Zoomg オブジェクト |

---

#### **Zoomg.add_image**

zoomg オブジェクトに画像を追加します.動画を入力したい場合はフレーム単位で分割する必要があります.

```python
zoom.add_image(frame)
```

- 引数

| パラメータ | 型                   | キーワード引数 | 省略 | 説明                             |
| ---------- | -------------------- | -------------- | ---- | -------------------------------- |
| frame      | 3 次元 numpy.ndarray | -              | -    | zoomg オブジェクトに追加する画像 |

- 返り値
  - なし

---

#### **Zoomg.generate_image**

`Zoomg.add_image`で追加した画像から背景画像を復元する.

```python
zoom.generate_image(param, noise_frame, comp)
```

- 引数

| パラメータ  | 型     | キーワード引数  | 省略                    | 説明                                                                                                     |
| ----------- | ------ | --------------- | ----------------------- | -------------------------------------------------------------------------------------------------------- |
| param       | float  | 可(param)       | 可(default=5.06)        | 各ピクセルが部屋の背景かバーチャル背景か判定するためのパラメータ．詳細は以下の表「色差について」を参照． |
| noise_frame | int    | 可(noise_frame) | 可(default=1)           | 画像にノイズが多く含まれている場合,ノイズを軽減するためのパラメータ                                      |
| comp        | string | 可(noise_frame) | 可(default="ciede2000") | 色差を決定する．詳細は以下の表「色差について」を参照．                                                   |

- 色差について

| 色差計算アルゴリズム | comp        | param 指定可能範囲 | 説明                                                                   |
| -------------------- | ----------- | ------------------ | ---------------------------------------------------------------------- |
| コサイン類似度       | `cos_sim`   | [0.0, 1.0]         | 高くすると復元できるピクセル数が**減り**,低くすると誤判定が**増える**. |
| CIEDE2000            | `ciede2000` | [0.0, 100]         | 高くすると誤判定が**増え**,低くすると復元できるピクセル数が**減る**.   |

- 返り値
  - なし

---

#### **Zoomg.get_image**

`Zoomg.generate_image`で生成した画像を取得します.

```python
image = zoom.get_image()
```

- 引数

  - なし

- 返り値

| パラメータ | 型                 | 説明                 |
| ---------- | ------------------ | -------------------- |
| image      | int 型 3 次元 list | 復元した画像ファイル |

---

#### **Zoomg.get_omgc**

`Zoomg.generate_image`を使用した際,正しく復元できたか拘らず,復元したピクセル数を返します.この数は`Zoomg.generate_image`の引数`param`に影響されます.

```python
omgc = zoom.get_omgc()
```

- 引数

  - なし

- 返り値

| パラメータ | 型  | 説明               |
| ---------- | --- | ------------------ |
| omgc       | int | 復元したピクセル数 |

---

#### **Zoomg.verify**

`Zoomg.generate_image`の`param`引数の値がどれだけいいものか検証します.部屋の画像と復元した部屋の画像からどれだけ二つの画像が似ているか計測し,復元精度を返します.

```python
ok, ng, acc = zoomg.verify(room_image)
```

- 引数

| パラメータ | 型                   | キーワード引数 | 省略 | 説明       |
| ---------- | -------------------- | -------------- | ---- | ---------- |
| room_image | 3 次元 numpy.ndarray | -              | -    | 部屋の画像 |

- 返り値

| パラメータ | 型    | 説明                                   |
| ---------- | ----- | -------------------------------------- |
| ok         | int   | 部屋の画像と復元画像のピクセル一致数   |
| ng         | int   | 部屋の画像と復元画像のピクセル不一致数 |
| acc        | float | 精度(`ok / (ok + ng)`)                 |

---

#### **Zoomg.get_height**

生成画像の高さを取得します.`zoomg.Zoomg(height, width)`の`height`と同一の値になります.

```python
height = zoomg.get_height()
```

- 引数

  - なし

- 返り値

| パラメータ | 型  | 説明           |
| ---------- | --- | -------------- |
| height     | int | 生成画像の高さ |

---

#### **Zoomg.get_width**

生成画像の幅を取得します.`zoomg.Zoomg(height, width)`の`width`と同一の値になります.

```python
width = zoomg.get_width()
```

- 引数

  - なし

- 返り値

| パラメータ | 型  | 説明         |
| ---------- | --- | ------------ |
| width      | int | 生成画像の幅 |

---

#### **Zoomg.get_shape**

生成画像の高さと幅を取得します.`zoomg.Zoomg(height, width)`の`height`と同一の値になります.

```python
height, width = zoomg.get_shape()
```

- 引数

  - なし

- 返り値

| パラメータ | 型  | 説明           |
| ---------- | --- | -------------- |
| height     | int | 生成画像の高さ |
| width      | int | 生成画像の幅   |

---

### Function

その他,利用できる関数をこちらに記載します.

#### **add_noise()**

背景画像の生成を妨害します.画像に対してノイズを入ります.

```python
zoomg.get_shape(image&, height, width, rate)
```

- 引数

| パラメータ | 型                          | キーワード引数 | 省略               | 説明                 |
| ---------- | --------------------------- | -------------- | ------------------ | -------------------- |
| image      | 3 次元 numpy.ndarray の参照 | -              | -                  | ノイズを入れたい画像 |
| height     | int                         | 可(h)          | -                  | `image`の高さ        |
| width      | int                         | 可(w)          | -                  | `image`の幅          |
| rate       | float                       | 可(rate)       | 可(default=0.0003) | ノイズの入れる割合   |

- 返り値

| パラメータ | 型  | 説明           |
| ---------- | --- | -------------- |
| height     | int | 生成画像の高さ |
| width      | int | 生成画像の幅   |

---

## Build / ビルド

- 本ソースコードのコンパイル

```sh
> pwd
ほにゃらら/zoomg/zoomg

> clang++ -O3 -Wall -shared -std=c++17 -fPIC `python -m pybind11 --includes` -undefined dynamic_lookup zoomg.cpp ColorSpace/src/Comparison.cpp ColorSpace/src/ColorSpace.cpp ColorSpace/src/Conversion.cpp -o zoomg`python3-config --extension-suffix`
```

- 実行

```sh
> cd test
> pip uninstall zoomg # いらないかも
> python test.py sample_02.mp4
```
