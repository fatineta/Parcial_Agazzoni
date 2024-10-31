from collections import deque

class BookNotFoundError(Exception):
    pass

class Book:
    def __init__(self, title, author, year, publisher, isbn, pages):
        self.title = title
        self.author = author
        self.year = year
        self.publisher = publisher
        self.isbn = isbn
        self.pages = pages

class Reader:
    def __init__(self, name, dni, book_title):
        self.name = name
        self.dni = dni
        self.book_title = book_title

class Biblioteca:
    def __init__(self):
        self.books = []
        self.atendidos = []    
        self.solicitudes = []   # Esto hace referencia a los lectores en espera en realidad :P
        self.history_stack = []

    def load_books_from_file(self, filename):
        try:
            with open(filename, "r") as file:
                lines = file.readlines()
                for i in range(0, len(lines), 6):
                    title = lines[i].strip()
                    author = lines[i + 1].strip()
                    year = int(lines[i + 2].strip())
                    publisher = lines[i + 3].strip()
                    isbn = lines[i + 4].strip()
                    pages = int(lines[i + 5].strip())
                    self.books.append(Book(title, author, year, publisher, isbn, pages))
        except FileNotFoundError:
            print(f"El archivo '{filename}' no se encontró. (｡•́︿•̀｡)")

    def agregar_libro(self, filename):
        option = input("₊˚⊹♡ ¿Desea agregar un libro? (Y/N): ")
        if option.lower() != 'y':
            print("No se agregarán libros. ദ്ദി(˵ •̀ ᴗ - ˵ ) ✧")
            return

        title = input("⋆౨ৎ˚⟡˖ ࣪Ingrese el título del libro: ")
        author = input("‧₊˚✩彡 Ingrese el autor del libro: ")
        year = int(input("⋆౨ৎ˚⟡ Ingrese el año de publicación: "))
        publisher = input("‧₊˚✩彡 Ingrese la editorial: ")

        while True:
            isbn = input("⋆౨ৎ˚⟡ Ingrese el ISBN: ")
            try:
                self.validate_isbn(isbn)
                break  
            except ValueError as e:
                print(e)  

        pages = int(input("‧₊˚✩彡 Ingrese el número de páginas: "))

        new_book = Book(title, author, year, publisher, isbn, pages)
        self.books.append(new_book)

        self.guardar_libros_en_archivo(filename)
        print("Libro agregado y guardado exitosamente. ⸜(｡˃ ᵕ ˂ )⸝")
        self.history_stack.append(("agregar", new_book))

    def display_books(self):
        self.ordenar_libros_bubble_sort()  
        if not self.books:
            print("૮(˶╥︿╥)ა . . . No hay libros en la biblioteca.")
        else:
            print("Libros disponibles:")
            for book in self.books:
                print(f"Título: {book.title}, Autor: {book.author}, Año: {book.year}, "
                      f"Editorial: {book.publisher}, ISBN: {book.isbn}, Páginas: {book.pages}")

    def buscar_libro(self, search_term):
        found_books = [book for book in self.books if (search_term.lower() in book.title.lower() or
                                                       search_term.lower() in book.author.lower() or
                                                       search_term in book.isbn)]
        if found_books:
            print("(>_<) ! -- ✩ Libros encontrados:")
            for book in found_books:
                print(f"Título: {book.title}, Autor: {book.author}, ISBN: {book.isbn}")
        else:
            print("No se encontraron libros con ese término de búsqueda. (⸝⸝ᴗ﹏ᴗ⸝⸝) ᶻ 𝗓 𐰁")

    def is_book_available(self, book_title, readers_queue):
        for libro in self.books:
            if libro.title.lower() == book_title.lower():
                for reader in readers_queue:
                    if reader.book_title.lower() == book_title.lower():
                        return False  # El libro ya está prestado
                return True  # El libro está disponible
        raise BookNotFoundError(f"(>_<) ! El libro '{book_title}' no existe en la biblioteca.")

    def guardar_libros_en_archivo(self, filename):
        with open(filename, "w") as file:
            for book in self.books:
                file.write(f"{book.title}\n{book.author}\n{book.year}\n"
                           f"{book.publisher}\n{book.isbn}\n{book.pages}\n")

    def guardar_solicitudes_en_archivo(self, queue, filename):
       with open(filename, "w") as file:
        for reader in queue:
            file.write(f"{reader.name},{reader.dni},{reader.book_title}\n")

    def guardar_atendidos_en_archivo(self, queue, filename):
       with open(filename, "w") as file:
        for reader in queue:
            file.write(f"{reader.name},{reader.dni},{reader.book_title}\n")

                

    def load_atendidos_from_file(self, filename, readers_queue):
        try:
            with open(filename, "r") as file:
                lines = file.readlines()
                for line in lines:
                    line = line.strip()
                    if not line:
                        continue
                    values = line.split(",")
                    if len(values) == 3:
                        name, dni, book_title = values
                        reader = Reader(name, dni, book_title)
                        readers_queue.append(reader)
                    else:
                        print(f"Línea ignorada por formato incorrecto: {line}")
        except FileNotFoundError:
            print(f"No se encontró el archivo {filename}. Se creará uno nuevo.")

    def load_solicitudes_from_file(self, filename, waiting_queue):
        try:
            with open(filename, "r") as file:
                lines = file.readlines()
                for line in lines:
                    line = line.strip()
                    if not line:
                        continue
                    values = line.split(",")
                    if len(values) == 3:
                        name, dni, book_title = values
                        waiting_queue.append(Reader(name, dni, book_title))
                    else:
                        print(f"Línea ignorada por formato incorrecto: {line}")
        except FileNotFoundError:
            print(f"No se encontró el archivo {filename}. Se creará uno nuevo.")

    def ordenar_libros_bubble_sort(self):
        n = len(self.books)
        swapped = True
        while swapped:
            swapped = False
            for i in range(n - 1):
                title1 = self.books[i].title.upper()
                title2 = self.books[i + 1].title.upper()
                if title1 > title2:
                    self.books[i], self.books[i + 1] = self.books[i + 1], self.books[i]
                    swapped = True

    def validate_isbn(self, isbn):  
        if len(isbn) != 13 or not isbn.isdigit():
            raise ValueError("El ISBN debe tener 13 dígitos y ser numérico. ( – ⌓ – )")

    def deshacer_accion(self):
     if not self.history_stack:
        print("( – ⌓ – )No hay acciones para deshacer.")
        return

     last_action = self.history_stack.pop()
     action_type, item = last_action

     if action_type == "agregar":
        if item in self.books:
            self.books.remove(item)
            self.guardar_libros_en_archivo("biblioteca_py.txt")
            print(f"Se ha deshecho la acción de agregar el libro: {item.title} (๑>؂•̀๑)")


