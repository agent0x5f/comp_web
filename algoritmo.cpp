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
double Algoritmo::dist_mayor = 0; //elemento mayor distancia
vector<vector<float>> Algoritmo::matrizDistancias;
double dist_mayor_inicial = 0;
int limite = 0;//iteraciones de creacion de clases, solo para debug

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
            //ya tenemos los casos minimos, toca iterar el resto
            matrizDistancias.assign(matrizDatos.size(), vector<float>());
            while (dist_mayor > umbral && limite < 20) {
                max_min(out);
                ++limite;
            }
        }

    }
}

void Algoritmo::realizarClasificacion(wxTextCtrl *out) {
    // ... (tu código anterior)
    while (dist_mayor > umbral && limite < 20) {
        max_min(out);
        ++limite;
    }

    // --- INICIO FASE 2: CLASIFICACIÓN DE ELEMENTOS RESTANTES ---
    if (verbo && out) log("--- Asignando elementos restantes ---\n", out);

    for (int i = 0; i < (int)matrizDatos.size(); ++i) {
        if (listaIndices[i] == -1) { // Si el elemento no es un centro

            float distMinima = 999999.0;
            int claseAsignada = -1;

            // Buscar cuál de los centros existentes le queda más cerca
            for (int j = 0; j < (int)matrizDatos.size(); ++j) {
                if (listaIndices[j] != -1) { // Solo comparamos contra los que SÍ son centros

                    double d = sqrt(pow(matrizDatos[i][0] - matrizDatos[j][0], 2) +
                                    pow(matrizDatos[i][1] - matrizDatos[j][1], 2));

                    if (d < distMinima) {
                        distMinima = d;
                        claseAsignada = listaIndices[j]; // Tomamos la clase de ese centro
                    }
                }
            }

            // Le asignamos la clase ganadora
            listaIndices[i] = claseAsignada;
            if (verbo && out) {
                log("Elemento [" + to_string(matrizDatos[i][0]) + ", " + to_string(matrizDatos[i][1]) +
                    "] asignado a clase #" + to_string(claseAsignada) + "\n", out);
            }
        }
    }
    // --- FIN FASE 2 ---

    log("Clasificación finalizada.\n", out);
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
//toma elemento al azar
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

//recibe: elemento, retorna: valor a mayor distancia de este.
int Algoritmo::obtenerMasLejano(int indiceReferencia,wxTextCtrl *out) {
    if (verbo && out) {
        log("Calculando distancias...\n",out);
    }
    int masLejano = 0;
    double maxDistanciaSq = -1.0;
    // Verificamos que la matriz no esté vacía y que el índice solicitado sea válido
    if (matrizDatos.empty() || indiceReferencia < 0 || indiceReferencia >= (int)matrizDatos.size()) {
        return 0;
    }
    // Asumimos que cada vector interno tiene al menos [0] para X y [1] para Y
    int xRef = matrizDatos[indiceReferencia][0];
    int yRef = matrizDatos[indiceReferencia][1];
    // Recorrer la matriz para comparar distancias
    for (int i = 0; i < (int)matrizDatos.size(); ++i) {
        if (i == indiceReferencia) continue;

        if (matrizDatos[i].size() >= 2) {
            int xActual = matrizDatos[i][0];
            int yActual = matrizDatos[i][1];

            // Cálculo de distancia euclides   sqrt(x2 - x1)^2 + (y2 - y1)^2
            double distSq = sqrt(pow(xActual - xRef, 2) + pow(yActual - yRef, 2));
            //Aquí podemos mutear el log con el boton verboso
            if (verbo && out) {
                std::stringstream ss;// Creamos un flujo de string para formatear
                ss << std::fixed << std::setprecision(2) << distSq;
                // Ahora construimos el mensaje usando ss.str()
                string mensaje = "Dist: " + std::to_string(xActual) + ", "+std::to_string((yActual))+" | " + std::to_string(xRef)+", "+std::to_string((yRef)) + " = " + ss.str() + "\n";
                log(mensaje, out);
            }
            if (distSq > maxDistanciaSq) {
                maxDistanciaSq = distSq;
                masLejano = i;
                dist_mayor = distSq;
            }
        }
    }
    return masLejano;
}

//procede al resto de la generación del algoritmo ya tenemos las primeras dos clases.
//recordemos que listaIndices contiene las clases
//y que matrizDistancias

void Algoritmo::max_min(wxTextCtrl *out) {
    int ultimoCentro = -1;

    // Buscamos cuál fue el último centro agregado (el que tiene el índice de clase más alto)
    for(int i = 0; i < (int)listaIndices.size(); ++i) {
        if(listaIndices[i] == num_clases) {
            ultimoCentro = i;
            break;
        }
    }

    double maxDeLasMinimas = -1.0;
    int indiceCandidato = -1;

    // --- PASO 1: Actualizar matriz de distancias con el nuevo centro ---
    for (int i = 0; i < (int)matrizDatos.size(); ++i) {
        // Calculamos distancia del punto 'i' al 'ultimoCentro'
        double d = sqrt(pow(matrizDatos[i][0] - matrizDatos[ultimoCentro][0], 2) +
                        pow(matrizDatos[i][1] - matrizDatos[ultimoCentro][1], 2));

        // Guardamos en la matriz de distancias
        matrizDistancias[i].push_back(static_cast<float>(d));
    }

    // --- PASO 2: Buscar el siguiente candidato (MAX-MIN) ---
    for (int i = 0; i < (int)matrizDatos.size(); ++i) {
        // Solo evaluamos puntos que aún no son centros
        if (listaIndices[i] == -1) {

            // MIN: Buscamos la distancia más corta del punto 'i' a sus centros conocidos
            // Esto está almacenado en matrizDistancias[i]
            float distMinimaACentro = *std::min_element(matrizDistancias[i].begin(), matrizDistancias[i].end());

            // MAX: Queremos el punto que tenga la mayor de esas distancias mínimas
            if (distMinimaACentro > maxDeLasMinimas) {
                maxDeLasMinimas = distMinimaACentro;
                indiceCandidato = i;
            }
        }
    }

    // --- PASO 3: Validar contra el umbral ---
    // En el Max-Min original, el umbral suele ser una fracción de la distancia entre los primeros dos centros
    if (indiceCandidato != -1 && maxDeLasMinimas > (Algoritmo::umbral * dist_mayor_inicial)) {
        num_clases++;
        listaIndices[indiceCandidato] = num_clases;

        log("Generando clase #" + to_string(num_clases+1) + " : " + std::to_string(matrizDatos[indiceCandidato][0]) +
             ", " + std::to_string(matrizDatos[indiceCandidato][1]) + '\n', out);

        dist_mayor = maxDeLasMinimas; // Actualizamos para la condición del while
    } else {
        log("No se encontraron más centros que superen el umbral.\n", out);
        dist_mayor = 0; // Rompemos el bucle while
    }
    realizarClasificacion(out);
}

int Algoritmo::obtenerMasCercano(int indiceReferencia,wxTextCtrl *out) {
    int mas_cercano = 0;
    double minDistanciaSq = 999999.0;
    // Verificamos que la matriz no esté vacía y que el índice solicitado sea válido
    if (matrizDatos.empty() || indiceReferencia < 0 || indiceReferencia >= (int)matrizDatos.size()) {
        return 0;
    }
    // Asumimos que cada vector interno tiene al menos [0] para X y [1] para Y
    int xRef = matrizDatos[indiceReferencia][0];
    int yRef = matrizDatos[indiceReferencia][1];
    // Recorrer la matriz para comparar distancias
    for (int i = 0; i < (int)matrizDatos.size(); ++i) {
        if (i == indiceReferencia) continue;

        if (matrizDatos[i].size() >= 2) {
            int xActual = matrizDatos[i][0];
            int yActual = matrizDatos[i][1];

            // Cálculo de distancia euclides   sqrt(x2 - x1)^2 + (y2 - y1)^2
            double distSq = sqrt(pow(xActual - xRef, 2) + pow(yActual - yRef, 2));
            //Aquí podemos mutear el log con el boton verboso
            if (verbo && out) {
                std::stringstream ss;// Creamos un flujo de string para formatear
                ss << std::fixed << std::setprecision(2) << distSq;
                // Ahora construimos el mensaje usando ss.str()
                string mensaje = "Dist: " + std::to_string(xActual) + ", "+std::to_string((yActual))+" | " + std::to_string(xRef)+", "+std::to_string((yRef)) + " = " + ss.str() + "\n";
                log(mensaje, out);
            }
            if (distSq < minDistanciaSq) {
                minDistanciaSq = distSq;
                mas_cercano = i;
            }
        }
    }
    return mas_cercano;
}
