#pragma once
// Copyright (c) 2006 by Volodymyr Myrnyy (Vladimir Mirnyi)
// Copyright (c) 2014-2024 Joel de Guzman. All rights reserved.
// References:
// https://www.drdobbs.com/cpp/a-simple-and-efficient-fft-implementatio/199500857
// https://gfft.sourceforge.net/
// https://www.eetimes.com/a-simple-and-efficient-fft-implementation-in-c-part-i/
// https://www.eetimes.com/a-simple-and-efficient-fft-implementation-in-c-part-ii/

#include <cmath>

namespace Qdsp {
namespace detail {

constexpr auto pi = 3.14159265358979323846;
constexpr auto sin_cos_accuracy = 40;

// The sine and cosine values calculated using series expansion
// M - Current term in the series expansion
// N - Final term in the series expansion
// B - Denominator for the angle in radians
// A - Numerator for the angle in radians
template <class T, unsigned M, unsigned N, unsigned B, unsigned A>
constexpr T sin_cos_series() {
  if constexpr (M == N) {
    return 1.0;
  } else {
    auto x = A * pi / B;
    return 1.0 - x * x / M / (M + 1) * sin_cos_series<T, M + 2, N, B, A>();
  }
}

template <class T, unsigned B, unsigned A> constexpr T sin() {
  return (A * pi / B) * sin_cos_series<T, 2, sin_cos_accuracy, B, A>();
}

template <class T, unsigned B, unsigned A> constexpr T cos() {
  return sin_cos_series<T, 1, sin_cos_accuracy - 1, B, A>();
}

template <class T, std::size_t N> struct danielson_lanczos {
  danielson_lanczos<T, N / 2> next;

  void apply(T *data) {
    next.apply(data);
    next.apply(data + N);

    constexpr auto sina = -sin<T, N, 1>();
    constexpr auto sinb = -sin<T, N, 2>();

    T wtemp = sina;
    T wpr = -2.0 * wtemp * wtemp;
    T wpi = sinb;
    T wr = 1.0;
    T wi = 0.0;
    for (std::size_t i = 0; i < N; i += 2) {
      T tempr = data[i + N] * wr - data[i + N + 1] * wi;
      T tempi = data[i + N] * wi + data[i + N + 1] * wr;
      data[i + N] = data[i] - tempr;
      data[i + N + 1] = data[i + 1] - tempi;
      data[i] += tempr;
      data[i + 1] += tempi;

      wtemp = wr;
      wr += wr * wpr - wi * wpi;
      wi += wi * wpr + wtemp * wpi;
    }
  }
};

// The Danielson-Lanczos algorithm specialization for N=4.
template <class T> struct danielson_lanczos<T, 4> {
  void apply(T *data) {
    T tr = data[2];
    T ti = data[3];
    data[2] = data[0] - tr;
    data[3] = data[1] - ti;
    data[0] += tr;
    data[1] += ti;
    tr = data[6];
    ti = data[7];
    data[6] = data[5] - ti;
    data[7] = tr - data[4];
    data[4] += tr;
    data[5] += ti;

    tr = data[4];
    ti = data[5];
    data[4] = data[0] - tr;
    data[5] = data[1] - ti;
    data[0] += tr;
    data[1] += ti;
    tr = data[6];
    ti = data[7];
    data[6] = data[2] - tr;
    data[7] = data[3] - ti;
    data[2] += tr;
    data[3] += ti;
  }
};

// The Danielson-Lanczos algorithm specialization for N=2.
template <class T> struct danielson_lanczos<T, 2> {
  void apply(T *data) {
    T tr = data[2];
    T ti = data[3];
    data[2] = data[0] - tr;
    data[3] = data[1] - ti;
    data[0] += tr;
    data[1] += ti;
  }
};

// The Danielson-Lanczos algorithm specialization for N=1.
template <class T> struct danielson_lanczos<T, 1> {
  void apply(T *data) {}
};

template <class T, std::size_t N> inline void scramble(T *data) {
  int j = 1;
  for (int i = 1; i < 2 * N; i += 2) {
    if (j > i) {
      std::swap(data[j - 1], data[i - 1]);
      std::swap(data[j], data[i]);
    }
    int m = N;
    while (m >= 2 && j > m) {
      j -= m;
      m >>= 1;
    }
    j += m;
  }
}
} // namespace detail

template <std::size_t N, class T> inline void fft(T *data) {
  detail::danielson_lanczos<T, N> recursion;
  detail::scramble<T, N>(data);
  recursion.apply(data);
}

template <std::size_t N, class T> inline void ifft(T *data) {
  constexpr std::size_t _2n = N * 2;
  // Swap the real and imaginary parts of the i-th and (N-i)-th complex
  // numbers.
  for (auto i = 1; i < N / 2; ++i) {
    auto _2i = 2 * i;
    std::swap(data[2 * i], data[_2n - _2i]);
    std::swap(data[2 * i + 1], data[(_2n + 1) - _2i]);
  }
  // Perform FFT in-situ
  fft<N>(data);
  // Normalize the data by dividing each element by N.
  for (auto i = 0; i < 2 * N; ++i) {
    data[i] /= N;
  }
}

template <std::size_t N, class T> inline void absfft(T *data) {
  // Perform FFT in place
  fft<N>(data);

  // Compute the magnitude of the DC component (frequency 0)
  data[0] = std::abs(data[0]);

  for (std::size_t i = 1; i < N / 2; ++i) {
    auto real = data[i];
    auto imag = data[N - i];
    data[i] = std::sqrt(real * real + imag * imag);
  }

  // Compute the magnitude of the Nyquist frequency component
  data[N / 2] = std::abs(data[N / 2]);
}
} // namespace Qdsp
