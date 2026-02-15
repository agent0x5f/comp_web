#include "algoritmo.h"
#include <wx/wx.h>
#include <filesystem>
#include <fstream>
#include <random>
#include <sstream>
#include <cmath>
#include  <iomanip>

#include "graficos.h"
using namespace std;

string datos; //guarda el dataset de entrada
int num_param = 0; //argc
vector<int>params; //args
vector<vector<int>> matrizDatos;
// Inicializa el vector con tantos ceros como filas tenga tu matriz
vector<int> lista_indices;
bool Algoritmo::verbo = true; //mostrar todas las calculaciones.
int Algoritmo::seed = 1;
int num_clases = 0; //cuantas clases existen acualmente.
double Algoritmo::umbral = 0.5;

/**
 * Procesa la imagen (o archivo de datos) y actualiza la interfaz si se proporciona un puntero a un wxTextCtrl.
 * @param path Ruta del archivo seleccionado.
 * @param salida Puntero al wxTextCtrl donde se mostrarán los logs (opcional).
 * @return Un string con el nombre o estado del procesamiento.
 */
string Algoritmo::procesarEntrada(string const& path, wxTextCtrl* salida) {
    std::ifstream archivo(path);
    if (!archivo.is_open()) return "Error al abrir";
    matrizDatos.clear(); // Limpiamos datos previos
    string linea;
    while (getline(archivo, linea)) {
        if (linea.empty()) continue; // Ignora líneas vacías

        stringstream ss(linea);
        string valor;
        vector<int> filaActual;
        // Separamos por comas
        while (getline(ss, valor, ',')) {
            try {
                // convertimos a int y agregamos a la fila
                filaActual.push_back(stoi(valor));
            } catch (...) {
                log("Error al leer la fila",salida);
            }
        }

        if (!filaActual.empty()) {
            matrizDatos.push_back(filaActual); // Agregamos la fila a la matriz
        }
    }
    archivo.close();
    lista_indices.assign(matrizDatos.size(), 0);
    std::filesystem::path p(path);
    string nombreArchivo = p.filename().string();
    return p;
}

void Algoritmo::ejecutarCalculo(wxTextCtrl* out) {
    if (out) {
        std::fill(lista_indices.begin(), lista_indices.end(), 0); //reinicio mi vector de clases.
        int n = obtenerIndiceAleatorio();
        lista_indices[n]=0; //el elemento n es la clase 0
        //zona de impresion a la "consola"
        if (!matrizDatos.empty()) {
            log("Matriz cargada: " + to_string(matrizDatos.size()) + " filas x " + to_string(matrizDatos[0].size()) + " columnas.\n",out);
            log("Seleccionando primer grupo #"+std::to_string(n)+": " + logM(n)+'\n',out);
            int lejano = obtenerMasLejano(n, out); //calculamos el más lejano para el segundo grupo.
            lista_indices[lejano] = ++num_clases; //es la clase '1'
            log("Segundo grupo #"+std::to_string(lejano)+": "+logM(lejano)+'\n',out);

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
    }
}
int Algoritmo::obtenerIndiceAleatorio() {
    if (matrizDatos.empty()) return -1;
    // 1. Inicializamos el motor con la semilla que capturamos del textbox
    std::mt19937 generador(seed);
    //Aqui podria poner un msgbox para decir que la semilla debe ser <= al número de renglones.

    // 2. Definimos el rango: de 0 al total de filas menos 1
    std::uniform_int_distribution<int> distribucion(0, matrizDatos.size() - 1);
    // 3. Generamos el número
    return distribucion(generador);
}

int Algoritmo::obtenerMasLejano(int indiceReferencia,wxTextCtrl *out) {
    log("Calculando distancias...\n",out);
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

            // Cálculo de distancia al cuadrado (evitamos sqrt)
            // (x2 - x1)^2 + (y2 - y1)^2
            double distSq = sqrt(pow(xActual - xRef, 2) + pow(yActual - yRef, 2));

            //Aquí podemos mutear el log con el boton verboso
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