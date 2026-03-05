#include <wx/textctrl.h>
#include "chainmap.h"
#include <wx/wx.h>
#include <vector>
#include <string>
#include <random>
#include <sstream>
#include <iomanip>
#include <algorithm>

using namespace std;

vector<vector<double>> chainmap::matrizDatos; 
vector<int> chainmap::listaIndices;  
bool chainmap::verbo = true; 
int chainmap::seed = 1;
int chainmap::num_clases = 0; 
double chainmap::umbral = 0.0;

string chainmap::a2decimal(double number) {
    stringstream ss;
    ss << std::fixed << std::setprecision(2) << number;
    return ss.str();
}

string chainmap::logM(const int pos) {
    string msg = "[";
    for (size_t i = 0; i < matrizDatos[pos].size(); ++i) {
        msg += a2decimal(matrizDatos[pos][i]);
        if (i < matrizDatos[pos].size() - 1) msg += ", ";
    }
    msg += "]";
    return msg;
}

void chainmap::log(const string& msg, wxTextCtrl *out) {
    if (out) {
        out->AppendText(msg);
        out->Update();
    }
}
//obtenemos el primer elemento al azar
int chainmap::obtenerIndiceAleatorio() {
    if (matrizDatos.empty()) return -1;
    std::mt19937 generador(seed);  //semilla del textbox
    std::uniform_int_distribution<int> distribucion(0, (int)matrizDatos.size() - 1);//Definimos el rango: de 0 al total de filas menos 1
    return distribucion(generador); //retornamos el número generado
}
//calcula la distancia entre dos puntos de n dimensiones
double chainmap::calcularDistancia(const std::vector<double>& p1, const std::vector<double>& p2) {
    double suma = 0.0;
    size_t dimensiones = std::min(p1.size(), p2.size());
    for (size_t i = 0; i < dimensiones; ++i) {
        double diff = (p1[i]) - (p2[i]);
        suma += diff * diff;
    }
    return std::sqrt(suma);
}

// Busca el vecino más cercano que todavia no tiene clase asignada
int chainmap::obtenerCercanoNoVisitado(int indiceActual, wxTextCtrl *out) {
    int mas_cercano = -1;
    double minDistancia = 999999.0;

    for (int i = 0; i < (int)matrizDatos.size(); ++i) { //recorro los elementos
        // Ignoramos el punto actual y los que ya tienen clase (!= -1)
        if (i == indiceActual || listaIndices[i] != -1) continue;

        if (!matrizDatos[i].empty()) {
            double dist = calcularDistancia(matrizDatos[i], matrizDatos[indiceActual]);
            //calculo la distancia y veo si es el más cercano actualmente.
            if (dist < minDistancia) {
                minDistancia = dist;
                mas_cercano = i;
            }
        }
    }
    return mas_cercano;
}

// Función principal que ejecuta el algoritmo chainmap
void chainmap::ejecutar(wxTextCtrl* out) {
    if (matrizDatos.empty() || !out) {
        log("Error: No hay datos para procesar.\n", out);
        return;
    }

    listaIndices.assign(matrizDatos.size(), -1);
    num_clases = 0;
    log("Umbral: " + a2decimal(umbral) + "\n", out);
    // Elegimos el primer punto al azar
    int puntoActual = obtenerIndiceAleatorio();
    listaIndices[puntoActual] = num_clases; // Lo asignamos a la Clase 0
    log("Inicio en #" + to_string(puntoActual) + " " + logM(puntoActual) + " -> Clase 1\n", out);

    // Recorrer la cadena saltando al más cercano
    // El ciclo corre (N-1) veces porque ya asignamos el primer punto
    for (size_t i = 0; i < matrizDatos.size()-1; ++i) {
        int siguientePunto = obtenerCercanoNoVisitado(puntoActual, out);
        if (siguientePunto == -1) break; // Si no hay más puntos disponibles

        // Calculamos la distancia del salto
        double distanciaSalto = calcularDistancia(matrizDatos[puntoActual], matrizDatos[siguientePunto]);

        if (verbo) {log("Dist. salto: #" + to_string(puntoActual) + " -> #" + to_string(siguientePunto) + " es: " + a2decimal(distanciaSalto) + "\n", out);}

        if (distanciaSalto > umbral) {  // Evaluar el salto contra el umbral
            num_clases++; // El salto es muy largo, creamos nueva clase
            if (verbo) {
                log("Salto supera el umbral: ("+ a2decimal(distanciaSalto)+" > "+ a2decimal(umbral)+ ") -> Clase " + to_string(num_clases + 1) + "\n",out);
            }
        }
        // Asignar clase y preparar el siguiente salto
        listaIndices[siguientePunto] = num_clases;
        puntoActual = siguientePunto; 
    }
    log("Terminado: " + to_string(num_clases + 1) + " clases generadas.\n", out);
}