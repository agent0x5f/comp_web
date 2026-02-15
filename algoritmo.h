#ifndef ALGORITMO_H
#define ALGORITMO_H

#include <string>
#include <vector>
class wxTextCtrl;

class Algoritmo {
public:
    static std::vector<std::vector<int>> matrizDatos;
    static std::vector<int>listaIndices;
    static int seed;
    static bool verbo;
    static double umbral;
    static std::string procesarEntrada(const std::string& path, wxTextCtrl* salida = nullptr);
    static void ejecutarCalculo(wxTextCtrl *out);
    static std::string logM(int pos);
    static void log(const std::string& msg,wxTextCtrl *out);
    static int obtenerIndiceAleatorio();
    static int obtenerMasLejano(int, wxTextCtrl *out);
};

#endif