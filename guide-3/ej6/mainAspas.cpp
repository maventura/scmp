#include <iostream>
#include <vector>
#include <chrono> // for high_resolution_clock.
#include <math.h>
#include <sstream>

using namespace std;

using mat3 = vector<vector<vector<long double > > >;
using mat2 = vector<vector<long double > >;
using mat1 = vector<long double>;


void clearScreen() {
    cerr << string( 100, '\n' );
}

long double diff(mat2 &A, mat2 &B) {
    long double sum = 0;
    for (int i = 0; i < A.size(); ++i) {
        for (int j = 0; j < A[0].size(); ++j) {
            sum += pow(B[i][j] - A[i][j], 2);
        }
    }
    return sqrt(sum);
}

void printMat(mat2 m) {
    for (int i = 0; i < m.size(); ++i) {
        for (int j = 0; j < m[0].size(); ++j) {
            cout << m[i][j] << " " ;
        }
        cout << endl;
    }
    cout << endl;
}
void printAll(mat2 U0, mat2 U1, mat2 U2, mat2 V0, mat2 V1, mat2 V2, mat2 P0, mat2 P1, mat2 P2) {
    {
        cout << "U0: " << endl;
        printMat(U0);
        cout << endl;
        cout << "U1: " << endl;
        printMat(U1);
        cout << endl;
        cout << "U2: " << endl;
        printMat(U2);
        cout << endl;
        cout << "V0: " << endl;
        printMat(V0);
        cout << endl;
        cout << "V1: " << endl;
        printMat(V1);
        cout << endl;
        cout << "V2: " << endl;
        printMat(V2);
        cout << endl;
        cout << "P0: " << endl;
        printMat(P0);
        cout << endl;
        cout << "P1: " << endl;
        printMat(P1);
        cout << endl;
        cout << "P2: " << endl;
        printMat(P2);
        cout << endl;
    }
}

void setPBorders(mat2 &P0, mat2 &P1, mat2 &P2, int nX, int nY) {
    for (int c = 0; c < nX; ++c) {
        P0[c][0] = P0[c][1];
        P1[c][0] = P1[c][1];
        P2[c][0] = P2[c][1];

        P0[c][nY - 1] = P0[c][nY - 2];
        P1[c][nY - 1] = P1[c][nY - 2];
        P2[c][nY - 1] = P2[c][nY - 2];
    }
    for (int c = 0; c < nY; ++c) {
        P0[nX - 1][c] = P0[nX - 2][c];
        P1[nX - 1][c] = P1[nX - 2][c];
        P2[nX - 1][c] = P2[nX - 2][c];
        P0[0][c] = P0[1][c];
        P1[0][c] = P1[1][c];
        P2[0][c] = P2[1][c];
    }
}
/* EJERCICIO 6 DE PRACTICA 3
  Donde x, y ∈ [0; 2] y t > 0 son las variables espacial y temporal respectivamente. u(x, y, t) y v(x, y, t) son los
  componentes x-y de la velocidad del fluido dependiente del espacio y el tiempo. Se desea resolver el Liddriven
  cavity problem en el cual a un recipiente (2D) que contiene un determinado fluido se le desliza su tapa
  hacia uno de los lados, provocando cambios en el movimiento y la presi ́on del fluido. Para resolver el problema
  se cuenta con los siguiente datos: ν = 0.1. ρ=1. Condiciones iniciales u, v, p = 0 en todos lados. Condiciones
  de contorno u = 1 en y = 2 (la “tapa”). u, v = 0 en los demas bordes. ∂p/∂y = 0 en y = 0. p = 0 en y = 2. ∂p/∂x =0 en x = 0, 2
*/
//TODO: Para el informe poner ley de gubstavson y andal
//nmap para ver la red.


