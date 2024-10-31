#include <iostream>
#include <string>
#include <queue>
#include <stack>
#include <stdexcept>
#include <fstream>
#include <vector>

using namespace std;

struct Reader {
    string name;
    string dni;
    string bookTitle;

    Reader(string n, string d, string bt) : name(n), dni(d), bookTitle(bt) {}
};

class Book {
public:
    string title;
    string autor;
    int anho;
    string publisher;
    string isbn;
    int pages;
    bool disponible;
    Book* siguiente;

    Book(string title, string _autor, int _anho, 
          string _publisher, string _isbn, int _pages) 
        : title(title), autor(_autor), anho(_anho), 
          publisher(_publisher), isbn(_isbn), pages(_pages), 
          disponible(true), siguiente(nullptr) {}
};

class ListaLibros {
private:
    Book* head;
    queue<Reader> wait_queue;
    stack<string> history;

public:
    ListaLibros() : head(nullptr) {}

    void agregar_book(string titulo, string autor, int anho, 
                      string publisher, string isbn, int pages) {
        Book* newBook = new Book(titulo, autor, anho, publisher, isbn, pages);
        
        if (!head) {
            head = newBook;
        } else {
            Book* temp = head;
            while (temp->siguiente != nullptr) {
                temp = temp->siguiente;
            }
            temp->siguiente = newBook;
        }

        history.push("*ੈ✩ Agregar libro: " + titulo);
        cout << "•ᴗ• Libro agregado: " << titulo << endl;
    }

     void cargarLibrosDesdeArchivo(const string& biblioteca) {
        ifstream archivo(biblioteca);
        if (!archivo.is_open()) {
            cout << "Error al abrir el archivo: " << biblioteca << " (_ _ ) . . z Z " << endl;
            return;
        }

        string titulo, autor, publisher, isbn;
        int anho, pages;

        while (getline(archivo, titulo) &&
               getline(archivo, autor) &&
               archivo >> anho &&
               archivo.ignore() && 
               getline(archivo, publisher) &&
               getline(archivo, isbn) &&
               archivo >> pages &&
               archivo.ignore()) {
            agregar_book(titulo, autor, anho, publisher, isbn, pages);
        }
        archivo.close();
    }
    void guardarLibrosEnArchivo(const string& biblioteca) {
        ofstream archivo(biblioteca);
        if (!archivo.is_open()) {
            cout << "Error al abrir el archivo para guardar: " << biblioteca << " (_ _ ) . . z Z " << endl;
            return;
        }
    Book* temp = head;
        while (temp != nullptr) {
            archivo << temp->title << endl;
            archivo << temp->autor << endl;
            archivo << temp->anho << endl;
            archivo << temp->publisher << endl;
            archivo << temp->isbn << endl;
            archivo << temp->pages << endl;
            temp = temp->siguiente;
        }
        archivo.close();
        cout << "Biblioteca guardada en archivo. ( ˊᵕˋ )♡.°⑅" << endl;
    }

    void deshacerUltimaOperacion() {
        if (history.empty()) {
            cout << "(๑-﹏-๑) No hay operaciones para deshacer." << endl;
            return;
        }

        string ultimaOperacion = history.top();
        history.pop();
        cout << "‧₊˚✧ Deshaciendo operación: " << ultimaOperacion << endl;

        if (ultimaOperacion.find("٩(ˊᗜˋ*)و Agregar libro: ") != string::npos) {
            string titulo = ultimaOperacion.substr(15);
            eliminarLibro(titulo);
        } else if (ultimaOperacion.find("Eliminar libro: ") != string::npos) {
            string titulo = ultimaOperacion.substr(16);
            cout << "¿Eliminaste sin querer? Bueno, agregalo manualmente. :P" << endl;
        }
    }

