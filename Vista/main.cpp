#define NOMINMAX 
#include <iostream>
#include <limits>
#include <string>
#include <mysql.h> // Biblioteca nativa de MySQL
#include "ConexionBD.h" 

using namespace std;

// ===================================================
// 1. PROTOTIPOS DE FUNCIONES
// ===================================================
void menuOperaciones(string entidad, ConexionBD& cn);
void ejecutarInsercion(string entidad, ConexionBD& cn);
void ejecutarLectura(string entidad, ConexionBD& cn);
void ejecutarActualizacion(string entidad, ConexionBD& cn);
void ejecutarEliminacion(string entidad, ConexionBD& cn);

void insertarMarca(ConexionBD& cn);
void insertarProducto(ConexionBD& cn);
void insertarProveedor(ConexionBD& cn);
void insertarCliente(ConexionBD& cn);
void insertarPuesto(ConexionBD& cn);
void insertarEmpleado(ConexionBD& cn);

void mostrarTabla(string query, int numColumnas, ConexionBD& cn);

// ===================================================
// 2. FUNCIÓN PRINCIPAL
// ===================================================
int main() {
    setlocale(LC_ALL, "spanish");

    ConexionBD cn = ConexionBD();
    cn.abrir_conexion();

    // Validar que la conexión no sea nula antes de continuar
    if (cn.getConector() == nullptr) {
        cout << "[!] El sistema no puede iniciar sin conexion a la base de datos." << endl;
        return 1;
    }

    int opcion = 0;
    do {
        cout << "\n=============================================" << endl;
        cout << "       SISTEMA DE VENTAS - MENU PRINCIPAL    " << endl;
        cout << "=============================================" << endl;
        cout << "1. Gestionar Marcas" << endl;
        cout << "2. Gestionar Productos" << endl;
        cout << "3. Gestionar Proveedores" << endl;
        cout << "4. Gestionar Clientes" << endl;
        cout << "5. Gestionar Puestos" << endl;
        cout << "6. Gestionar Empleados" << endl;
        cout << "7. Salir" << endl;
        cout << "Seleccione una opcion: ";

        if (!(cin >> opcion)) {
            cout << "\n[!] Error: Por favor, ingrese un numero valido." << endl;
            cin.clear();
            cin.ignore(10000, '\n');
            continue;
        }

        switch (opcion) {
        case 1: menuOperaciones("Marcas", cn); break;
        case 2: menuOperaciones("Productos", cn); break;
        case 3: menuOperaciones("Proveedores", cn); break;
        case 4: menuOperaciones("Clientes", cn); break;
        case 5: menuOperaciones("Puestos", cn); break;
        case 6: menuOperaciones("Empleados", cn); break;
        case 7:
            cout << "\nCerrando conexion y saliendo del sistema..." << endl;
            break;
        default:
            cout << "[!] Opcion no valida." << endl;
        }

    } while (opcion != 7);

    cn.cerrar_conexion();
    return 0;
}

