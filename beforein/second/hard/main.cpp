#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <filesystem>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb/stb_image_write.h>

struct Image{
    int w=0;
    int h=0;
    int c=0;
    std::vector<uint8_t> px;

    std::size_t pixels() const{
        return static_cast<std::size_t>(w)* h;
    }
    bool empty() const{return w<=0 || h<=0||c<=0;}
};

static Image loadImage(const std::string& path) {
    Image im;
    int w = 0, h = 0, n = 0;
    std::uint8_t* data = stbi_load(path.c_str(), &w, &h, &n, 3);
    if (!data){
         std::cerr << "[错误] 读取失败: " << path << "\n"
                  << "       原因: " << stbi_failure_reason() << "\n";
        return im;
    }
      im.w = w;
    im.h = h;
    im.c = 3;
    im.px.assign(data, data + static_cast<std::size_t>(w) * h * 3);
    stbi_image_free(data);           
    return im;
}

static void saveImage(const Image& im, const std::string& path) {
    stbi_write_png(path.c_str(), im.w, im.h, im.c, im.px.data(), im.w * im.c);
    std::cout << "  保存: " << path << "\n";
}
static Image toGray(const Image& src) {
    Image g;
    g.w = src.w;
    g.h = src.h;
    g.c = 1;
    g.px.resize(src.pixels());

    for (int y = 0; y < src.h; ++y) {
        for (int x = 0; x < src.w; ++x) {
            const std::uint8_t* p = &src.px[(static_cast<std::size_t>(y) * src.w + x) * 3];
            const double v = 0.299 * p[0] + 0.587 * p[1] + 0.114 * p[2];
            g.px[static_cast<std::size_t>(y) * src.w + x] =
                static_cast<std::uint8_t>(std::lround(std::clamp(v, 0.0, 255.0)));
        }
    }
    return g;
}

static int otsuThreshold(const Image& gray) {
    long long hist[256] = {0};
    for (std::uint8_t v : gray.px) ++hist[v];

    const long long total = static_cast<long long>(gray.px.size());
    double sumAll = 0.0;
    for (int i = 0; i < 256; ++i) sumAll += static_cast<double>(i) * hist[i];

    double sumBack = 0.0;      // 背景部分的灰度总和
    long long wBack = 0;       // 背景像素数
    double bestVar = -1.0;
    int bestT = 0;

    for (int t = 0; t < 256; ++t) {
        wBack += hist[t];
        if (wBack == 0) continue;
        const long long wFore = total - wBack;
        if (wFore == 0) break;

        sumBack += static_cast<double>(t) * hist[t];
        const double mBack = sumBack / static_cast<double>(wBack);
        const double mFore = (sumAll - sumBack) / static_cast<double>(wFore);
        const double var = static_cast<double>(wBack) * static_cast<double>(wFore) *
                           (mBack - mFore) * (mBack - mFore);
        if (var > bestVar) {
            bestVar = var;
            bestT = t;
        }
    }
    return bestT;
}

static Image binarize(const Image& gray, int T) {
    Image b;
    b.w = gray.w;
    b.h = gray.h;
    b.c = 1;
    b.px.resize(gray.pixels());
    for (std::size_t i = 0; i < gray.px.size(); ++i)
        b.px[i] = (gray.px[i] > T) ? 255 : 0;
    return b;
}

static Image toGray(const Image& src);
static long long countNonZero(const Image& img);
static double meanOf(const Image& img);
static long long countForeground(const Image& img);
// 统计非零像素
static long long countNonZero(const Image& img) {
    long long count = 0;
    for (size_t i = 0; i < img.px.size(); ++i) {
        if (img.px[i] != 0) count++;
    }
    return count;
}

// 计算平均灰度
static double meanOf(const Image& img) {
    if (img.px.empty()) return 0.0;
    double sum = 0;
    for (size_t i = 0; i < img.px.size(); ++i) {
        sum += img.px[i];
    }
    return sum / img.px.size();
}

