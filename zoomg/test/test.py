import os
import sys
sys.path.append(os.pardir)

import zoomg
import cv2
import numpy
from tqdm import tqdm

# ビデオ読み込みの初期設定
filename = sys.argv[1]
video = cv2.VideoCapture(filename)
height = int(video.get(cv2.CAP_PROP_FRAME_HEIGHT))
width = int(video.get(cv2.CAP_PROP_FRAME_WIDTH))
count = int(video.get(cv2.CAP_PROP_FRAME_COUNT))

# 時間がかかる場合 画像を圧縮する
compress = 4 # 画像を1/8に圧縮
height //= compress
width //= compress
    
# zoomgの初期化
zoom = zoomg.Zoomg(height, width)

# 進捗バー
bar = tqdm(total=count)

while True:
    # フレーム読み込み
    ret, frame = video.read()
    if not ret:
        break
    # (時間がかかる場合)画像を圧縮
    frame = cv2.resize(frame, (width, height))
    # zoomgに画像を追加
    zoom.add_image(frame)
    # 進捗バー更新
    bar.update(1)

# 背景画像を生成
zoom.generate_image(comp="ciede2000", param=30)

# 背景画像を取得
image = zoom.get_image()

# 背景画像を保存
cv2.imwrite("room.png", numpy.array(image))
bar.close()

