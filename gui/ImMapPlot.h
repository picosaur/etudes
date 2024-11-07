#include <imgui.h>
#include <cmath>
#include <numbers>

namespace ImMapPlot
{
    inline constexpr double PI{3.14159265358979323846};
    inline constexpr double PI2{PI * 2.0};
    inline constexpr double RAD{PI / 180.0};
    inline constexpr double DEG{180.0 / PI};
    inline constexpr double R_EARTH{6371e3};
    inline constexpr double RAD{PI / 180.0};
    inline constexpr int POW2[]{
        (1 << 0), (1 << 1), (1 << 2), (1 << 3), (1 << 4), (1 << 5), (1 << 6),
        (1 << 7), (1 << 8), (1 << 9), (1 << 10), (1 << 11), (1 << 12), (1 << 13),
        (1 << 14), (1 << 15), (1 << 16), (1 << 17), (1 << 18)};
    inline constexpr double MIN_LAT{-85.0};
    inline constexpr double MAX_LAT{+85.0};
    inline constexpr double MIN_LON{-179.9};
    inline constexpr double MAX_LON{+179.9};
    inline constexpr int MIN_ZOOM{0};
    inline constexpr int MAX_ZOOM{18};

    double dmsToDeg(double d, double m, const double s, const char nsew);
    double distance(double lat1, double lon1, double lat2, double lon2);
    double bearing(double lat1, double lon1, double lat2, double lon2);

    void midpoint(double &lat, double &lon, double lat1, double lon1, double lat2, double lon2);
    void destination(double &lat, double &lon, double lat1, double lon1,
                     double d, double b);
    void cartesian(double &x, double &y, double lat, double lon, double lat0, double lon0);

}