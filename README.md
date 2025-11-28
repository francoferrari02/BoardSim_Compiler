# BoardSim - Compilador de Simulación de Juegos de Mesa

Compilador que implementa un DSL (Domain Specific Language) para la simulación de juegos de mesa usando Flex y Bison.

## Requisitos

- Docker (recomendado) o sistema Linux con:
  - GCC
  - CMake 3.10+
  - Flex
  - Bison

## Instalación y Uso

### Con Docker (recomendado)

```sh
docker compose run --rm compiler
```

Dentro del contenedor:

```sh
# Compilar el proyecto
./src/main/bash/build.sh

# Ejecutar una simulación
.build/Flex-Bison-Compiler archivo.bsim salida.txt

# Ejecutar tests
./src/main/bash/test.sh
```

### Sin Docker (Linux)

```sh
# Compilar el proyecto
./src/main/bash/build.sh

# Ejecutar una simulación
.build/Flex-Bison-Compiler archivo.bsim debug.txt
//ver simulation_log.txt para ver el programa

# Ejecutar tests
./src/main/bash/test.sh
```

## Variables de Entorno

| Variable | Valor Por Defecto | Descripción |
|----------|-------------------|-------------|
| `LOGGING_LEVEL` | `ALL` | Nivel mínimo de logging (ALL, DEBUGGING, INFORMATION, WARNING, ERROR, CRITICAL) |
| `LOG_IGNORED_LEXEMES` | `true` | Mostrar lexemas ignorados en debug |

## Estructura de Archivos de Entrada

Los archivos `.bsim` deben contener definiciones de:
- Tablero (`board`)
- Casillas (`cell`)
- Jugadores (`player`)
- Dados (`dice`)
- Bloque de simulación (`simulate`)

Ejemplos disponibles en `src/test/c/accept/`.
