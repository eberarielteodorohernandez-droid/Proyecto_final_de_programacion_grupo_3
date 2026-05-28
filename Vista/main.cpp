#define NOMINMAX 
#include <iostream>
#include <limits>
#include <string>
#include <vector>
#include <iomanip>
#include <ctime>
#include <cctype>  // Para isalpha, isdigit, etc.
#include <mysql.h>
#include "ConexionBD.h" 

using namespace std;

// ===================================================
// 0. ENUM DE VALIDACIÓN DE CARACTERES
// ===================================================
enum class ValidacionTipo {
    GENERICO,
    SOLO_LETRAS,
    SOLO_DIGITOS,
    ALFANUMERICO_ESPACIOS,
    NIT_FORMAT,
    DIRECCION,
    ALFANUMERICO_ESPACIOS_PUNTUACION,
    NOMBRE_ARCHIVO,
    FECHA
};

// Valida si una cadena contiene solo los caracteres permitidos según el tipo
bool validarCadena(const string& str, ValidacionTipo tipo) {
    for (char c : str) {
        switch (tipo) {
        case ValidacionTipo::SOLO_LETRAS:
            if (!isalpha(c) && c != ' ') return false;
            break;
        case ValidacionTipo::SOLO_DIGITOS:
            if (!isdigit(c)) return false;
            break;
        case ValidacionTipo::ALFANUMERICO_ESPACIOS:
            if (!isalnum(c) && c != ' ') return false;
            break;
        case ValidacionTipo::NIT_FORMAT:
            if (!isupper(c) && !isdigit(c) && c != '-' && c != '/') return false;
            break;
        case ValidacionTipo::DIRECCION:
            if (!isalnum(c) && c != ' ' && c != '#' && c != '-' && c != '.' && c != ',')
                return false;
            break;
        case ValidacionTipo::ALFANUMERICO_ESPACIOS_PUNTUACION:
            if (!isalnum(c) && c != ' ' && c != '.' && c != ',' && c != '-' && c != '#' && c != '(' && c != ')')
                return false;
            break;
        case ValidacionTipo::NOMBRE_ARCHIVO:
            if (!isalnum(c) && c != '_' && c != '-') return false;
            break;
        case ValidacionTipo::FECHA:
            if (!isdigit(c) && c != '-') return false;
            break;
        default: // GENERICO
            break;
        }
    }
    return true;
}

// ===================================================
// 1. FUNCIONES DE VALIDACIÓN Y SEGURIDAD
// ===================================================
string sanitizarString(ConexionBD& cn, const string& input) {
    if (input.empty()) return "";
    char* buffer = new char[input.length() * 2 + 1];
    mysql_real_escape_string(cn.getConector(), buffer, input.c_str(), input.length());
    string safeString(buffer);
    delete[] buffer;
    return safeString;
}

