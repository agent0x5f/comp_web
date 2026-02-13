#include "algoritmo.h"
#include <iostream>
#include <fstream>
#include <filesystem> // Requiere C++17

std::string Algoritmo::procesarImagen(const std::string& ruta) {
    // Usamos std::filesystem para extraer el nombre del archivo de la ruta completa
    std::filesystem::path p(ruta);
    std::string nombreArchivo = p.filename().string();

    // Aquí podrías añadir tu lógica de procesamiento de imagen
    // como la que usas en tus proyectos de dermatoscopia

    return nombreArchivo;
}