def devolver_libro(readers_queue, waiting_queue, archivo_2, archivo_3, biblioteca):
    dni = input("Ingrese el DNI del lector que desea devolver el libro: ")

    for reader in readers_queue:
        if reader.dni == dni:
            print(f"₊˚⊹♡ Lector encontrado: {reader.name}, Libro: {reader.book_title}")
            confirm = input("¿Desea devolver el libro? (Y/N): ")
            if confirm.lower() == 'y':
                biblioteca.history_stack.append(("devolver", reader))
                readers_queue.remove(reader)  
                print("Libro devuelto exitosamente. (˵ •̀ ᴗ - ˵ ) ✧")
                
                if waiting_queue:
                    next_reader = waiting_queue.popleft()  
                    readers_queue.append(next_reader)  
                    print(f"*ੈ✩‧₊˚ La solicitud de {next_reader.name} fue atendida con el libro '{next_reader.book_title}'.")
                    biblioteca.history_stack.append(("devolver", next_reader))
                else:
                    print("No hay más solicitudes en espera. (ㅅ´ ˘ `)")
                
                biblioteca.guardar_atendidos_en_archivo(readers_queue, archivo_3)
                biblioteca.guardar_solicitudes_en_archivo(waiting_queue, archivo_2)
                return

    print("No se encontró un lector con ese DNI. (⸝⸝ᴗ﹏ᴗ⸝⸝) ᶻ 𝗓 𐰁")

