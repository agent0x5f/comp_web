#ifndef GRAFICOS_H
#define GRAFICOS_H

#include <wx/wx.h>
#include "graficador.h"

class MyFrame : public wxFrame {
public:
    MyFrame();
    void OnOpenExplorer(const wxCommandEvent& event);
private:
    wxButton* cargar_archivo; //explorador del sistema
    wxStaticText* etiqueta1; //semilla
    wxTextCtrl* texbox1; //semilla
    wxTextCtrl* textbox2; //archivo entrada
    wxButton* calcula; //run
    wxTextCtrl* textbox3; //terminal
    wxCheckBox* checkbox1; //verbo checkbox
    MyGraphCanvas *canvas;
};

#endif