    void eliminarLibro(const string& title) {
        if (!head) return;

        if (head->title == title) {
            Book* temp = head;
            head = head->siguiente;
            delete temp;
            history.push(" ̊ ̟ ꒷꒦ Eliminar libro: " + title);
            return;
        }

        Book* temp = head;
        while (temp->siguiente != nullptr) {
            if (temp->siguiente->title == title) {
                Book* libroAEliminar = temp->siguiente;
                temp->siguiente = temp->siguiente->siguiente;
                delete libroAEliminar;
                history.push("˚. ✦. Eliminar libro: " + title);
                return;
            }
            temp = temp->siguiente;
        }

        cout << "No se encontró el libro para eliminar. (¬_¬)" << endl;
    }

    void solicitarLibro(const string& nombre, const string& dni, const string& title) {
        try {
            Book* libroSolicitado = buscarPorTitulo(title);
            if (!libroSolicitado) {
                throw runtime_error("Error: No se encontró un libro con el titulo '" + title + "'. ( •̀ - •́ )");
            }

            if (libroSolicitado->disponible) {
                libroSolicitado->disponible = false;
                cout << "El libro '" << title << "' ha sido asignado a " << nombre << " (DNI: " << dni << ")." << endl;
                history.push("Solicitar libro: " + title + " por " + nombre);
            } else {
                cout << "El libro '" << title << "' no está disponible actualmente. " << nombre << " ha sido añadido a la cola de espera." << endl;
                wait_queue.push(Reader(nombre, dni, title));
            }
        } catch (const runtime_error& e) {
            cout << e.what() << endl;
        }
    }

    void devolverLibro(const string& title) {
        try {
            Book* returned_book = buscarPorTitulo(title);
            if (!returned_book) {
                throw runtime_error("Error: No se encontró un libro con el titulo '" + title + "' para devolver.(·•᷄0 •᷅ )");
            }

            returned_book->disponible = true;
            cout << "El libro '" << title << "' ha sido devuelto y está disponible nuevamente." << endl;

            if (!wait_queue.empty()) {
                Reader lectorEnEspera = wait_queue.front();
                if (lectorEnEspera.bookTitle == title) {
                    wait_queue.pop();
                    returned_book->disponible = false;
                    cout << "El libro '" << title << "' ha sido asignado a " << lectorEnEspera.name << " (DNI: " << lectorEnEspera.dni << ") de la cola de espera." << endl;
                }
            }

            history.push("Devolver libro: " + title);
        } catch (const runtime_error& e) {
            cout << e.what() << endl;
        }
    }

    Book* buscarPorTitulo(const string& title) {
        Book* temp = head;
        while (temp != nullptr) {
            if (temp->title == title) return temp;
            temp = temp->siguiente;
        }
        return nullptr;
    }

    void search_book(const string& title) {
        vector<string> posible_titles;
        Book* temp = head;
        
        while (temp != nullptr) {
            if (temp->title.find(title) != string::npos) {
                posible_titles.push_back(temp->title);
            }
            temp = temp->siguiente;
        }

        if (!posible_titles.empty()) {
            cout << " ༘ ⋆｡˚ Esto es lo que nuestro servicio de búsqueda ha proporcionado:\n";
            for (const string& posible_title : posible_titles) {
                cout << "- " << posible_title << endl;
            }
        } else {
            cout << "No se encontró el libro con el titulo: " << title << "(´｡• ◡ •｡`) ♡" << endl;
        }
    }

    void mostrarListaLibros() {
        if (!head) {
            cout << "No hay libros en la biblioteca. (T _ T)" << endl;
            return;
        }

        cout << "⁺˚⋆｡°✩₊ Lista completa de libros:\n";
        Book* temp = head;
        while (temp != nullptr) {
            cout << "- " << temp->title << " (Autor: " << temp->autor << ", Año: " << temp->anho 
                 << ", publisher: " << temp->publisher << ", ISBN: " << temp->isbn 
                 << ", Páginas: " << temp->pages 
                 << ", Disponible: " << (temp->disponible ? "Sí" : "No") << ")\n";
            temp = temp->siguiente;
        }
    }