int leerEntero(const string& mensaje) {
    int valor;
    while (true) {
        cout << mensaje;
        if (cin >> valor) {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return valor;
        }
        cout << "[!] Error: Debe ingresar un numero entero valido. No se aceptan letras.\n";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
}

double leerDouble(const string& mensaje) {
    double valor;
    while (true) {
        cout << mensaje;
        if (cin >> valor) {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return valor;
        }
        cout << "[!] Error: Debe ingresar un numero decimal valido.\n";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
}

string leerCadena(const string& mensaje, int maxLen = 255, ValidacionTipo tipo = ValidacionTipo::GENERICO) {
    string valor;
    while (true) {
        cout << mensaje;
        getline(cin, valor);
        if (!valor.empty()) {
            if (valor.length() > maxLen) {
                cout << "[!] Error: Has excedido el limite de " << maxLen << " caracteres. Intenta de nuevo.\n";
                continue;
            }
            if (tipo != ValidacionTipo::GENERICO && !validarCadena(valor, tipo)) {
                cout << "[!] Error: El campo contiene caracteres no permitidos. Intenta de nuevo.\n";
                continue;
            }
            return valor;
        }
        cout << "[!] Error: El campo no puede estar vacio.\n";
    }
}

string obtenerFechaActual() {
    time_t t = time(nullptr);
    tm nowStruct;
#if defined(_MSC_VER)
    localtime_s(&nowStruct, &t);
#else
    localtime_r(&t, &nowStruct);
#endif
    char buffer[11];
    strftime(buffer, sizeof(buffer), "%Y-%m-%d", &nowStruct);
    return string(buffer);
}

// ===================================================
// 2. PROGRAMACIÓN ORIENTADA A OBJETOS (POO)
// ===================================================
class Persona {
protected:
    string nombres;
    string apellidos;
    string telefono;
    int genero; // 1 = M, 0 = F

public:
    Persona(string n, string a, string t, int g) : nombres(n), apellidos(a), telefono(t), genero(g) {}
    string getNombres() { return nombres; }
    string getApellidos() { return apellidos; }
};

class Cliente : public Persona {
private:
    string nit;
    string correo;

public:
    Cliente(string n, string a, string t, int g, string ni, string c)
        : Persona(n, a, t, g), nit(ni), correo(c) {
    }

    bool registrarEnBD(ConexionBD& cn) {
        string n_safe = sanitizarString(cn, nombres);
        string a_safe = sanitizarString(cn, apellidos);
        string nit_safe = sanitizarString(cn, nit);
        string t_safe = sanitizarString(cn, telefono);
        string c_safe = sanitizarString(cn, correo);

        string query = "INSERT INTO clientes (nombres, apellidos, nit, genero, telefono, correo_electronico, fecha_ingreso) VALUES ('"
            + n_safe + "', '" + a_safe + "', '" + nit_safe + "', " + to_string(genero) + ", '" + t_safe + "', '" + c_safe + "', NOW());";

        return mysql_query(cn.getConector(), query.c_str()) == 0;
    }
};

struct ProductoDetalle {
    int idProducto;
    string nombre;
    int cantidad;
    double precioUnitario;
    double subtotal;
};

// ===================================================
// 3. PROTOTIPOS DE FUNCIONES DEL SISTEMA
// ===================================================
void menuOperaciones(string entidad, ConexionBD& cn);
void ejecutarInsercion(string entidad, ConexionBD& cn);
void ejecutarLectura(string entidad, ConexionBD& cn);
void ejecutarActualizacion(string entidad, ConexionBD& cn);
void ejecutarEliminacion(string entidad, ConexionBD& cn);
void mostrarTabla(string query, int numColumnas, ConexionBD& cn);

void insertarMarca(ConexionBD& cn);
void insertarProducto(ConexionBD& cn);
void insertarProveedor(ConexionBD& cn);
void insertarCliente(ConexionBD& cn);
void insertarPuesto(ConexionBD& cn);
void insertarEmpleado(ConexionBD& cn);

void moduloVentasInteligente(ConexionBD& cn);
void insertarCompra(ConexionBD& cn);

// ===================================================
// 4. FUNCIÓN PRINCIPAL
// ===================================================
int main() {
    setlocale(LC_ALL, "spanish");
    setlocale(LC_NUMERIC, "C");
    ConexionBD cn = ConexionBD();
    cn.abrir_conexion();

    if (cn.getConector() == nullptr) {
        cout << "[!] Falla critica: El sistema no puede iniciar sin conexion a la base de datos." << endl;
        return 1;
    }

    int opcion = 0;
    do {
        cout << "\n=================================================" << endl;
        cout << "        SUPERMERCADO XYZ - MENU PRINCIPAL        " << endl;
        cout << "=================================================" << endl;
        cout << "1. Modulo de Ventas y Facturacion " << endl;
        cout << "2. Modulo de Compras e Inventario" << endl;
        cout << "3. Gestionar Marcas" << endl;
        cout << "4. Gestionar Productos" << endl;
        cout << "5. Gestionar Proveedores" << endl;
        cout << "6. Gestionar Clientes" << endl;
        cout << "7. Gestionar Puestos" << endl;
        cout << "8. Gestionar Empleados" << endl;
        cout << "9. Salir" << endl;

        opcion = leerEntero("Seleccione una opcion: ");

        switch (opcion) {
        case 1: moduloVentasInteligente(cn); break;
        case 2: menuOperaciones("Compras", cn); break;
        case 3: menuOperaciones("Marcas", cn); break;
        case 4: menuOperaciones("Productos", cn); break;
        case 5: menuOperaciones("Proveedores", cn); break;
        case 6: menuOperaciones("Clientes", cn); break;
        case 7: menuOperaciones("Puestos", cn); break;
        case 8: menuOperaciones("Empleados", cn); break;
        case 9:
            cout << "\nCerrando sistema y desconectando base de datos..." << endl;
            break;
        default: cout << "[!] Opcion no valida. Intente de nuevo." << endl;
        }
    } while (opcion != 9);

    cn.cerrar_conexion();
    return 0;
}

// ===================================================
// 5. MÓDULO INTELIGENTE DE VENTAS (MAESTRO-DETALLE)
// ===================================================
void moduloVentasInteligente(ConexionBD& cn) {
    cout << "\n=============================================" << endl;
    cout << "           NUEVA VENTA / FACTURACION         " << endl;
    cout << "=============================================" << endl;

    string nit = leerCadena("Ingrese NIT del cliente (Escriba 'C/F' para Consumidor Final): ", 12, ValidacionTipo::NIT_FORMAT);
    int idCliente = 0;
    string nombreCliente = "Consumidor Final";

    for (auto& c : nit) c = toupper(c);

    if (nit != "C/F" && nit != "CF") {
        string qNIT = "SELECT id_cliente, nombres, apellidos FROM clientes WHERE nit = '" + sanitizarString(cn, nit) + "';";
        if (mysql_query(cn.getConector(), qNIT.c_str()) == 0) {
            MYSQL_RES* res = mysql_store_result(cn.getConector());
            if (mysql_num_rows(res) > 0) {
                MYSQL_ROW row = mysql_fetch_row(res);
                idCliente = atoi(row[0]);
                nombreCliente = string(row[1]) + " " + string(row[2]);
                cout << "[I] Cliente encontrado: " << nombreCliente << endl;
            }
            else {
                cout << "\n[!] Cliente no registrado. Iniciando registro automatico..." << endl;
                string nom = leerCadena("Nombres (Max 60): ", 60, ValidacionTipo::SOLO_LETRAS);
                string ape = leerCadena("Apellidos (Max 60): ", 60, ValidacionTipo::SOLO_LETRAS);
                int gen = leerEntero("Genero (1=Masculino, 0=Femenino): ");
                string tel = leerCadena("Telefono (Max 25): ", 25, ValidacionTipo::SOLO_DIGITOS);
                string correo = leerCadena("Correo (Max 45): ", 45);

                Cliente nuevoCliente(nom, ape, tel, gen, nit, correo);
                if (nuevoCliente.registrarEnBD(cn)) {
                    cout << "[OK] Cliente registrado exitosamente." << endl;
                    nombreCliente = nom + " " + ape;
                    mysql_query(cn.getConector(), "SELECT LAST_INSERT_ID();");
                    MYSQL_RES* resID = mysql_store_result(cn.getConector());
                    MYSQL_ROW rowID = mysql_fetch_row(resID);
                    idCliente = atoi(rowID[0]);
                    mysql_free_result(resID);
                }
                else {
                    cout << "[!] Error al registrar cliente: " << mysql_error(cn.getConector()) << endl;
                    mysql_free_result(res);
                    return;
                }
            }
            mysql_free_result(res);
        }
    }
    else {
        cout << "[I] Facturando como Consumidor Final." << endl;
        idCliente = 1;
    }

    int idEmpleado = leerEntero("Ingrese ID de Empleado (Cajero): ");
    int noFactura = 1;
    char serie = 'A';
    string fechaFactura = obtenerFechaActual();

    mysql_query(cn.getConector(), "SELECT MAX(no_factura) FROM ventas;");
    MYSQL_RES* resFact = mysql_store_result(cn.getConector());
    MYSQL_ROW rowFact = mysql_fetch_row(resFact);
    if (rowFact[0] != NULL) {
        noFactura = atoi(rowFact[0]) + 1;
    }
    mysql_free_result(resFact);

    vector<ProductoDetalle> carrito;
    double totalVenta = 0.0;
    int numProductos = leerEntero("\nCantidad de productos diferentes a facturar: ");

    for (int i = 0; i < numProductos; i++) {
        cout << "\n--- Producto " << (i + 1) << " ---" << endl;
        int idProd = leerEntero("ID Producto: ");

        string qProd = "SELECT producto, precio_venta, existencia FROM productos WHERE id_producto = " + to_string(idProd);
        if (mysql_query(cn.getConector(), qProd.c_str()) == 0) {
            MYSQL_RES* resP = mysql_store_result(cn.getConector());
            if (mysql_num_rows(resP) > 0) {
                MYSQL_ROW rowP = mysql_fetch_row(resP);
                string nombreProd = rowP[0];
                double precioUnitario = atof(rowP[1]);
                int stock = atoi(rowP[2]);

                cout << "Producto: " << nombreProd << " | Precio: Q" << precioUnitario << " | Stock: " << stock << endl;

                int cant = leerEntero("Cantidad a llevar: ");
                if (cant > stock) {
                    cout << "[!] Stock insuficiente. La venta se ha cancelado." << endl;
                    mysql_free_result(resP);
                    return;
                }

                double sub = cant * precioUnitario;
                carrito.push_back({ idProd, nombreProd, cant, precioUnitario, sub });
                totalVenta += sub;

                cout << "-> Subtotal acumulado: Q" << sub << endl;
            }
            else {
                cout << "[!] Producto no existe. Intente de nuevo." << endl;
                i--;
            }
            mysql_free_result(resP);
        }
    }

    mysql_query(cn.getConector(), "START TRANSACTION");

    string qVenta = "INSERT INTO ventas (no_factura, serie, fecha_factura, id_cliente, id_empleado, fecha_ingreso) VALUES ("
        + to_string(noFactura) + ", '" + string(1, serie) + "', '" + fechaFactura + "', " + to_string(idCliente) + ", " + to_string(idEmpleado) + ", NOW());";

    if (mysql_query(cn.getConector(), qVenta.c_str()) != 0) {
        cout << "[!] Error al guardar venta: " << mysql_error(cn.getConector()) << endl;
        mysql_query(cn.getConector(), "ROLLBACK");
        return;
    }

    int idVenta = 0;
    mysql_query(cn.getConector(), "SELECT LAST_INSERT_ID();");
    MYSQL_RES* resVenta = mysql_store_result(cn.getConector());
    MYSQL_ROW rowVenta = mysql_fetch_row(resVenta);
    idVenta = atoi(rowVenta[0]);
    mysql_free_result(resVenta);

    for (const auto& item : carrito) {
        string qDetalle = "INSERT INTO ventas_detalle (id_venta, id_producto, cantidad, precio_unitario) VALUES ("
            + to_string(idVenta) + ", " + to_string(item.idProducto) + ", " + to_string(item.cantidad) + ", " + to_string(item.precioUnitario) + ");";

        string qStock = "UPDATE productos SET existencia = existencia - " + to_string(item.cantidad) + " WHERE id_producto = " + to_string(item.idProducto);

        if (mysql_query(cn.getConector(), qDetalle.c_str()) != 0 || mysql_query(cn.getConector(), qStock.c_str()) != 0) {
            cout << "[!] Error en detalle/stock. Revirtiendo..." << endl;
            mysql_query(cn.getConector(), "ROLLBACK");
            return;
        }
    }

    mysql_query(cn.getConector(), "COMMIT");

    cout << "\n\n======================================================" << endl;
    cout << "                   SUPERMERCADO XYZ                   " << endl;
    cout << "                   Factura de venta                   " << endl;
    cout << "Factura No: " << setfill('0') << setw(6) << noFactura << " Serie: " << serie << setfill(' ') << endl;
    cout << "Fecha: " << fechaFactura << endl;
    cout << "NIT: " << nit << endl;
    cout << "Cliente: " << nombreCliente << endl;
    cout << "Direccion: Ciudad" << endl;
    cout << "------------------------------------------------------" << endl;
    cout << left << setw(6) << "COD" << setw(26) << "PRODUCTO" << setw(6) << "CANT" << setw(10) << "PRECIO" << "SUBTOTAL" << endl;

    for (const auto& item : carrito) {
        cout << left << setw(6) << item.idProducto
            << setw(26) << item.nombre.substr(0, 25)
            << setw(6) << item.cantidad
            << "Q " << left << setw(8) << fixed << setprecision(2) << item.precioUnitario
            << "Q " << item.subtotal << endl;
    }
    cout << "------------------------------------------------------" << endl;
    cout << right << setw(40) << "TOTAL:" << " Q " << fixed << setprecision(2) << totalVenta << endl;
    cout << "                Gracias por su compra                 " << endl;
    cout << "======================================================\n" << endl;
}

// ===================================================
// 6. FLUJO DE CONTROL Y CRUD GENERALES
// ===================================================
void menuOperaciones(string entidad, ConexionBD& cn) {
    int subOpcion = 0;
    do {
        cout << "\n--- Gestion de " << entidad << " ---" << endl;
        cout << "1. Crear (Insertar)" << endl;
        cout << "2. Leer (Consultar)" << endl;
        if (entidad != "Compras") {
            cout << "3. Actualizar" << endl;
            cout << "4. Eliminar" << endl;
        }
        cout << "5. Volver al menu principal" << endl;

        subOpcion = leerEntero("Seleccione una operacion: ");

        switch (subOpcion) {
        case 1: ejecutarInsercion(entidad, cn); break;
        case 2: ejecutarLectura(entidad, cn); break;
        case 3:
            if (entidad != "Compras") ejecutarActualizacion(entidad, cn);
            else cout << "[!] Deshabilitado para documentos contables." << endl;
            break;
        case 4:
            if (entidad != "Compras") ejecutarEliminacion(entidad, cn);
            else cout << "[!] Deshabilitado para documentos contables." << endl;
            break;
        case 5: cout << "Regresando..." << endl; break;
        default: cout << "[!] Opcion no valida." << endl;
        }
    } while (subOpcion != 5);
}

void ejecutarInsercion(string entidad, ConexionBD& cn) {
    if (entidad == "Marcas") insertarMarca(cn);
    else if (entidad == "Productos") insertarProducto(cn);
    else if (entidad == "Proveedores") insertarProveedor(cn);
    else if (entidad == "Clientes") insertarCliente(cn);
    else if (entidad == "Puestos") insertarPuesto(cn);
    else if (entidad == "Empleados") insertarEmpleado(cn);
    else if (entidad == "Compras") insertarCompra(cn);
}

void ejecutarLectura(string entidad, ConexionBD& cn) {
    if (entidad == "Marcas") mostrarTabla("SELECT id_marca, marca FROM marcas", 2, cn);
    else if (entidad == "Puestos") mostrarTabla("SELECT id_puesto, puesto FROM puestos", 2, cn);
    else if (entidad == "Proveedores") mostrarTabla("SELECT id_proveedor, proveedor, nit, telefono FROM proveedores", 4, cn);
    else if (entidad == "Clientes") mostrarTabla("SELECT id_cliente, nit, nombres, apellidos, telefono FROM clientes", 5, cn);
    else if (entidad == "Productos") mostrarTabla("SELECT p.id_producto, p.producto, m.marca, p.precio_venta, p.existencia FROM productos p INNER JOIN marcas m ON p.id_marca = m.id_marca", 5, cn);
    else if (entidad == "Empleados") mostrarTabla("SELECT e.id_empleado, e.nombres, e.apellidos, p.puesto, e.cui FROM empleados e INNER JOIN puestos p ON e.id_puesto = p.id_puesto", 5, cn);
    else if (entidad == "Compras") mostrarTabla("SELECT c.id_compra, c.no_orden_compra, p.proveedor, c.fecha_orden FROM compras c INNER JOIN proveedores p ON c.id_proveedor = p.id_proveedor", 4, cn);
}

// --------------------- CRUDS BÁSICOS ---------------------
void insertarMarca(ConexionBD& cn) {
    string marca = sanitizarString(cn, leerCadena("Nombre de marca (Max 50): ", 50, ValidacionTipo::SOLO_LETRAS));
    string query = "INSERT INTO marcas (marca) VALUES ('" + marca + "');";
    if (mysql_query(cn.getConector(), query.c_str()) == 0) cout << "[OK] Guardado." << endl;
    else cout << "[!] Error: " << mysql_error(cn.getConector()) << endl;
}

void insertarPuesto(ConexionBD& cn) {
    string puesto = sanitizarString(cn, leerCadena("Nombre de puesto (Max 50): ", 50, ValidacionTipo::SOLO_LETRAS));
    string query = "INSERT INTO puestos (puesto) VALUES ('" + puesto + "');";
    if (mysql_query(cn.getConector(), query.c_str()) == 0) cout << "[OK] Guardado." << endl;
    else cout << "[!] Error: " << mysql_error(cn.getConector()) << endl;
}

void insertarProveedor(ConexionBD& cn) {
    string prov = sanitizarString(cn, leerCadena("Proveedor (Max 60): ", 60, ValidacionTipo::SOLO_LETRAS));
    string nit = sanitizarString(cn, leerCadena("NIT (Max 12): ", 12, ValidacionTipo::NIT_FORMAT));
    string dir = sanitizarString(cn, leerCadena("Direccion (Max 80): ", 80, ValidacionTipo::DIRECCION));
    string tel = sanitizarString(cn, leerCadena("Telefono (Max 25): ", 25, ValidacionTipo::SOLO_DIGITOS));

    string query = "INSERT INTO proveedores (proveedor, nit, direccion, telefono) VALUES ('"
        + prov + "', '" + nit + "', '" + dir + "', '" + tel + "');";

    if (mysql_query(cn.getConector(), query.c_str()) == 0) cout << "[OK] Proveedor registrado." << endl;
    else cout << "[!] Error: " << mysql_error(cn.getConector()) << endl;
}

void insertarCliente(ConexionBD& cn) {
    string nom = sanitizarString(cn, leerCadena("Nombres: ", 60, ValidacionTipo::SOLO_LETRAS));
    string ape = sanitizarString(cn, leerCadena("Apellidos: ", 60, ValidacionTipo::SOLO_LETRAS));
    string nit = sanitizarString(cn, leerCadena("NIT: ", 12, ValidacionTipo::NIT_FORMAT));
    int gen = leerEntero("Genero (1=M, 0=F): ");
    string tel = sanitizarString(cn, leerCadena("Telefono: ", 25, ValidacionTipo::SOLO_DIGITOS));
    string correo = sanitizarString(cn, leerCadena("Correo: ", 45));

    string query = "INSERT INTO clientes (nombres, apellidos, nit, genero, telefono, correo_electronico, fecha_ingreso) VALUES ('"
        + nom + "', '" + ape + "', '" + nit + "', " + to_string(gen) + ", '" + tel + "', '" + correo + "', NOW());";

    if (mysql_query(cn.getConector(), query.c_str()) == 0) cout << "[OK] Cliente registrado." << endl;
    else cout << "[!] Error: " << mysql_error(cn.getConector()) << endl;
}

void insertarProducto(ConexionBD& cn) {
    string prod = sanitizarString(cn, leerCadena("Producto: ", 50, ValidacionTipo::ALFANUMERICO_ESPACIOS));
    int idMarca = leerEntero("ID Marca: ");
    string desc = sanitizarString(cn, leerCadena("Descripcion: ", 100, ValidacionTipo::ALFANUMERICO_ESPACIOS_PUNTUACION));
    string img = sanitizarString(cn, leerCadena("Nombre de Imagen (Max 30): ", 30, ValidacionTipo::NOMBRE_ARCHIVO));
    double pCosto = leerDouble("Precio Costo: ");
    double pVenta = leerDouble("Precio Venta: ");
    int existencia = leerEntero("Existencia: ");

    string query = "INSERT INTO productos (producto, id_marca, descripcion, imagen, precio_costo, precio_venta, existencia, fecha_ingreso) VALUES ('"
        + prod + "', " + to_string(idMarca) + ", '" + desc + "', '" + img + "', " + to_string(pCosto) + ", " + to_string(pVenta) + ", " + to_string(existencia) + ", NOW());";

    if (mysql_query(cn.getConector(), query.c_str()) == 0) cout << "[OK] Producto registrado." << endl;
    else cout << "[!] Error: " << mysql_error(cn.getConector()) << endl;
}

void insertarEmpleado(ConexionBD& cn) {
    string nom = sanitizarString(cn, leerCadena("Nombres: ", 60, ValidacionTipo::SOLO_LETRAS));
    string ape = sanitizarString(cn, leerCadena("Apellidos: ", 60, ValidacionTipo::SOLO_LETRAS));
    string dir = sanitizarString(cn, leerCadena("Direccion: ", 80, ValidacionTipo::DIRECCION));
    string tel = sanitizarString(cn, leerCadena("Telefono: ", 25, ValidacionTipo::SOLO_DIGITOS));
    string cui = sanitizarString(cn, leerCadena("CUI: ", 15, ValidacionTipo::SOLO_DIGITOS));
    int gen = leerEntero("Genero (1=M, 0=F): ");
    string fNac = sanitizarString(cn, leerCadena("Fecha Nacimiento (YYYY-MM-DD): ", 10, ValidacionTipo::FECHA));
    int idPuesto = leerEntero("ID Puesto: ");
    string fInicio = sanitizarString(cn, leerCadena("Fecha Inicio Labores (YYYY-MM-DD): ", 10, ValidacionTipo::FECHA));

    string query = "INSERT INTO empleados (nombres, apellidos, direccion, telefono, cui, genero, fecha_nacimiento, id_puesto, fecha_inicio_labores, fecha_ingreso) VALUES ('"
        + nom + "', '" + ape + "', '" + dir + "', '" + tel + "', '" + cui + "', " + to_string(gen) + ", '" + fNac + "', " + to_string(idPuesto) + ", '" + fInicio + "', NOW());";

    if (mysql_query(cn.getConector(), query.c_str()) == 0) cout << "[OK] Empleado registrado." << endl;
    else cout << "[!] Error: " << mysql_error(cn.getConector()) << endl;
}

void insertarCompra(ConexionBD& cn) {
    cout << "\n=== NUEVA ORDEN DE COMPRA ===" << endl;
    int noOrden = leerEntero("Numero de Orden: ");
    int idProveedor = leerEntero("ID Proveedor: ");
    string fecha = obtenerFechaActual();

    mysql_query(cn.getConector(), "START TRANSACTION");

    string qMaestro = "INSERT INTO compras (no_orden_compra, id_proveedor, fecha_orden, fecha_ingreso) VALUES (" +
        to_string(noOrden) + ", " + to_string(idProveedor) + ", '" + fecha + "', NOW());";

    if (mysql_query(cn.getConector(), qMaestro.c_str()) != 0) {
        cout << "[!] Error maestro: " << mysql_error(cn.getConector()) << endl;
        mysql_query(cn.getConector(), "ROLLBACK"); return;
    }

    int idCompra = 0;
    mysql_query(cn.getConector(), "SELECT LAST_INSERT_ID();");
    MYSQL_RES* res = mysql_store_result(cn.getConector());
    MYSQL_ROW row = mysql_fetch_row(res);
    idCompra = atoi(row[0]);
    mysql_free_result(res);

    int numProductos = leerEntero("Cantidad de productos a ingresar: ");
    for (int i = 0; i < numProductos; i++) {
        int idProducto = leerEntero("ID Producto: ");
        int cantidad = leerEntero("Cantidad: ");
        double precioCosto = leerDouble("Costo Unitario: ");

        string qDetalle = "INSERT INTO compras_detalle (id_compra, id_producto, cantidad, precio_costo_unitario) VALUES (" +
            to_string(idCompra) + ", " + to_string(idProducto) + ", " + to_string(cantidad) + ", " + to_string(precioCosto) + ");";

        string qUpdateStock = "UPDATE productos SET existencia = existencia + " + to_string(cantidad) + " WHERE id_producto = " + to_string(idProducto);

        if (mysql_query(cn.getConector(), qDetalle.c_str()) != 0 || mysql_query(cn.getConector(), qUpdateStock.c_str()) != 0) {
            cout << "[!] Error. Revirtiendo..." << endl;
            mysql_query(cn.getConector(), "ROLLBACK"); return;
        }
    }
    mysql_query(cn.getConector(), "COMMIT");
    cout << "\n[OK] Orden procesada con exito." << endl;
}

// --------------------- ACT / ELI / READ ---------------------
void ejecutarActualizacion(string entidad, ConexionBD& cn) {
    int id = leerEntero("Ingrese ID a modificar: ");
    string tabla = "", campoId = "", campoUpd = "";
    ValidacionTipo tipo = ValidacionTipo::GENERICO;
    if (entidad == "Marcas") {
        tabla = "marcas"; campoId = "id_marca"; campoUpd = "marca";
        tipo = ValidacionTipo::SOLO_LETRAS;
    }
    else if (entidad == "Puestos") {
        tabla = "puestos"; campoId = "id_puesto"; campoUpd = "puesto";
        tipo = ValidacionTipo::SOLO_LETRAS;
    }
    else {
        cout << "Soporte rapido solo para catalogos pequenos.\n"; return;
    }

    string nuevoVal = sanitizarString(cn, leerCadena("Nuevo valor: ", 50, tipo));
    string q = "UPDATE " + tabla + " SET " + campoUpd + " = '" + nuevoVal + "' WHERE " + campoId + " = " + to_string(id);
    if (mysql_query(cn.getConector(), q.c_str()) == 0) cout << "[OK] Actualizado." << endl;
    else cout << "[!] Error: " << mysql_error(cn.getConector()) << endl;
}

void ejecutarEliminacion(string entidad, ConexionBD& cn) {
    int id = leerEntero("Ingrese ID a eliminar: ");
    string tabla = "", campoId = "";
    if (entidad == "Marcas") { tabla = "marcas"; campoId = "id_marca"; }
    else if (entidad == "Productos") { tabla = "productos"; campoId = "id_producto"; }
    else if (entidad == "Proveedores") { tabla = "proveedores"; campoId = "id_proveedor"; }
    else if (entidad == "Clientes") { tabla = "clientes"; campoId = "id_cliente"; }
    else if (entidad == "Puestos") { tabla = "puestos"; campoId = "id_puesto"; }
    else if (entidad == "Empleados") { tabla = "empleados"; campoId = "id_empleado"; }

    string q = "DELETE FROM " + tabla + " WHERE " + campoId + " = " + to_string(id);
    if (mysql_query(cn.getConector(), q.c_str()) == 0) cout << "[OK] Eliminado." << endl;
    else cout << "[!] Error (Revisa llaves foraneas): " << mysql_error(cn.getConector()) << endl;
}

void mostrarTabla(string query, int numColumnas, ConexionBD& cn) {
    if (mysql_query(cn.getConector(), query.c_str()) == 0) {
        MYSQL_RES* res = mysql_store_result(cn.getConector());
        if (res) {
            cout << "\n--- RESULTADOS ---" << endl;
            MYSQL_ROW row;
            while ((row = mysql_fetch_row(res))) {
                for (int i = 0; i < numColumnas; i++) {
                    cout << (row[i] ? row[i] : "NULL") << (i < numColumnas - 1 ? " | " : "");
                }
                cout << endl;
            }
            mysql_free_result(res);
        }
    }
    else {
        cout << "[!] Error: " << mysql_error(cn.getConector()) << endl;
    }
}