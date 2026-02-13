#include "graficos.h"
#include "algoritmo.h"
#include "graficador.h"
#include <wx/wx.h>
#include <filesystem>
#include <fstream>
#include <sstream>
using namespace std;

MyFrame::MyFrame() : wxFrame(nullptr, wxID_ANY, "Programa", wxPoint(50, 50), wxSize(1280, 720)) {
    auto* panel = new wxPanel(this, wxID_ANY); // El panel puede seguir siendo local si no lo vas a modificar después

    cargar_archivo = new wxButton(panel, wxID_ANY, "Cargar Archivo", wxPoint(10, 10), wxSize(150, 30));
    cargar_archivo->Bind(wxEVT_BUTTON, &MyFrame::OnOpenExplorer, this);

    textbox2 = new wxTextCtrl(panel, wxID_ANY, "", wxPoint(10,50), wxSize(250,600), wxTE_READONLY | wxTE_MULTILINE | wxBORDER_SIMPLE);
    calcula = new wxButton(panel,wxID_ANY, "Calcula", wxPoint(300, 10));
    checkbox1 = new wxCheckBox(panel,wxID_ANY, "Explicado", wxPoint(400,15));
    etiqueta1 = new wxStaticText(panel, wxID_ANY, "Semilla: ", wxPoint(500,15));
    texbox1 = new wxTextCtrl(panel, wxID_ANY, "1", wxPoint(550,10),wxSize(20,20),wxBORDER_SIMPLE);
    textbox3 = new wxTextCtrl(panel,wxID_ANY,"", wxPoint(270,50), wxSize(400,600),wxTE_READONLY | wxTE_MULTILINE | wxBORDER_SIMPLE);

    canvas = new MyGraphCanvas(panel, wxPoint(680, 50), wxSize(550, 600));


    wxFrameBase::CreateStatusBar();
}

void MyFrame::OnOpenExplorer(const wxCommandEvent& event) {
    wxUnusedVar(event);
    // Filtramos para que solo se vean imágenes
    wxFileDialog openFileDialog(this, "Selecciona un archivo", "", "",
                       R"(*.txt;*.csv;)",
                       wxFD_OPEN | wxFD_FILE_MUST_EXIST);

    if (openFileDialog.ShowModal() == wxID_OK) {
        string path = openFileDialog.GetPath().ToStdString();

        // 1. Llamamos al algoritmo y guardamos lo que nos devuelve
        string nombreParaMostrar = Algoritmo::procesarImagen(path);
        filesystem::path p(path);
        ifstream archivo(p);
        stringstream buffer;
        buffer << archivo.rdbuf();
        string datos = buffer.str();
        textbox2->SetValue(datos);

        // 2. Creamos la ventana emergente con el nombre
       //wxMessageBox("El algoritmo procesa: " + nombreParaMostrar,"Procesamiento Exitoso",wxOK | wxICON_INFORMATION);
    }
}