// 统计前景像素（统计值为255的像素）
static long long countForeground(const Image& img) {
    long long count = 0;
    for (size_t i = 0; i < img.px.size(); ++i) {
        if (img.px[i] == 255) count++;
    }
    return count;
}

// ============================================================================
//  6. 几何变换：ROI 裁剪 / 缩放
// ============================================================================

// 取 (x, y) 处第 k 个通道的像素值（k 默认 0；灰度图只有 1 个通道）
static inline std::uint8_t at(const Image& im, int x, int y, int k = 0) {
    return im.px[(static_cast<std::size_t>(y) * im.w + x) * im.c + k];
}
static inline void setAt(Image& im, int x, int y, int k, std::uint8_t v) {
    im.px[(static_cast<std::size_t>(y) * im.w + x) * im.c + k] = v;
}

// ① ROI 裁剪：从左上角 (x0, y0) 起裁出 cw x ch 的一块，返回新图（原图不改）
//    x0/y0/cw/ch 越界时自动夹紧到图像范围内，保证不会访问越界内存
static Image cropROI(const Image& src, int x0, int y0, int cw, int ch) {
    Image out;
    if (src.empty()) return out;

    x0 = std::clamp(x0, 0, src.w - 1);
    y0 = std::clamp(y0, 0, src.h - 1);
    cw = std::clamp(cw, 1, src.w - x0);
    ch = std::clamp(ch, 1, src.h - y0);

    out.w = cw;
    out.h = ch;
    out.c = src.c;
    out.px.resize(static_cast<std::size_t>(cw) * ch * src.c);
    for (int y = 0; y < ch; ++y)
        for (int x = 0; x < cw; ++x)
            for (int k = 0; k < src.c; ++k)
                setAt(out, x, y, k, at(src, x0 + x, y0 + y, k));
    return out;
}

// ② 缩放 - 最近邻：目标像素直接抄最近的源像素，速度快，放大后边缘有锯齿
static Image resizeNearest(const Image& src, int nw, int nh) {
    Image dst;
    if (src.empty() || nw <= 0 || nh <= 0) return dst;

    dst.w = nw;
    dst.h = nh;
    dst.c = src.c;
    dst.px.resize(static_cast<std::size_t>(nw) * nh * src.c);

    const double sx = static_cast<double>(src.w) / nw;
    const double sy = static_cast<double>(src.h) / nh;
    for (int y = 0; y < nh; ++y) {
        const int syy = std::min(static_cast<int>((y + 0.5) * sy), src.h - 1);
        for (int x = 0; x < nw; ++x) {
            const int sxx = std::min(static_cast<int>((x + 0.5) * sx), src.w - 1);
            for (int k = 0; k < src.c; ++k)
                setAt(dst, x, y, k, at(src, sxx, syy, k));
        }
    }
    return dst;
}

// ② 缩放 - 双线性：用周围 4 个源像素按距离加权，放大后更平滑（推荐）
static Image resizeBilinear(const Image& src, int nw, int nh) {
    Image dst;
    if (src.empty() || nw <= 0 || nh <= 0) return dst;

    dst.w = nw;
    dst.h = nh;
    dst.c = src.c;
    dst.px.resize(static_cast<std::size_t>(nw) * nh * src.c);

    const double sx = static_cast<double>(src.w) / nw;
    const double sy = static_cast<double>(src.h) / nh;

    for (int y = 0; y < nh; ++y) {
        for (int x = 0; x < nw; ++x) {
            // 目标像素中心 (x+0.5, y+0.5) 反向映射到源图坐标
            const double fx = (x + 0.5) * sx - 0.5;
            const double fy = (y + 0.5) * sy - 0.5;
            int x0 = static_cast<int>(std::floor(fx));
            int y0 = static_cast<int>(std::floor(fy));
            const double wx = fx - x0;      // 横向权重
            const double wy = fy - y0;      // 纵向权重
            int x1 = x0 + 1;
            int y1 = y0 + 1;
            // 边界处理：超出图像的部分夹到边缘像素
            x0 = std::clamp(x0, 0, src.w - 1);
            x1 = std::clamp(x1, 0, src.w - 1);
            y0 = std::clamp(y0, 0, src.h - 1);
            y1 = std::clamp(y1, 0, src.h - 1);

            for (int k = 0; k < src.c; ++k) {
                const double v00 = at(src, x0, y0, k);
                const double v10 = at(src, x1, y0, k);
                const double v01 = at(src, x0, y1, k);
                const double v11 = at(src, x1, y1, k);
                const double top = v00 * (1 - wx) + v10 * wx;
                const double bot = v01 * (1 - wx) + v11 * wx;
                const double v   = top * (1 - wy) + bot * wy;
                setAt(dst, x, y, k,
                      static_cast<std::uint8_t>(std::lround(std::clamp(v, 0.0, 255.0))));
            }
        }
    }
    return dst;
}

