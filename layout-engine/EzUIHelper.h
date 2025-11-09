#pragma once

#include <algorithm>
#include <cstdint>

namespace EzUI {
  template<typename T>
  struct Point {
    T x, y;

    Point() : x(0), y(0) {}
    Point(T _x, T _y) : x(_x), y(_y) {}
  };

  template<typename T>
  struct Rect {
    T x, y, width, height;

    Rect() : x(0), y(0), width(0), height(0) {}
    Rect(T _x, T _y, T w, T h) : x(_x), y(_y), width(w), height(h) {}

    T left() const { return x; }
    T top() const { return y; }
    T right() const { return x + width; }
    T bottom() const { return y + height; }

    bool empty() const { return width <= 0 || height <= 0; }
  };

  using PointF = Point<float>;
  using RectF = Rect<float>;

  // 矩形与点是否相交（包含点在矩形边界）
  template<typename T>
  inline bool IsIntersect(const Point<T>& p, const Rect<T>& r) {
    return (p.x >= r.left() && p.x <= r.right() &&
      p.y >= r.top() && p.y <= r.bottom());
  }

  template<typename T>
  inline bool IsIntersect(const Rect<T>& a, const Rect<T>& b) {
    T nx = (std::max)(a.left(), b.left());
    T ny = (std::max)(a.top(), b.top());
    T nr = (std::min)(a.right(), b.right());
    T nb = (std::min)(a.bottom(), b.bottom());

    T nw = nr - nx;
    T nh = nb - ny;

    if (nw > 0 && nh > 0) {
      return true;
    }
    return false;
  }

  // -------------------- Color --------------------
  struct Color {
    uint8_t r, g, b, a;

    Color() : r(0), g(0), b(0), a(255) {} // 默认不透明黑色
    Color(uint8_t _r, uint8_t _g, uint8_t _b, uint8_t _a = 255)
      : r(_r), g(_g), b(_b), a(_a) {
    }
  };

  struct EdgeSizes {
    float left = 0;
    float right = 0;
    float top = 0;
    float bottom = 0;
  };
}

