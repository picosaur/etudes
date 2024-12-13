#pragma once
namespace EtOsm {
double Lon2X(double lon, int z = 0);
double Lat2Y(double lat, int z = 0);
double X2Lon(double x, int z = 0);
double Y2Lat(double y, int z = 0);
int Lon2Tx(double lon, int z = 0);
int Lat2Ty(double lat, int z = 0);

double Dms2Deg(double d, double m, double s, char nsew);

double Distance(double lat1, double lon1, double lat2, double lon2);
double Bearing(double lat1, double lon1, double lat2, double lon2);

void Midpoint(double &lat, double &lon, double lat1, double lon1, double lat2,
              double lon2);
void Destination(double &lat, double &lon, double lat1, double lon1, double d,
                 double b);
void Cartesian(double &x, double &y, double lat, double lon, double lat0,
               double lon0);
} // namespace EtOsm
