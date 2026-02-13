#include "algoritmo.h"
#include <iostream>
#include <fstream>
#include <filesystem> // Requiere C++17

#include "graficos.h"
using namespace std;

string Algoritmo::procesarImagen(const string& ruta) {
    // recibimos la ruta del file
    filesystem::path p(ruta);
    string nombreArchivo = p.filename().string();
    // Aquí podrías añadir tu lógica de procesamiento de imagen
    // como la que usas en tus proyectos de dermatoscopia
    return nombreArchivo;
}
