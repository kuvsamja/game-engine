#ifndef VEC3_HPP
#define VEC3_HPP

#include <vector>

template <typename T> 
class vec2 {
  public:
    T e[2];
    
    vec2() : e{0,0} {}
    vec2(T e0, T e1) : e{e0, e1} {}

    T x() const { return e[0]; }
    T y() const { return e[1]; }

    T& x() { return e[0]; }
    T& y() { return e[1]; }

    vec2 operator-() const { return vec2(-e[0], -e[1]); }
    T operator[](int i) const { return e[i]; }
    T& operator[](int i) { return e[i]; }

    vec2& operator+=(const vec2& v) {
        e[0] += v.e[0];
        e[1] += v.e[1];
        return *this;
    }

    vec2& operator*=(T t) {
        e[0] *= t;
        e[1] *= t;
        return *this;
    }

    vec2& operator/=(T t) {
        return *this *= 1/t;
    }

    bool operator==(const vec2& other) const {
        return this->x() == other.x() && this->y() == other.y();
    }

    template <typename U>
    explicit operator vec2<U>() const {
        return vec2<U>(static_cast<U>(e[0]), static_cast<U>(e[1]));
    }
};

template <typename T>
using point2 = vec2<T>;

template <typename T>
inline vec2<T> operator+(const vec2<T>& u, const vec2<T>& v) {
    return vec2<T>(u.e[0] + v.e[0], u.e[1] + v.e[1]);
}

template <typename T>
inline vec2<T> operator-(const vec2<T>& u, const vec2<T>& v) {
    return vec2<T>(u.e[0] - v.e[0], u.e[1] - v.e[1]);
}

template <typename T>
inline vec2<T> operator*(const vec2<T>& u, const vec2<T>& v) {
    return vec2<T>(u.e[0] * v.e[0], u.e[1] * v.e[1]);
}

template <typename T>
inline vec2<T> operator*(T t, const vec2<T>& v) {
    return vec2<T>(t*v.e[0], t*v.e[1]);
}

template <typename T>
inline vec2<T> operator*(const vec2<T>& v, T t) {
    return t * v;
}

template <typename T>
inline vec2<T> operator/(const vec2<T>& v, T t) {
    return (1/t) * v;
}

template <typename T>
inline vec2<T> abs(const vec2<T>& v) {
    return vec2<T>(std::abs(v.x()), std::abs(v.y()));
}



#endif