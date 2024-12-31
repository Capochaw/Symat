# Compilador y flags
CC = gcc
CFLAGS = -Wall -Iinc -Isrc/buffers -lm -lSDL2 -lSDL2_ttf -lSDL2_image -lunistring -g
#CFLAGS = -Wall -Iinc -lunistring -g `MagickWand-config --cflags --cppflags` `MagickWand-config --ldflags --libs`
# CFLAGS = -Wall -Iinc -Isrc/buffers (cada vez que quiera meter una carpeta mas dentro del src, linkear asi) -lSDL2 -lSDL2_ttf -lSDL2_image -lunistring -g
# TODO: Implementar que pueda generar imagenes tambien.


# Directorios
SRC_DIR = src
OBJ_DIR = obj
BIN_DIR = bin

# Nombre del ejecutable
TARGET = $(BIN_DIR)/mathparser

# Encontrar todos los archivos .c dentro de src y sus subdirectorios
SRC_FILES = $(shell find $(SRC_DIR) -name "*.c")
# Los archivos .o se generarán en la carpeta obj
OBJ_FILES = $(SRC_FILES:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

# Reglas
all: $(BIN_DIR) $(OBJ_DIR) $(TARGET)

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

# Compilar el ejecutable final
$(TARGET): $(OBJ_FILES)
	$(CC) $(CFLAGS) -o $@ $(OBJ_FILES) -ly -lfl

# Regla genérica para compilar archivos .c
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# Limpiar archivos generados
clean:
	rm -rf $(OBJ_DIR)

.PHONY: all clean