// ===================================================
// 3. DESARROLLO DE FUNCIONES DE CONTROL DE FLUJO
// ===================================================
void menuOperaciones(string entidad, ConexionBD& cn) {
    int subOpcion = 0;
    do {
        cout << "\n--- Gestion de " << entidad << " ---" << endl;
        cout << "1. Crear (Insertar)" << endl;
        cout << "2. Leer (Consultar)" << endl;
        cout << "3. Actualizar" << endl;
        cout << "4. Eliminar" << endl;
        cout << "5. Volver al menu principal" << endl;
        cout << "Seleccione una operacion: ";

        if (!(cin >> subOpcion)) {
            cout << "\n[!] Error: Entrada no valida." << endl;
            cin.clear();
            cin.ignore(10000, '\n');
            continue;
        }

        switch (subOpcion) {
        case 1: ejecutarInsercion(entidad, cn); break;
        case 2: ejecutarLectura(entidad, cn); break;
        case 3: ejecutarActualizacion(entidad, cn); break;
        case 4: ejecutarEliminacion(entidad, cn); break;
        case 5: cout << "Regresando..." << endl; break;
        default: cout << "[!] Opcion no disponible." << endl;
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
}

void ejecutarLectura(string entidad, ConexionBD& cn) {
    if (entidad == "Marcas") mostrarTabla("SELECT id_marca, marca FROM marcas", 2, cn);
    else if (entidad == "Puestos") mostrarTabla("SELECT id_puesto, puesto FROM puestos", 2, cn);
    else if (entidad == "Proveedores") mostrarTabla("SELECT id_proveedor, proveedor, nit, telefono FROM proveedores", 4, cn);
    else if (entidad == "Clientes") mostrarTabla("SELECT id_cliente, nit, nombres, apellidos, telefono FROM clientes", 5, cn);
    else if (entidad == "Productos") mostrarTabla("SELECT p.id_producto, p.producto, m.marca, p.precio_venta, p.existencia FROM productos p INNER JOIN marcas m ON p.id_marca = m.id_marca", 5, cn);
    else if (entidad == "Empleados") mostrarTabla("SELECT e.id_empleado, e.nombres, e.apellidos, p.puesto, e.telefono FROM empleados e INNER JOIN puestos p ON e.id_puesto = p.id_puesto", 5, cn);
}

void ejecutarActualizacion(string entidad, ConexionBD& cn) {
    int id = 0;
    string tabla = "", campoId = "", campoUpdate = "", nuevoValor = "";

    cout << "\nIngrese el ID del registro que desea actualizar de " << entidad << ": ";
    cin >> id; cin.ignore();

    if (entidad == "Marcas") { tabla = "marcas"; campoId = "id_marca"; campoUpdate = "marca"; }
    else if (entidad == "Puestos") { tabla = "puestos"; campoId = "id_puesto"; campoUpdate = "puesto"; }
    else {
        cout << "[!] Soporte de modificacion rapida en consola para Marcas y Puestos." << endl;
        return;
    }

    cout << "Ingrese el nuevo valor para " << campoUpdate << ": ";
    getline(cin, nuevoValor);

    string query = "UPDATE " + tabla + " SET " + campoUpdate + " = '" + nuevoValor + "' WHERE " + campoId + " = " + to_string(id) + ";";

    if (mysql_query(cn.getConector(), query.c_str()) == 0) {
        cout << "\n[OK] Registro actualizado correctamente." << endl;
    }
    else {
        cout << "\n[!] Error al actualizar: " << mysql_error(cn.getConector()) << endl;
    }
}

void ejecutarEliminacion(string entidad, ConexionBD& cn) {
    int id = 0;
    string tabla = "", campoId = "";
    cout << "\nIngrese el ID del registro a ELIMINAR de " << entidad << ": ";
    cin >> id;

    if (entidad == "Marcas") { tabla = "marcas"; campoId = "id_marca"; }
    else if (entidad == "Productos") { tabla = "productos"; campoId = "id_producto"; }
    else if (entidad == "Proveedores") { tabla = "proveedores"; campoId = "id_proveedor"; }
    else if (entidad == "Clientes") { tabla = "clientes"; campoId = "id_cliente"; }
    else if (entidad == "Puestos") { tabla = "puestos"; campoId = "id_puesto"; }
    else if (entidad == "Empleados") { tabla = "empleados"; campoId = "id_empleado"; }

    string query = "DELETE FROM " + tabla + " WHERE " + campoId + " = " + to_string(id) + ";";

    if (mysql_query(cn.getConector(), query.c_str()) == 0) {
        cout << "\n[OK] Registro eliminado con exito." << endl;
    }
    else {
        cout << "\n[!] Error al eliminar (Verifique llaves foraneas o integridad): " << mysql_error(cn.getConector()) << endl;
    }
}

// ===================================================
// 4. DESARROLLO DE FUNCIONES CRUD NATIVAS
// ===================================================
void insertarMarca(ConexionBD& cn) {
    string nombreMarca;
    cout << "\n=== NUEVA MARCA ===" << endl;
    cin.ignore();
    cout << "Ingrese el nombre de la marca: "; getline(cin, nombreMarca);

    string query = "INSERT INTO marcas (marca) VALUES ('" + nombreMarca + "');";

    if (mysql_query(cn.getConector(), query.c_str()) == 0) {
        cout << "\n[OK] Marca guardada exitosamente." << endl;
    }
    else {
        cout << "\n[!] Error al guardar: " << mysql_error(cn.getConector()) << endl;
    }
}

void insertarPuesto(ConexionBD& cn) {
    string nombrePuesto;
    cout << "\n=== NUEVO PUESTO ===" << endl;
    cin.ignore();
    cout << "Nombre del puesto: "; getline(cin, nombrePuesto);

    string query = "INSERT INTO puestos (puesto) VALUES ('" + nombrePuesto + "');";

    if (mysql_query(cn.getConector(), query.c_str()) == 0) {
        cout << "\n[OK] Puesto guardado exitosamente." << endl;
    }
    else {
        cout << "\n[!] Error al guardar: " << mysql_error(cn.getConector()) << endl;
    }
}

void insertarProveedor(ConexionBD& cn) {
    string prov, nit, dir, tel;
    cout << "\n=== NUEVO PROVEEDOR ===" << endl;
    cin.ignore();
    cout << "Nombre Proveedor: "; getline(cin, prov);
    cout << "NIT: "; getline(cin, nit);
    cout << "Direccion: "; getline(cin, dir);
    cout << "Telefono: "; getline(cin, tel);

    string query = "INSERT INTO proveedores (proveedor, nit, direccion, telefono) VALUES ('"
        + prov + "', '" + nit + "', '" + dir + "', '" + tel + "');";

    if (mysql_query(cn.getConector(), query.c_str()) == 0) {
        cout << "\n[OK] Proveedor registrado con exito." << endl;
    }
    else {
        cout << "\n[!] Error al registrar: " << mysql_error(cn.getConector()) << endl;
    }
}

void insertarCliente(ConexionBD& cn) {
    string nom, ape, nit, tel, correo;
    cout << "\n=== NUEVO CLIENTE ===" << endl;
    cin.ignore();
    cout << "Nombres: "; getline(cin, nom);
    cout << "Apellidos: "; getline(cin, ape);
    cout << "NIT: "; getline(cin, nit);
    cout << "Telefono: "; getline(cin, tel);
    cout << "Correo Electronico: "; getline(cin, correo);

    string query = "INSERT INTO clientes (nombres, apellidos, nit, telefono, correo_electronico, fecha_ingreso) VALUES ('"
        + nom + "', '" + ape + "', '" + nit + "', '" + tel + "', '" + correo + "', NOW());";

    if (mysql_query(cn.getConector(), query.c_str()) == 0) {
        cout << "\n[OK] Cliente registrado correctamente." << endl;
    }
    else {
        cout << "\n[!] Error al registrar cliente: " << mysql_error(cn.getConector()) << endl;
    }
}

void insertarProducto(ConexionBD& cn) {
    string prod, desc;
    int idMarca = 0, existencia = 0;
    double pCosto = 0.0, pVenta = 0.0;

    cout << "\n=== NUEVO PRODUCTO ===" << endl;
    cin.ignore();
    cout << "Nombre Producto: "; getline(cin, prod);
    cout << "ID Marca asociada: "; cin >> idMarca; cin.ignore();
    cout << "Descripcion: "; getline(cin, desc);
    cout << "Precio Costo: "; cin >> pCosto;
    cout << "Precio Venta: "; cin >> pVenta;
    cout << "Existencia Inicial: "; cin >> existencia;

    string query = "INSERT INTO productos (producto, id_marca, descripcion, precio_costo, precio_venta, existencia, fecha_ingreso) VALUES ('"
        + prod + "', " + to_string(idMarca) + ", '" + desc + "', " + to_string(pCosto) + ", " + to_string(pVenta) + ", " + to_string(existencia) + ", NOW());";

    if (mysql_query(cn.getConector(), query.c_str()) == 0) {
        cout << "\n[OK] Producto registrado con exito." << endl;
    }
    else {
        cout << "\n[!] Error al registrar producto: " << mysql_error(cn.getConector()) << endl;
    }
}

void insertarEmpleado(ConexionBD& cn) {
    string nom, ape, dir, tel, cui, fNac, fInicio;
    int idPuesto = 0;

    cout << "\n=== NUEVO EMPLEADO ===" << endl;
    cin.ignore();
    cout << "Nombres: "; getline(cin, nom);
    cout << "Apellidos: "; getline(cin, ape);
    cout << "Direccion: "; getline(cin, dir);
    cout << "Telefono: "; getline(cin, tel);
    cout << "CUI: "; getline(cin, cui);
    cout << "Fecha Nacimiento (YYYY-MM-DD): "; getline(cin, fNac);
    cout << "ID del Puesto laboral: "; cin >> idPuesto; cin.ignore();
    cout << "Fecha Inicio Labores (YYYY-MM-DD): "; getline(cin, fInicio);

    string query = "INSERT INTO empleados (nombres, apellidos, direccion, telefono, cui, fecha_nacimiento, id_puesto, fecha_inicio_labores, fecha_ingreso) VALUES ('"
        + nom + "', '" + ape + "', '" + dir + "', '" + tel + "', '" + cui + "', '" + fNac + "', " + to_string(idPuesto) + ", '" + fInicio + "', NOW());";

    if (mysql_query(cn.getConector(), query.c_str()) == 0) {
        cout << "\n[OK] Empleado registrado correctamente." << endl;
    }
    else {
        cout << "\n[!] Error al registrar empleado: " << mysql_error(cn.getConector()) << endl;
    }
}

// ===================================================
// 5. FUNCIÓN SELECT REESCRIBIBLE PARA MYSQL NATIVO
// ===================================================
void mostrarTabla(string query, int numColumnas, ConexionBD& cn) {
    if (mysql_query(cn.getConector(), query.c_str()) == 0) {
        MYSQL_RES* resultado = mysql_store_result(cn.getConector());

        if (resultado) {
            cout << "\n--- REGISTROS ENCONTRADOS ---" << endl;
            MYSQL_ROW fila;

            // Recorrer las filas obtenidas desde la base de datos
            while ((fila = mysql_fetch_row(resultado))) {
                for (int i = 0; i < numColumnas; i++) {
                    if (fila[i] == NULL) cout << "NULL";
                    else cout << fila[i];

                    if (i < numColumnas - 1) cout << " | ";
                }
                cout << endl;
            }
            mysql_free_result(resultado); // Liberar memoria del resultado
        }
    }
    else {
        cout << "[!] Error al consultar datos: " << mysql_error(cn.getConector()) << endl;
    }
}