//
// Created by Miguel on 17/02/26.
//

#ifndef APPVISIONCENIDET_IO_H
#define APPVISIONCENIDET_IO_H

#include <wx/wx.h>

class io {
    public:
    static std::string procesarEntrada(const std::string& path, wxTextCtrl* salida = nullptr);

};


#endif //APPVISIONCENIDET_IO_H