def show_queue(queue, queue_name):
    print(f"{queue_name}:")
    if not queue:
        print("No hay lectores en la cola. (,,>﹏<,,)")
    else:
        for reader in queue:
            print(f"✧˚ ༘ Nombre: {reader.name}, ⋆｡♡˚ DNI: {reader.dni}, ✧˚ ༘ Libro solicitado: {reader.book_title}")

def main():
    readers_queue = deque()
    waiting_queue = deque()
    biblioteca = Biblioteca()

    # Carga de libros antes de ordenar
    archivo = "biblioteca_py.txt"
    archivo_2 = "solicitudes_py.txt"
    archivo_3 = "atendidos_py.txt"

    biblioteca.load_books_from_file(archivo)
    biblioteca.load_solicitudes_from_file(archivo_2, waiting_queue)
    biblioteca.load_atendidos_from_file(archivo_3, readers_queue)

    biblioteca.ordenar_libros_bubble_sort()

    while True:
        print()
        print("⋆｡ﾟ☁︎｡⋆｡ ﾟ☾ ﾟ｡⋆")
        print("\n1. Mostrar libros")
        print("2. Agregar libro")
        print("3. Solicitar libro")
        print("4. Mostrar colas")
        print("5. Buscar libro")
        print("6. Devolver libro")
        print("7. Deshacer acción")
        print("8. Salir")
        print()
        print("⋆｡ﾟ☁︎｡⋆｡ ﾟ☾ ﾟ｡⋆")
        print()
        choice = input("Seleccione una opción: ")
       

        if choice == '1':
            biblioteca.display_books()
        elif choice == '2':
            biblioteca.agregar_libro(archivo)
        elif choice == '3':
            name = input("✩ ₊˚Ingrese su nombre: ")
            dni = input(" ೀ⋆｡˚ Ingrese su DNI: ")
            book_title = input("✩ ₊˚ Ingrese el título del libro que desea solicitar: ")

            if biblioteca.is_book_available(book_title, readers_queue):
              readers_queue.append(Reader(name, dni, book_title))
              print("Solicitud agregada a la cola de atendidos. (੭˃ᴗ˂)੭")
              biblioteca.guardar_atendidos_en_archivo(readers_queue, archivo_3)
            else:
              waiting_queue.append(Reader(name, dni, book_title))
              print(f"(｡•́︿•̀｡) . . . El libro '{book_title}' no está disponible en este momento.")
              biblioteca.guardar_solicitudes_en_archivo(waiting_queue, archivo_2)
        elif choice == '4':
            print("⊹˚. ♡ Lectores atendidos:")
            for reader in readers_queue:
                print(f"{reader.name} (DNI: {reader.dni}) solicitó '{reader.book_title}'")
            print("ꕤ*.ﾟ Solicitudes en espera:")
            for reader in waiting_queue:
                print(f"{reader.name} (DNI: {reader.dni}) solicitó '{reader.book_title}'")
        elif choice == '5':
            search_term = input("☾ ﾟ｡ ⋆ Ingrese el título, autor o ISBN: ")
            biblioteca.buscar_libro(search_term)
        elif choice == '6':
                devolver_libro(readers_queue, waiting_queue, archivo_2, archivo_3, biblioteca)
        elif choice == '7':
            biblioteca.deshacer_accion()
        elif choice == '8':
            print("Saliendo del sistema. (*ᴗ͈ˬᴗ͈)")
            break
        else:
            print("( ｡ •̀ ᴖ •́ ｡). . . Opción no válida. Inténtelo de nuevo.")

if __name__ == "__main__":
    main()

# https://www.geeksforgeeks.org/queue-in-python/
# https://www.geeksforgeeks.org/python-linked-list/
# https://www.geeksforgeeks.org/reading-writing-text-files-python/
# https://stackoverflow.com/questions/48723387/searching-for-specific-words-in-text-file-python // para el search
# https://docs.python.org/3/tutorial/errors.html
# https://www.geeksforgeeks.org/stack-in-python/ 
# https://www.w3schools.com/python/ref_string_isdigit.asp