//TODO para el tp:
//1) poner todo condiciones de Neumann, excepto en un solo punto del borde donde la presion nula.
//Esto es comp ara tener un cero de referencia.
//2) dejarlo 2d
//3) Potencialmente podrian no ponerse  aspas
//4) Una sola aspa. recta.
//Agregar una fuerza como en channel flow(12) de lorena barba. Manejaria dos ecuaciones, una con la f y otra sin la f. aplico la de la f en un segmento que varia con el angulo, y la que no tiene la f en el resto del "mapa"
//el reactor peude ser circular o cuadrado.
int main() {
    long double xMax = 2.0;
    long double yMax = 2.0;
    long double tMax = 0.05;
    long double nu = 0.1; //viscosidad
    long double rho = 1.0;  //densidad
    long double dx = 1.0 / 20.0;
    long double dy = 1.0 / 20.0;
    long double dt = 0.00001;
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
    long double rMax = 0.8 * min(xMax, yMax) / 2.0;
    long double rMin = 0.3 * min(xMax, yMax) / 2.0;
    long double fanTurns = 2.0;
    long double fanAngle = 0.0;

    long double pi = atan(1) * 4;


    //0 es tiempo n-1, 1 es tiempo n, 2 es tiempo n+1
    //3 es simplemente las matrices auxiliares para no perder datos de el tiempo 2
    mat2 U0(nX, vector<long double>(nY));
    mat2 V0(nX, vector<long double>(nY));
    mat2 P0(nX, vector<long double>(nY));
    mat2 U1(nX, vector<long double>(nY));
    mat2 V1(nX, vector<long double>(nY));
    mat2 P1(nX, vector<long double>(nY));
    mat2 U2(nX, vector<long double>(nY));
    mat2 V2(nX, vector<long double>(nY));
    mat2 P2(nX, vector<long double>(nY));
    mat2 U2_old(nX, vector<long double>(nY));
    mat2 V2_old(nX, vector<long double>(nY));
    mat2 P2_old(nX, vector<long double>(nY));

    for (int i = 0; i < nX; ++i) {
        fill(U0[i].begin(), U0[i].end(), 0.0);
        fill(V0[i].begin(), V0[i].end(), 0.0);
        fill(P0[i].begin(), P0[i].end(), 0.0);
        fill(U1[i].begin(), U1[i].end(), 0.0);
        fill(V1[i].begin(), V1[i].end(), 0.0);
        fill(P1[i].begin(), P1[i].end(), 0.0);
        fill(U2[i].begin(), U2[i].end(), 0.0);
        fill(V2[i].begin(), V2[i].end(), 0.0);
        fill(P2[i].begin(), P2[i].end(), 0.0);
    }

    unsigned long int iter = 0;
    for (long double t = 0.0; t < tMax; t = t + dt) {
        //clearScreen();


        long double dFanAngle = fanTurns * 2 * pi / nT; //
        fanAngle += dFanAngle; //TODO: solo funciona en el 1er y tercer cuadrante.
        if(fanAngle > 2*pi) fanAngle = 0;

        cerr << 100 * t / tMax << "%" << endl ;
        if (isnan(U1[3][3])) {
            cerr << "ERROR: nan found" << endl;
            exit(EXIT_FAILURE);
        }
        printMat(U0);
        printMat(V0);
        //printMat(P0);
        mat2 tmp = U1;
        for (int k = 0; true; ++k) {
            tmp = U2;
            setPBorders(P0, P1, P2, nX, nY);

            //U2_old = U2;
            //V2_old = V2;
            //P2_old = P2;



            for (int i = 1; i < nX - 1; ++i) {
                for (int j = 1; j < nY - 1; ++j) { //las condiciones borde en Y=2 se respetan aca
                    iter++;
                    //printAll(U0,U1,U2,V0,V1,V2,P0,P1,P2);
                    //0 = n-1, 1 = n, 2 = n+1
                    //x = dx, y = dy, xx = d2x, yy = d2y.
                    long double U1x = (U1[i + 1][j] - U1[i - 1][j]) / (2.0 * dx);
                    long double U2x = (U2[i + 1][j] - U2[i - 1][j]) / (2.0 * dx);
                    long double U1y = (U1[i][j + 1] - U1[i][j - 1]) / (2.0 * dy);
                    long double U2y = (U2[i][j + 1] - U2[i][j - 1]) / (2.0 * dy);
                    long double U1xx = (U1[i + 1][j] - 2.0 * U1[i][j] + U1[i - 1][j]) / (dx * dx);
                    long double U2xx = (U2[i + 1][j] - 2.0 * U2[i][j] + U2[i - 1][j]) / (dx * dx);
                    long double U1yy = (U1[i][j + 1] - 2.0 * U1[i][j] + U1[i][j - 1]) / (dy * dy);
                    long double U2yy = (U2[i][j + 1] - 2.0 * U2[i][j] + U2[i][j - 1]) / (dy * dy);
                    long double P1x = (P1[i + 1][j] - P1[i - 1][j]) / (2.0 * dx);
                    long double P2x = (P2[i + 1][j] - P2[i - 1][j]) / (2.0 * dx);
                    long double P1y = (P1[i][j + 1] - P1[i][j - 1]) / (2.0 * dy);
                    long double P2y = (P2[i][j + 1] - P2[i][j - 1]) / (2.0 * dy);
                    long double V1x = (V1[i + 1][j] - V1[i - 1][j]) / (2.0 * dx);
                    long double V2x = (V2[i + 1][j] - V2[i - 1][j]) / (2.0 * dx);
                    long double V1y = (V1[i][j + 1] - V1[i][j - 1]) / (2.0 * dy);
                    long double V2y = (V2[i][j + 1] - V2[i][j - 1]) / (2.0 * dy);
                    long double V1xx = (V1[i + 1][j] - 2.0 * V1[i][j] + V1[i - 1][j]) / (dx * dx);
                    long double V2xx = (V2[i + 1][j] - 2.0 * V2[i][j] + V2[i - 1][j]) / (dx * dx);
                    long double V1yy = (V1[i][j + 1] - 2.0 * V1[i][j] + V1[i][j - 1]) / (dy * dy);
                    long double V2yy = (V2[i][j + 1] - 2.0 * V2[i][j] + V2[i][j - 1]) / (dy * dy);
                    long double P1xx = (P1[i + 1][j] - 2.0 * P1[i][j] + P1[i - 1][j]) / (dx * dx);
                    long double P1yy = (P1[i][j + 1] - 2.0 * P1[i][j] + P1[i][j - 1]) / (dy * dy);
                    long double P2yy = (P2[i][j + 1] - 2.0 * P2[i][j] + P2[i][j - 1]) / (dy * dy);

                    //TODO: U0, V0, P0, are not used if we use forward scheme in time.
                    /*long double oU1x = U1x;
                    long double oU2x = U2x;
                    long double oV1x = V1x;
                    long double oV2x = V2x;
                    long double oU1y = U1y;
                    long double oU2y = U2y;
                    long double oV1y = V1y;
                    long double oV2y = V2y;*/

                    if (upwind) {
                        if (U1[i][j] > 0) {
                            U1x = (U1[i + 1][j] - U1[i][j]) / dt;
                            V1x = (V1[i + 1][j] - V1[i][j]) / dt;
                            U2x = (U2[i + 1][j] - U2[i][j]) / dt;
                            V2x = (V2[i + 1][j] - V2[i][j]) / dt;
                        } else {
                            U1x = (U1[i][j] - U1[i - 1][j]) / dt;
                            V1x = (V1[i][j] - V1[i - 1][j]) / dt;
                            U2x = (U2[i][j] - U2[i - 1][j]) / dt;
                            V2x = (V2[i][j] - V2[i - 1][j]) / dt;
                        }
                        if (V1[i][j] > 0) {
                            U1y = (V1[i][j + 1] - V1[i][j]) / dt;
                            V1y = (V1[i][j + 1] - V1[i][j]) / dt;
                            U2y = (V2[i][j + 1] - V2[i][j]) / dt;
                            V2y = (V2[i][j + 1] - V2[i][j]) / dt;
                        } else {
                            U1y = (V1[i][j] - V1[i][j - 1]) / dt;
                            V1y = (V1[i][j] - V1[i][j - 1]) / dt;
                            U2y = (V2[i][j] - V2[i][j - 1]) / dt;
                            V2y = (V2[i][j] - V2[i][j - 1]) / dt;
                        }
                    }

                    /* cout << "k = " << k << endl;
                     cout << "U1x = " << fabs(oU1x - U1x) << endl;
                     cout << "U2x = " << fabs(oU2x - U2x) << endl;
                     cout << "V1x = " << fabs(oV1x - V1x) << endl;
                     cout << "V2x = " << fabs(oV2x - V2x) << endl;
                     cout << "U1y = " << fabs(oU1y - U1y) << endl;
                     cout << "U2y = " << fabs(oU2y - U2y) << endl;
                     cout << "V1y = " << fabs(oV1y - V1y) << endl;
                     cout << "V2y = " << fabs(oV2y - V2y) << endl;
                    */


                    //U2[i][j] = U0[i][j] + 2.0 * dt * (-U1[i][j] * (al * U1x + (1.0 - al) * U2x ) - V1[i][j] * (al * U1y + (1.0 - al) * U2y)
                    //                                  - (1.0 / rho) * (al * P1x + (1.0 - al) * P2x) + nu * (al * U1xx + (1.0 - al) * U2xx + al * U1yy + (1.0 - al) * U2yy));
                    //V2[i][j] = V0[i][j] + 2.0 * dt * (-U1[i][j] * (al * V1x + (1.0 - al) * V2x ) - V1[i][j] * (al * V1y + (1.0 - al) * V2y)
                    //                                  - (1.0 / rho) * (al * P1y + (1.0 - al) * P2y) + nu * (al * V1xx + (1.0 - al) * V2xx + al * V1yy + (1.0 - al) * V2yy));
                    //P2[i][j] = (P2[i - 1][j] / 2.0) + (P2[i + 1][j] / 2.0) + (dx * dx) / (2.0 * al - 2.0) * (-al * P1xx - al * P1yy - (1.0 - al) * P2yy
                    //           - rho * ( pow((al * U1x + (1 - al) * U2x), 2) + 2.0 * (al * U1y + (1.0 - al) * U2y) * (al * V1x + (1.0 - al) * V2x) + pow((al * V1y + (1.0 - al) * V2y), 2)));


                    //new discretization:
                    //other discretization (adv diff for time back for spatial)
                    U2[i][j] = U1[i][j] - U1[i][j] * (dt / dx) * (U1[i][j] - U1[i - 1][j]) - V1[i][j] * (dt / dy) * (U1[i][j] - U1[i][j - 1])
                               - (dt / (rho * 2 * dx)) * (P1[i + 1][j] - P1[i - 1][j])
                               + nu * ((dt / (dx * dx)) * (U1[i + 1][j] - 2 * U1[i][j] + U1[i - 1][j]) + (dt / (dy * dy)) * (U1[i][j + 1] - 2 * U1[i][j] + U1[i][j - 1]));

                    V2[i][j] = V1[i][j] - U1[i][j] * (dt / dx) * (V1[i][j] - V1[i - 1][j]) - V1[i][j] * (dt / dy) * (V1[i][j] - V1[i][j - 1])
                               - (dt / (rho * 2 * dy)) * (P1[i][j + 1] - P1[i][j - 1])
                               + nu * ((dt / (dx * dx)) * (V1[i + 1][j] - 2 * V1[i][j] + V1[i - 1][j]) + (dt / (dy * dy)) * (V1[i][j + 1] - 2 * V1[i][j] + V1[i][j - 1]));

                    long double finalTerm = (1 / dt) * (U1x + V1y) - U1x * U1x - 2 * U1y * V1x - V1y * V1y;
                    P2[i][j] = ((P1[i + 1][j] + P1[i - 1][j]) * dy * dy + (P1[i][j + 1] + P1[i][j - 1]) * dx * dx) * (1.0 / (2 * (dx * dx + dy * dy)));
                    P2[i][j] -= (rho * dx * dx * dy * dy / (2 * dx * dx + 2 * dy * dy)) * finalTerm;

                    long double x = i * dx - xc;
                    long double y = j * dy - yc;
                    long double theta = atan(y / x);
                    long double beta = theta + (pi / 2.0);

                    long double r = sqrt(x * x + y * y);
                    long double tangSpeed = dFanAngle * r;

                    //long double Fu = r * (cos(theta + dTheta) - cos(theta));
                    //long double Fv = r * (sin(theta + dTheta) - sin(theta));
                    //cerr << "(Fu, Fv) = (" << Fu << ", " << Fv << ")" << endl;
                    long double F = 2.0;
                    long double Fu = F * r * cos(beta);
                    long double Fv = F * r * sin(beta);
                    if (i < nX / 2.0) {
                        Fu = -Fu;
                        Fv = -Fv;
                    }
                    if ( fabs(theta - fanAngle) < 0.1  && r > rMin && r < rMax) {
                        U2[i][j] += Fu * dt;
                        V2[i][j] += Fv * dt;
                    }
                }
            }

            break; // TODO: Explicit converges in one iteration. When finished, use alpha for this.
            long double matDiff = diff(tmp, U2);
            cerr << "iter = " << iter << endl;
            cerr << "matDiff = " << matDiff << endl;

            if ((matDiff < fixedPointError) && (k > minFixedPointIters)) {
                cerr << "out & matDiff = " << matDiff << endl;
                if (debug && matDiff != 0) cerr << "out at k = " << k << ", and diff = " << matDiff << endl;
                break;
            } else if (k > 4000) {
                cerr << "ERROR: unstable." << endl;
                exit(EXIT_FAILURE);
            }

        }

        U0 = U1;
        U1 = U2;
        V0 = V1;
        V1 = V2;
        P0 = P1;
        P1 = P2;
    }
}