// ② 缩放 - 按倍数缩放（<1 缩小，>1 放大），内部走双线性，至少保留 1 像素
static Image scaleImage(const Image& src, double factor) {
    const int nw = std::max(1, static_cast<int>(std::lround(src.w * factor)));
    const int nh = std::max(1, static_cast<int>(std::lround(src.h * factor)));
    return resizeBilinear(src, nw, nh);
}

int main(int argc, char** argv) {
    const std::string inPath = (argc > 1) ? argv[1] : "data/test_image.png";
    const std::string outDir = (argc > 2) ? argv[2] : "output";
    std::filesystem::create_directories(outDir);
    const std::string D = outDir + "/";

    const auto t0 = std::chrono::steady_clock::now();
    auto msSince = [&t0] {
        return std::chrono::duration<double, std::milli>(
                   std::chrono::steady_clock::now() - t0).count();
    };

    std::cout << "==================================================\n";
    std::cout << "  图像处理基础程序：灰度化 + 二值化\n";
    std::cout << "==================================================\n";


  Image src = loadImage(inPath);
    if (src.px.empty()) { // 检查像素是否为空，防止读取失败
        std::cerr << "无法读取输入图片: " << inPath << "\n";
        return -1;
    }


 std::cout << "\n[2] 转灰度图\n";
    const Image gray = toGray(src);
    const long long grayNz = countNonZero(gray);
    std::cout << "  公式      : Gray = 0.299*R + 0.587*G + 0.114*B\n";
     const double total = static_cast<double>(gray.px.size()); 
    std::cout << "  灰度范围  : " << static_cast<int>(*std::min_element(gray.px.begin(), gray.px.end()))
              << " ~ " << static_cast<int>(*std::max_element(gray.px.begin(), gray.px.end())) << "\n";
    std::cout << "  平均灰度  : " << meanOf(gray) << "\n";
    std::cout << "  非零像素数: " << grayNz << "  ("
              << 100.0 * grayNz / total << "%)\n";
    saveImage(gray, D + "01_gray.png");

 
   
 std::cout << "\n[3] 二值化（阈值分割）\n";
    int T = 0;
    if (argc > 3) {
        T = std::stoi(argv[3]);
        std::cout << "  阈值      : " << T << "（由命令行指定）\n";
    } else {
        T = otsuThreshold(gray);
        std::cout << "  阈值      : " << T << "（Otsu 大津法自动计算）\n";
    }

    const Image bin = binarize(gray, T);
    const long long fg = countForeground(bin);
    std::cout << "  规则      : 灰度 > " << T << " 记为前景 255，否则记为 0\n";
    std::cout << "  前景像素  : " << fg << "  ("
              << 100.0 * fg / total << "%)\n";
    std::cout << "  背景像素  : " << total - fg << "  ("
              << 100.0 * (total - fg) / total << "%)\n";
    std::cout << "  非零像素数: " << countNonZero(bin) << "\n";
    saveImage(bin, D + "02_binary.png");

    // ---------------- 步骤 4：ROI 裁剪 ----------------
    std::cout << "\n[4] ROI 裁剪\n";
    // 命令行给了 4 个参数（x y 宽 高）就按指定矩形裁，否则默认裁原图居中一半
    int rx = src.w / 4;
    int ry = src.h / 4;
    int rw = src.w / 2;
    int rh = src.h / 2;
    const bool roiFromCli = (argc > 7);
    if (roiFromCli) {
        rx = std::stoi(argv[4]);
        ry = std::stoi(argv[5]);
        rw = std::stoi(argv[6]);
        rh = std::stoi(argv[7]);
    }

    const Image roi = cropROI(src, rx, ry, rw, rh);
    std::cout << "  裁剪区域  : x=" << rx << ", y=" << ry
              << ", 宽=" << rw << ", 高=" << rh
              << (roiFromCli ? "（命令行指定）" : "（默认：居中裁一半）") << "\n";
    std::cout << "  实际尺寸  : " << roi.w << " x " << roi.h << "\n";
    std::cout << "  像素占比  : " << std::fixed << std::setprecision(2)
              << 100.0 * static_cast<double>(roi.pixels()) / static_cast<double>(total) << "%\n";
    const long long roiNz = countNonZero(roi);
    std::cout << "  非零像素数: " << roiNz << "\n";
    saveImage(roi, D + "03_roi.png");

    // ---------------- 步骤 5：缩放 ----------------
    std::cout << "\n[5] 缩放\n";
    const double scaleArg = (argc > 8) ? std::stod(argv[8]) : 0.0;

    const Image half = scaleImage(src, 0.5);
    std::cout << "  缩小 0.5x : " << src.w << " x " << src.h << " -> "
              << half.w << " x " << half.h << "（双线性）\n";
    saveImage(half, D + "04_resize_half.png");

    const int dw = std::max(1, src.w * 2);
    const int dh = std::max(1, src.h * 2);
    const Image twice = resizeBilinear(src, dw, dh);
    std::cout << "  放大 2.0x : " << src.w << " x " << src.h << " -> "
              << twice.w << " x " << twice.h << "（双线性）\n";
    saveImage(twice, D + "04_resize_double.png");

    // 同样尺寸再用最近邻做一遍，便于肉眼对比插值差异
    const Image twiceNn = resizeNearest(src, dw, dh);
    std::cout << "  放大 2.0x : 同上尺寸（最近邻，边缘会有锯齿，用于对比）\n";
    saveImage(twiceNn, D + "04_resize_double_nn.png");

    // 把 ① 和 ② 串起来：先裁 ROI，再把 ROI 放大 2 倍
    const Image roiX2 = scaleImage(roi, 2.0);
    std::cout << "  ROI 再放大: " << roi.w << " x " << roi.h << " -> "
              << roiX2.w << " x " << roiX2.h << "（双线性）\n";
    saveImage(roiX2, D + "04_roi_x2.png");

    // 命令行第 8 个参数给了倍数，就再按该倍数缩一次
    if (scaleArg > 0.0) {
        const Image custom = scaleImage(src, scaleArg);
        std::cout << "  自定义倍数: " << scaleArg << "x -> "
                  << custom.w << " x " << custom.h << "（双线性）\n";
        saveImage(custom, D + "04_resize_custom.png");
    }

 std::cout << "\n[4] 处理前后统计汇总\n";
    const long long srcNz = countNonZero(src);
    std::cout << "  原图非零像素数    : " << srcNz <<"\n";
    std::cout << "  灰度图非零像素数  : " << grayNz << "\n";
    std::cout << "  二值图非零像素数  : " << fg << "   <- 阈值分割后保留的前景\n";
    std::cout << "  非零像素减少量    : " << srcNz - fg << "  ("
              << 100.0 * (srcNz - fg) / (srcNz == 0 ? 1 : srcNz) << "%)\n";
    std::cout << "\n  总耗时: " << std::setprecision(1) << msSince() << " ms\n";
    std::cout << "\n[完成] 结果已保存到 " << outDir << "/\n";
    return 0;
}
