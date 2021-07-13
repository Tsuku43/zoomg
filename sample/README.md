# Sample

zoomg を用いた最も簡単なサンプルプログラム

```shell
# 実行コマンド
pip3 install -U pip
pip install opencv-python numpy tqdm
pip3 install zoomg # zoomgのインストールは下記「Installing」参照
python3 sample.py ../resources/verification.mp4
```

```python
import zoomg
import cv2
import sys
import numpy
from tqdm import tqdm

# ビデオ読み込みの初期設定
filename = sys.argv[1]
video = cv2.VideoCapture(filename)
height = int(video.get(cv2.CAP_PROP_FRAME_HEIGHT))
width = int(video.get(cv2.CAP_PROP_FRAME_WIDTH))
count = int(video.get(cv2.CAP_PROP_FRAME_COUNT))

# zoomgの初期化
zoom = zoomg.Zoomg(height, width)

# 進捗バー
bar = tqdm(total=count)

while True:
    # フレーム読み込み
    ret, frame = video.read()
    if not ret:
        break
    # zoomgに画像を追加
    zoom.add_image(frame)
    # 進捗バー更新
    bar.update(1)

# 背景画像を生成
zoom.generate_image()
# 背景画像を取得
image = zoom.get_image()

# 背景画像を保存
cv2.imwrite("room.png", numpy.array(image))
bar.close()
```

## room.png

![room.png](https://github.com/Tsuku43/zoomg/blob/master/sample/room.png?raw=true)
