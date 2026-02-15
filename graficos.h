#ifndef GRAFICOS_H
#define GRAFICOS_H

#include <wx/wx.h>
#include "graficador.h"

class MyFrame : public wxFrame {
public:
    MyFrame();
    void OnOpenExplorer(const wxCommandEvent& event);
    void OnCalculaClick(wxCommandEvent& event);

    void OnEscritura(wxCommandEvent &event);

    static void log(std::string msg, wxTextCtrl *out);

private:
    wxButton* cargar_archivo; //explorador del sistema
    wxStaticText* etiqueta1; //semilla
    wxTextCtrl* textbox1; //semilla
    wxTextCtrl* textbox2; //archivo entrada
    wxButton* calcula; //run
    wxTextCtrl* consola; //terminal
 //   wxCheckBox* checkbox1; //verbo checkbox
    MyGraphCanvas *canvas;
    wxButton* grafica; //dibuja en el canvas
    wxButton* exporta; //imprime el cvs con las clases resultantes
};

#endif
