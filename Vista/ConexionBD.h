#pragma once
#include <mysql.h>
#include <iostream>

using namespace std;

class ConexionBD {
private:
    MYSQL* conector;

public:
    // Constructor
    ConexionBD() {
        conector = nullptr;
    }

    void abrir_conexion() {
        conector = mysql_init(0);
        // Conexión directa usando tus credenciales de MySQL
        conector = mysql_real_connect(conector, "localhost", "root", "eber_yt19", "sistema_ventas", 3306, NULL, 0);

        if (conector) {
            cout << "[OK] Conexion exitosa a la base de datos (MySQL API)." << endl;
        }
        else {
            cout << "[!] Error al conectar: " << mysql_error(conector) << endl;
        }
    }

    // Esta es la función que te pide el main.cpp
    MYSQL* getConector() {
        return conector;
    }

    void cerrar_conexion() {
        if (conector) {
            mysql_close(conector);
            conector = nullptr;
            cout << "[INFO] Conexion con MySQL cerrada." << endl;
        }
    }
};