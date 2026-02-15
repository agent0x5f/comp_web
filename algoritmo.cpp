#include "algoritmo.h"
#include <wx/wx.h>
#include <filesystem>
#include <fstream>
#include <random>
#include <sstream>
#include <cmath>
#include <iomanip>
#include <algorithm>

#include "graficos.h"
using namespace std;

string datos; //guarda el dataset de entrada
int num_param = 0; //argc
vector<int>params; //args
vector<vector<int>> Algoritmo::matrizDatos;
vector<int> Algoritmo::listaIndices;
bool Algoritmo::verbo = true; //mostrar todas las calculaciones.
int Algoritmo::seed = 1;
int num_clases = 0; //cuantas clases existen actualmente.
double Algoritmo::umbral = 0.5;


string Algoritmo::procesarEntrada(string const& path, wxTextCtrl* salida) {
    std::ifstream archivo(path);
    if (!archivo.is_open()) return "Error al abrir";

    matrizDatos.clear();
    string linea;

    while (getline(archivo, linea)) {
        // Quitamos espacios en blanco accidentales al inicio y final
        linea.erase(0, linea.find_first_not_of(" \t\r\n"));

        // Comentarios: Si la línea está vacía o empieza con '@', pero NO es el umbral
        if (linea.empty()) continue;
        if (linea[0] == '@' && linea.find("umbral") == string::npos) continue;

        // Extracción del Umbral: Buscamos la etiqueta "@umbral:"
        if (linea.find("@umbral:") != string::npos) {
            try {
                // Buscamos la posición después de los dos puntos
                size_t posDospuntos = linea.find(":") + 1;
                string valorStr = linea.substr(posDospuntos);
                // Convertimos el resto de la línea a double
                Algoritmo::umbral = std::stod(valorStr);
                //FIXME: no se muestra en el log por el orden de ejecución
                if (salida) log("Umbral actualizado: " + std::to_string(Algoritmo::umbral) + "\n", salida);
            } catch (...) {
                if (salida) log("Error: Formato de umbral incorrecto en el archivo.\n", salida);
            }
            continue; // No agregamos esta línea a la matriz de datos
        }

        // 4. Lectura de coordenadas (X, Y)
        stringstream ss(linea);
        string valor;
        vector<int> filaActual;

        while (getline(ss, valor, ',')) {
            try {
                filaActual.push_back(stoi(valor));
            } catch (...) { /* Ignorar basura */ }
        }

        if (!filaActual.empty()) {
            matrizDatos.push_back(filaActual);
        }
    }

    archivo.close();
    listaIndices.assign(matrizDatos.size(), -1);

    std::filesystem::path p(path);
    return p.filename().string();
}

void Algoritmo::ejecutarCalculo(wxTextCtrl* out) {
    if (out) {
        std::fill(listaIndices.begin(), listaIndices.end(), -1); //reinicio mi vector de clases.
        num_clases = 0; // Reiniciamos el contador de grupos encontrados
        int n = obtenerIndiceAleatorio();
        listaIndices[n]=0; //el elemento n es la clase 0
        //zona de impresion a la "consola"
        if (!matrizDatos.empty()) {
            std::stringstream ss;
            ss << std::fixed << std::setprecision(2) << Algoritmo::umbral;
            std::string resultado = ss.str();
            log("Umbral: "+resultado+'\n',out);
            log("Matriz cargada: " + to_string(matrizDatos.size()) + " filas x " + to_string(matrizDatos[0].size()) + " columnas.\n",out);
            log("Seleccionando primer grupo #"+std::to_string(n)+": " + logM(n)+'\n',out);
            int lejano = obtenerMasLejano(n, out); //calculamos el más lejano para el segundo grupo.
            listaIndices[lejano] = ++num_clases; //es la clase '1'
            log("Seleccionando segundo grupo #"+std::to_string(lejano)+": "+logM(lejano)+'\n',out);

        }

    }
}

//recibe el índice y retorna un string con todos los elementos en formato x, y...
string Algoritmo::logM(const int pos) {
    string msg = std::to_string(matrizDatos[pos][0])+" ,"+ std::to_string(matrizDatos[pos][1]);
    return msg;
}

void Algoritmo::log(const string& msg,wxTextCtrl *out) {
    if (out) {
        out->AppendText(msg);
        out->Update();
    }
}
int Algoritmo::obtenerIndiceAleatorio() {
    if (matrizDatos.empty()) return -1;
    // 1. Inicializamos el motor con la semilla que capturamos del textbox
    std::mt19937 generador(seed);
    //Aquí podria poner un msgbox para decir que la semilla debe ser <= al número de renglones.

    // 2. Definimos el rango: de 0 al total de filas menos 1
    std::uniform_int_distribution<int> distribucion(0, matrizDatos.size() - 1);
    // 3. Generamos el número
    return distribucion(generador);
}

int Algoritmo::obtenerMasLejano(int indiceReferencia,wxTextCtrl *out) {
    if (verbo && out) {
        log("Calculando distancias...\n",out);
    }

    int masLejano = 0;
    double maxDistanciaSq = -1.0;

    // 1. Validaciones de seguridad
    // Verificamos que la matriz no esté vacía y que el índice solicitado sea válido
    if (matrizDatos.empty() || indiceReferencia < 0 || indiceReferencia >= (int)matrizDatos.size()) {
        return 0;
    }

    // 2. Extraer coordenadas del punto de referencia
    // Asumimos que cada vector interno tiene al menos [0] para X y [1] para Y
    int xRef = matrizDatos[indiceReferencia][0];
    int yRef = matrizDatos[indiceReferencia][1];

    // 3. Recorrer la matriz para comparar distancias
    for (int i = 0; i < (int)matrizDatos.size(); ++i) {
        // No es estrictamente necesario saltarse a sí mismo,
        // ya que la distancia será 0 y no superará el máximo, pero es más limpio.
        if (i == indiceReferencia) continue;

        if (matrizDatos[i].size() >= 2) {
            int xActual = matrizDatos[i][0];
            int yActual = matrizDatos[i][1];

            // Cálculo de distancia euclides
            // sqrt(x2 - x1)^2 + (y2 - y1)^2
            double distSq = sqrt(pow(xActual - xRef, 2) + pow(yActual - yRef, 2));

            //Aquí podemos mutear el log con el boton verboso, falta implementar
            if (verbo && out) {
                // Creamos un flujo de string para formatear
                std::stringstream ss;
                ss << std::fixed << std::setprecision(2) << distSq;
                // Ahora construimos el mensaje usando ss.str()
                string mensaje = "Dist: " + std::to_string(xActual) + ", "+std::to_string((yActual))+" | " + std::to_string(xRef)+", "+std::to_string((yRef)) + " = " + ss.str() + "\n";
                log(mensaje, out);
            }
            if (distSq > maxDistanciaSq) {
                maxDistanciaSq = distSq;
                masLejano = i;
            }
        }
    }

    return masLejano;
}