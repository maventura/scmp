
long double xMax = 1.0;
long double yMax = 1.0;
long double tMax = 4.0;
long double nu = 0.1; //viscosidad. Era 0.1.
long double rho = 1.0;  //densidad
long double dx = (1.0 / 20.0);
long double dy = (1.0 / 20.0);
long double dt = 0.0001;
int nX = round(xMax / dx) + 1;
int nY = round(yMax / dy) + 1;
int nT = round(tMax / dt) + 1;
long double al = 0.5;
bool upwind = false;
long double fixedPointError = 0.000001;
long double minFixedPointIters = 10;
bool debug = true;
long double xc = xMax / 2;
long double yc = yMax / 2;
long double rMax = 0.4 * min(xMax, yMax) / 2.0;
long double rMin = 0.1 * min(xMax, yMax) / 2.0;
long double fanTurns = 1.0;
long double pi = atan(1) * 4;
unsigned int stepsUntilPrint = 35000;
long double F = 2.0;
