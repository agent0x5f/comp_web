#include "graficos.h"
#include "algoritmo.h"
#include "graficador.h"
#include <wx/wx.h>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <random>
using namespace std;

MyFrame::MyFrame() : wxFrame(nullptr, wxID_ANY, "Programa", wxPoint(50, 50), wxSize(1280, 720)) {
    auto* panel = new wxPanel(this, wxID_ANY); // El panel puede seguir siendo local si no lo vas a modificar después

    cargar_archivo = new wxButton(panel, wxID_ANY, "Cargar Archivo", wxPoint(10, 10), wxSize(150, 30));
    cargar_archivo->Bind(wxEVT_BUTTON, &MyFrame::OnOpenExplorer, this);
    textbox2 = new wxTextCtrl(panel, wxID_ANY, "", wxPoint(10,50), wxSize(250,600), wxTE_READONLY | wxTE_MULTILINE | wxBORDER_SIMPLE);
    calcula = new wxButton(panel,wxID_ANY, "Calcula", wxPoint(300, 10));
    calcula->Bind(wxEVT_BUTTON, &MyFrame::OnCalculaClick, this);
   // checkbox1 = new wxCheckBox(panel,wxID_ANY, "Explicado", wxPoint(400,15));
    etiqueta1 = new wxStaticText(panel, wxID_ANY, "Semilla: ", wxPoint(500,15));
    textbox1 = new wxTextCtrl(panel, wxID_ANY, "1", wxPoint(550,10),wxSize(60,20),wxBORDER_SIMPLE);
    textbox1->Bind(wxEVT_TEXT, &MyFrame::OnEscritura, this);
    consola = new wxTextCtrl(panel,wxID_ANY,"", wxPoint(270,50), wxSize(350,600),wxTE_READONLY | wxTE_MULTILINE | wxBORDER_SIMPLE);
    canvas = new MyGraphCanvas(panel, wxPoint(630, 50), wxSize(600, 600));
    grafica = new wxButton(panel, wxID_ANY, "Grafica", wxPoint(680, 10));
    exporta = new wxButton(panel, wxID_ANY, "Exporta", wxPoint(1080, 10));

    wxFrameBase::CreateStatusBar();
}

void MyFrame::OnOpenExplorer(const wxCommandEvent& event) {
    wxUnusedVar(event);
    // Filtramos
    wxFileDialog openFileDialog(this, "Selecciona un archivo", "", "",
                       R"(*.txt;*.csv;)",
                       wxFD_OPEN | wxFD_FILE_MUST_EXIST);

    if (openFileDialog.ShowModal() == wxID_OK) {
        string path = openFileDialog.GetPath().ToStdString();

        // 1. Llamamos al algoritmo y lo imprimimos en el visualizador
        //procesar entrada también guarda los datos en un string.
        string nombreParaMostrar = Algoritmo::procesarEntrada(path);
        filesystem::path p(path);
        ifstream archivo(p);
        stringstream buffer;
        buffer << archivo.rdbuf();
        string datos = buffer.str();
        textbox2->SetValue(datos);
        log("Datos cargados\n",consola);
        log("Semilla predeterminada: 1\n",consola);
    }
}

void MyFrame::OnCalculaClick(wxCommandEvent& event) {
    //Feedback visual en la barra de estado
    SetStatusText("Cálculo ejecutado desde Algoritmo.");
    log("==============================\n",consola);
    log("Calculando...\n",consola);
    // 2. Llamamos al algoritmo y le pasamos nuestra consola
    Algoritmo::ejecutarCalculo(consola);

}

void MyFrame::OnEscritura(wxCommandEvent& event){
    // Obtenemos el texto del textbox que disparó el evento
    wxString texto = event.GetString();

    if (!texto.IsEmpty()) {
        int valorTemporal;
        if (texto.ToInt(&valorTemporal)) {
            // Actualizamos la variable en la clase Algoritmo
            Algoritmo::seed = (int)valorTemporal;
            log(("Semilla actualizada: "+std::to_string(valorTemporal)+'\n'),consola);
         //   log("sem mem: "+ std::to_string(Algoritmo::seed),consola);
        }
    }
}

void MyFrame::log(string msg, wxTextCtrl *out) {
    out->AppendText(msg);
}

