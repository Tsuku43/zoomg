#include <pybind11/numpy.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include <iostream>
#include <map>
#include <tuple>
#include <vector>

using namespace std;
using Pixel = vector<int>;
using Image = vector<vector<Pixel>>;

class Zoomg {
   private:
    // 画素の値がキー，値を<個数, 出現タイミング>として出現頻度を管理
    vector<vector<map<Pixel, pair<int, int>>>> freq;
    // 生成する画像
    Image image;
    // 動画の高さと幅
    int height, width;
    // 出現頻度のソート用配列
    vector<tuple<Pixel, int, int>> most_common;
    // 生成率
    int omgc;

    double cos_sim(Pixel &p, const Pixel &q) {
        // コサイン類似度を計算
        double norm_a = 0.0;
        double norm_b = 0.0;
        double in_prod = 0.0;
        for (int i = 0; i < 3; ++i) {
            const double p_f = p[i] / 256.0;
            const double q_f = q[i] / 256.0;
            in_prod += p_f * q_f;
            norm_a += p_f * p_f;
            norm_b += q_f * q_f;
        }
        const double norm = sqrt(norm_a) * sqrt(norm_b);
        return (norm == 0.0) ? 1.0 : in_prod / norm;
    }

   public:
    Zoomg(const int h, const int w) : height(h), width(w), omgc(0) {
        // 初期化
        freq = vector<vector<map<Pixel, pair<int, int>>>>(
            h, vector<map<Pixel, pair<int, int>>>(w));
        image = Image(h, vector<Pixel>(w));
    }
    void add_image(const pybind11::array_t<int> &img_np) {
        // 画像を登録
        int order = 0;  // 出現順を保持する
        const int *img = (const int *)img_np.request().ptr;
        for (int h = 0; h < height; ++h) {
            for (int w = 0; w < width; ++w) {
                const Pixel p = Pixel(img + 3 * (h * width + w),
                                img + 3 * (h * width + w + 1));
                if (freq[h][w].find(p) != freq[h][w].end()) {
                    ++freq[h][w][p].first;
                } else {
                    freq[h][w][p] = make_pair(order++, 1);
                }
            }
        }
    }
    void generate_image(const double param) {
        // 画像を生成
        for (int h = 0; h < height; ++h) {
            for (int w = 0; w < width; ++w) {
                most_common.clear();
                for (auto it = freq[h][w].begin(); it != freq[h][w].end();
                     ++it) {
                    // ノイズは無視
                    if (it->second.first == 1) continue;
                    // 画素, 個数, 出現タイミングの順で配列に画素の情報を追加
                    most_common.emplace_back(it->first, it->second.first,
                                             it->second.second);
                }
                // 出現個数, 出現タイミングの順でソート
                sort(most_common.begin(), most_common.end(),
                     [](auto const &lhs, auto const &rhs) {
                         if (get<1>(lhs) == get<1>(rhs)) {
                             return get<2>(lhs) < get<2>(rhs);
                         } else {
                             return get<1>(lhs) < get<1>(rhs);
                         }
                     });
                const Pixel most = get<0>(*most_common.rbegin());
                [&]() -> void {
                    // コサイン類似度がparam未満で出現タイミングが最も早い画素を探索
                    for (auto p : most_common) {
                        if (cos_sim(get<0>(p), most) < param) {
                            ++omgc;
                            image[h][w] = get<0>(p);
                            return;
                        }
                    }
                    // 探索しても見つからなかった場合
                    image[h][w] = most;
                }();
            }
        }
    }
    Image get_image(void) {
        // 生成された画像を取得
        return image;
    }
    int get_omgc(void) {
        // 生成された画像を取得
        return omgc;
    }
    int get_height(void) {
        // 生成された画像を取得
        return height;
    }
    int get_width(void) {
        // 生成された画像を取得
        return width;
    }
    pair<int, int> get_shape(void) {
        // 生成された画像を取得
        return {height, width};
    }
    tuple<int, int, double> verify(const pybind11::array_t<int> &img_np) {
        // 生成した画像がどれくらい部屋を復元できているか検証
        const int *verify_img = (const int *)img_np.request().ptr;
        int ok = 0, ng = 0;
        for (int h = 0; h < height; ++h) {
            for (int w = 0; w < width; ++w) {
                const Pixel p = Pixel(verify_img + 3 * (h * width + w),
                                      verify_img + 3 * (h * width + w + 1));
                if (cos_sim(image[h][w], p) > 0.99) {
                    ++ok;
                } else {
                    ++ng;
                }
            }
        }
        return {ok, ng, ok / (double)(ok + ng)};
    }
};



PYBIND11_MODULE(zoomg, m) {
    pybind11::class_<Zoomg>(m, "Zoomg")
        .def(pybind11::init<const int, const int>())
        .def("add_image", &Zoomg::add_image)
        .def("generate_image", &Zoomg::generate_image)
        .def("get_image", &Zoomg::get_image)
        .def("get_omgc", &Zoomg::get_omgc)
        .def("verify", &Zoomg::verify)
        .def("get_height", &Zoomg::get_height)
        .def("get_width", &Zoomg::get_width)
        .def("get_shape", &Zoomg::get_shape);
}