    ~ListaLibros() {
        while (head != nullptr) {
            Book* temp = head;
            head = head->siguiente;
            delete temp;
        }
    }
    void ordenarLibros() {
    if (!head || !head->siguiente) {
        return;
    }

    bool intercambio;
    do {
        intercambio = false;
        Book* temp = head;
        while (temp->siguiente != nullptr) {
            if (temp->title > temp->siguiente->title) {
                swap(temp->title, temp->siguiente->title);
                swap(temp->autor, temp->siguiente->autor);
                swap(temp->anho, temp->siguiente->anho);
                swap(temp->publisher, temp->siguiente->publisher);
                swap(temp->isbn, temp->siguiente->isbn);
                swap(temp->pages, temp->siguiente->pages);
                intercambio = true;
            }
            temp = temp->siguiente;
        }
    } while (intercambio);

    cout << "Libros ordenados por titulo. ٩(ˊᗜˋ*)و" << endl;
}
};

int main() {
    ListaLibros biblioteca;
    biblioteca.cargarLibrosDesdeArchivo("biblioteca.txt"); 

    while (true) {
        cout << " ";
        cout << " ⋆ ˚｡⋆୨ ♡ ୧⋆ ˚｡⋆ ";
        cout << "\nSeleccione una opcion:\n";
        cout << "1. Agregar libro\n";
        cout << "2. Buscar libro\n";
        cout << "3. Solicitar libro\n";
        cout << "4. Devolver libro\n";
        cout << "5. Mostrar lista completa de libros\n";
        cout << "6. Deshacer última operación\n";
        cout << "7. Ordenar libros\n";
        cout << "0. Salir\n";
        cout << " ";
        cout << " ⋆ ˚｡⋆୨ ♡ ୧⋆ ˚｡⋆ ";
        cout << " ";
        cout << "Opcion: ";
        

        int opcion;
        cin >> opcion;
        cin.ignore();

        string nombre, dni, title, autor, isbn, publisher;
        int anho, pages;

        switch (opcion) {
            case 1:
                cout << "૮₍ ˃ ⤙ ˂ ₎ა Ingrese los detalles del libro:\n";
                cout << "༘⋆♡ Título: ";
                getline(cin, title);
                cout << "˚₊· ͟͟͞͞➳❥ Autor: ";
                getline(cin, autor);
                cout << "°˖➴ Anho de edición (solamente numeros): ";
                cin >> anho;
                cin.ignore();
                cout << "༘⋆♡ Editorial: ";
                getline(cin, publisher);
                cout << "˚₊· ͟͟͞͞➳❥ ISBN: ";
                getline(cin, isbn);
                cout << "°˖➴ Número de páginas: ";
                cin >> pages;
                biblioteca.agregar_book(title, autor, anho, publisher, isbn, pages);
                break;

            case 2:
                cout << "₊✧ Ingrese el titulo del libro a buscar: ";
                getline(cin, title);
                biblioteca.search_book(title);
                break;

            case 3:
                cout << "⋆ ˚｡⋆ Ingrese su nombre: ";
                getline(cin, nombre);
                cout << "₎♡ ༘Ingrese su DNI (sin puntos y todo junto): ";
                getline(cin, dni);
                cout << "⋆ ˚｡⋆ Ingrese el título del libro a solicitar: ";
                getline(cin, title);
                biblioteca.solicitarLibro(nombre, dni, title);
                break;

            case 4:
                cout << "₊✧ Ingrese el título del libro a devolver: ";
                getline(cin, title);
                biblioteca.devolverLibro(title);
                break;

            case 5: 
                biblioteca.mostrarListaLibros();
                break;

            case 6: 
                biblioteca.deshacerUltimaOperacion();
                break;

            case 7: 
                biblioteca.ordenarLibros(); 
                break;
            case 0: 
                biblioteca.guardarLibrosEnArchivo("biblioteca.txt");
                return 0;

            default:
                cout << "(˚ ˃̣̣̥⌓˂̣̣̥ ) Opcion no valida. Intente nuevamente.\n";
        }
    }

    return 0;
}

// https://www.geeksforgeeks.org/cpp-linked-list/
// https://www.geeksforgeeks.org/bubble-sort-in-cpp/ 
// https://stackoverflow.com/questions/37848186/how-to-understand-head-next-next-head-for-reverse-single-list-by-recursion
// https://www.geeksforgeeks.org/queue-cpp-stl/