#include <pybind11/numpy.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include <iostream>
#include <map>
#include <tuple>
#include <vector>

using Pixel = std::vector<uint8_t>;
using Image = std::vector<std::vector<Pixel>>;
using Movie = std::vector<Image>;

class Zoomg {
   private:
    // 画素の値がキー，値を<個数, 出現タイミング>として出現頻度を管理
    std::vector<std::vector<std::map<Pixel, std::pair<int, int>>>> freq;
    // 生成する画像
    Image image;
    // 動画の高さと幅
    int height, width;
    // 出現頻度のソート用配列
    std::vector<std::tuple<Pixel, int, int>> most_common;
    // 生成率
    int omgc;
    // 出現︎タイミングを保持する
    int order;

    double cos_sim(Pixel &p, const Pixel &q) const {
        // コサイン類似度を計算
        double norm_a = 0.0;
        double norm_b = 0.0;
        double in_prod = 0.0;
        for (int i = 0; i < 3; ++i) {
            in_prod += p[i] * q[i];
            norm_a += p[i] * p[i];
            norm_b += q[i] * q[i];
        }
        const double norm = sqrt(norm_a) * sqrt(norm_b);
        return (norm == 0.0) ? 1.0 : in_prod / norm;
    }

   public:
    Zoomg(const int h, const int w) : height(h), width(w), omgc(0), order(0) {
        // 初期化
        freq = std::vector<std::vector<std::map<Pixel, std::pair<int, int>>>>(
            h, std::vector<std::map<Pixel, std::pair<int, int>>>(w));
        image = Image(h, std::vector<Pixel>(w));
    }
    void add_image(const pybind11::array_t<uint8_t> img_np) {
        // 画像を登録
        const uint8_t *img = (const uint8_t *)img_np.request().ptr;
        for (int h = 0; h < height; ++h) {
            for (int w = 0; w < width; ++w) {
                const Pixel p = Pixel(img + 3 * (h * width + w),
                                      img + 3 * (h * width + w + 1));
                if (freq[h][w].find(p) != freq[h][w].end()) {
                    ++freq[h][w][p].first;
                } else {
                    freq[h][w][p] = {1, order++};
                }
            }
        }
    }
    void generate_image(const double param, const int noise_frame) {
        // 画像を生成
        for (int h = 0; h < height; ++h) {
            for (int w = 0; w < width; ++w) {
                most_common.clear();
                for (auto it = freq[h][w].begin(); it != freq[h][w].end();
                     ++it) {
                    // ノイズは無視
                    if (it->second.first <= noise_frame) continue;
                    // 画素, 個数, 出現タイミングの順で配列に画素の情報を追加
                    most_common.emplace_back(it->first, it->second.first,
                                             it->second.second);
                }
                // 出現個数(昇順), 出現タイミング(降順)の順でソート
                sort(most_common.begin(), most_common.end(),
                     [](auto const &lhs, auto const &rhs) {
                         if (std::get<1>(lhs) == std::get<1>(rhs)) {
                             return std::get<2>(lhs) > std::get<2>(rhs);
                         } else {
                             return std::get<1>(lhs) < std::get<1>(rhs);
                         }
                     });
                const Pixel most = std::get<0>(*most_common.rbegin());
                [&]() -> void {
                    // コサイン類似度がparam未満で出現タイミングが最も早い画素を探索
                    for (auto p : most_common) {
                        if (cos_sim(std::get<0>(p), most) < param) {
                            ++omgc;
                            image[h][w] = std::get<0>(p);
                            return;
                        }
                    }
                    // 探索しても見つからなかった場合
                    image[h][w] = most;
                }();
            }
        }
    }
    Image get_image(void) const {
        // 生成された画像を取得
        return image;
    }
    int get_omgc(void) const {
        // 生成された画像を取得
        return omgc;
    }
    int get_height(void) const {
        // 生成された画像を取得
        return height;
    }
    int get_width(void) const {
        // 生成された画像を取得
        return width;
    }
    std::pair<int, int> get_shape(void) const {
        // 生成された画像を取得
        return {height, width};
    }
    std::tuple<int, int, double> verify(const pybind11::array_t<int> &img_np) {
        // 生成した画像がどれくらい部屋を復元できているか検証
        const uint8_t *verify_img = (const uint8_t *)img_np.request().ptr;
        int ok = 0, ng = 0;
        for (int h = 0; h < height; ++h) {
            for (int w = 0; w < width; ++w) {
                const Pixel p = Pixel(verify_img + 3 * (h * width + w),
                                      verify_img + 3 * (h * width + w + 1));
                if (cos_sim(image[h][w], p) > 0.999) {
                    ++ok;
                } else {
                    ++ng;
                }
            }
        }
        return {ok, ng, ok / (double)(ok + ng)};
    }
};

/*
const pybind11::array_t<int> &image := 入力画像
const int h := 画像の高さ
const int w := 画像のはば
const int frame := 生成するフレーム数
const double rate := 過疎率
*/

void add_noise(const pybind11::array_t<uint8_t> &image, const int h, const int w,
               const double rate) {
    // 画像をノイズ入り動画へ変換
    uint8_t *img = (uint8_t *)image.request().ptr;
    srand((unsigned)time(NULL));

    for (int k = 0; k < h * w * rate; ++k) {
        int _h = rand() % h;
        int _w = rand() % w;
        for (int p = 0; p < 3; ++p) {
            img[3 * (_h * w + _w) + p] = rand() % 256;
        }
    }
}

PYBIND11_MODULE(zoomg, m) {
    pybind11::class_<Zoomg>(m, "Zoomg")
        .def(pybind11::init<const int, const int>(), pybind11::arg("h"),
             pybind11::arg("w"))
        .def("add_image", &Zoomg::add_image)
        .def("generate_image", &Zoomg::generate_image,
             pybind11::arg("param") = 0.75, pybind11::arg("noise_frame") = 0)
        .def("get_image", &Zoomg::get_image)
        .def("get_omgc", &Zoomg::get_omgc)
        .def("verify", &Zoomg::verify)
        .def("get_height", &Zoomg::get_height)
        .def("get_width", &Zoomg::get_width)
        .def("get_shape", &Zoomg::get_shape);
    m.def("add_noise", &add_noise, pybind11::arg("image"), pybind11::arg("h"),
          pybind11::arg("w"), pybind11::arg("rate") = 0.0003);
}
