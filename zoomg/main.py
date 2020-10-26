"""
git init
git submodule add https://github.com/pybind/pybind11.git
[.soのビルド]
// cd build
// cmake -DPYTHON_EXECUTABLE=`which python` ..
make

---------------------------
[実行]cat 
PYTHONPATH=$PYTHONPATH:build python ../zoomg.py
PYTHONPATH=$PYTHONPATH:build python ../main.py ../../../satoki/roomg/tools/verification.mp4 0.75
clang++ -O3 -Wall -shared -std=c++14 -fPIC `python -m pybind11 --includes` -undefined dynamic_lookup zoomg.cpp -o zoomg`python3-config --extension-suffix`
python main.py ../resources/verification.mp4 0.75
"""
import zoomg
import cv2
import sys
import time
import numpy as np
from tqdm import tqdm
import threading
import logging

# ビデオ読み込みの初期設定
def video_initialization():
    args = sys.argv
    filename = args[1]

    video = cv2.VideoCapture(filename)
    if not video.isOpened():
        print("c('o'c)")
        print("plz {}".format(filename))
        sys.exit()

    _h = int(video.get(cv2.CAP_PROP_FRAME_HEIGHT))
    _w = int(video.get(cv2.CAP_PROP_FRAME_WIDTH))
    return video, _h, _w, filename


# フレームを読み込む
def frame_read(video, h, w):
    ret, tmp = video.read()
    if not ret:
        return [], False
    return cv2.resize(tmp, (w, h)).copy(), True


# def savefig(image, filename, omg):
#     cv2.imwrite(
#         "{}_room_{}_cpp.png".format(filename.replace(".mp4", ""), omg), np.array(image)
#     )


def main():
    # 画像処理初期設定
    video, height, width, filename = video_initialization()
    # 圧縮率
    div = 20  # オススメ : [1 2 4 5 8 10 16 20 40 80]
    height //= div
    width //= div
    omg = float(sys.argv[2])  # 0.63

    # 動画は607フレームの縦1080横1920
    print("Height:{}\nWidht:{}".format(height, width))

    print("add images ...")
    bar = tqdm(total=607)
    bar.set_description("Progress")
    start = time.time()
    zoom = zoomg.Zoomg(height, width)

    while True:
        frame, state = frame_read(video, height, width)
        if not state:
            break
        zoom.add_image(frame)
        bar.update(1)

    zoom.generate_image(omg)
    image = zoom.get_image()
    omgc = zoom.get_omgc()

    bar.close()

    print("omg:{}".format(omg))
    print("omgc:{}".format(omgc))
    print("Time:{:.2f}".format(time.time() - start))
    print("-" * 50)

    savefig(image, filename, omg)

    print("b(^0^b)")
    return zoom

# 検証
def verification(zoom):
    h, w = zoom.get_shape()
    back_img = cv2.imread("../resources/verification.png")
    back_img = cv2.resize(back_img, (w, h))
    ok, ng, acc = zoom.verify(back_img)
    print("ok : {}".format(ok))
    print("ng : {}".format(ng))
    print("accuracy : {}".format(acc))


if __name__ == "__main__":
    zoom = main()
    verification(zoom)
