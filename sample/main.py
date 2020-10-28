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
zoom = zoomg.Zoomg(width, height)

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

cv2.imwrite("room.png", numpy.array(image))