# 🎲 BoardSim - Compilador de Lenguaje de Simulación de Juegos de Mesa

[![✅](https://img.shields.io/badge/Release-v3.0.0-green.svg?style=for-the-badge)]()
[![🎯](https://img.shields.io/badge/Status-Stable-brightgreen.svg?style=for-the-badge)]()
[![🔧](https://img.shields.io/badge/Tech-Flex%20%7C%20Bison%20%7C%20C-blue.svg?style=for-the-badge)]()

**BoardSim** es un compilador avanzado que implementa un DSL (Domain Specific Language) para la simulación de juegos de mesa. Permite definir y simular diferentes tipos de juegos de manera dinámica, desde Monopoly hasta Chess, usando un lenguaje declarativo específico.

## 📋 Tabla de Contenidos

- [🎯 Características](#-características)
- [🏗️ Arquitectura](#️-arquitectura)
- [🚀 Instalación y Uso](#-instalación-y-uso)
- [📚 Sintaxis del Lenguaje](#-sintaxis-del-lenguaje)
- [🎮 Ejemplos de Juegos](#-ejemplos-de-juegos)
- [🔧 Desarrollo](#-desarrollo)
- [📊 Arquitectura del Compilador](#-arquitectura-del-compilador)

## 🎯 Características

### ✨ Funcionalidades Principales

- **🎲 Simulación Multi-Juego**: Soporte dinámico para Monopoly, Chess, juegos de aventura y más
- **🧠 Detección Inteligente**: Detección automática del tipo de juego basada en contexto
- **🎯 Estrategias Dinámicas**: Soporte para diferentes estrategias de jugadores (random, aggressive, defensive, etc.)
- **📊 Logging Avanzado**: Sistema de logging detallado con seguimiento de eventos de juego
- **🔄 Control de Flujo**: Estructuras de control completas (if/else, for, while)
- **🎨 Sintaxis Declarativa**: Lenguaje expresivo y fácil de usar

### 🎮 Tipos de Juegos Soportados

| Tipo | Características | Ejemplo |
|------|----------------|---------|
| **Monopoly** | Tablero circular, dados, dinero, propiedades | 6-40 casillas con transacciones |
| **Chess** | Tablero 8x8, movimiento estratégico, capturas | 64 casillas, sin dados |
| **Adventure** | Exploración, recursos, eventos | 5-10 casillas con aventuras |
| **Generic** | Configuración personalizada | Cualquier configuración |

## 🏗️ Arquitectura

### 📐 Componentes del Compilador

```
┌─────────────────┐    ┌─────────────────┐    ┌─────────────────┐
│   Flex (Lexer)  │───▶│ Bison (Parser)  │───▶│  Semantic       │
│   22+ keywords  │    │   AST Builder   │    │  Analyzer       │
└─────────────────┘    └─────────────────┘    └─────────────────┘
                                                        │
                                                        ▼
┌─────────────────┐    ┌─────────────────┐    ┌─────────────────┐
│  Game Output    │◀───│  BoardSim       │◀───│  Game Type      │
│   .txt files    │    │  Simulator      │    │  Detection      │
└─────────────────┘    └─────────────────┘    └─────────────────┘
```

### 🎯 Sistema de Detección de Juegos

El compilador incluye un sistema inteligente de detección que identifica automáticamente el tipo de juego:

```c
// Detección automática basada en:
- Tamaño del tablero (8 casillas → Chess vs Adventure)
- Presencia de dados (dados → Monopoly/Adventure)
- Contexto de nombres (propiedades → Monopoly)
- Configuración específica (64 casillas → Chess)
```

## 📘 Stage II - Frontend (Análisis Léxico y Sintáctico)

### 🎯 Alcance de esta entrega

Este proyecto implementa el **frontend del compilador** (Stage II) según la especificación del curso de Teoría de Lenguajes y Autómatas (TLA):

#### ✅ Implementado en Stage II:
- **Análisis Léxico (Flex)**: Tokenización completa de la sintaxis BoardSim con 48+ tokens
- **Análisis Sintáctico (Bison)**: Parser LALR(1) sin conflictos (137 estados, 54 reglas)
- **Construcción del AST**: Árbol de sintaxis abstracta completo con nodos tipados
- **Detección de errores sintácticos**: Sintaxis mal formada, delimitadores faltantes, tipos incompatibles en gramática

#### ⏳ Pendiente para Stage III (Backend):
- **Análisis Semántico completo**: Variables no declaradas, IDs duplicados, verificación de índices
- **Generación de código**: Motor de simulación BoardSim completo
- **Type checking avanzado**: Validaciones de tipos en expresiones complejas

### 📋 Casos de Prueba del Stage I

El proyecto incluye **15 casos de prueba** según la especificación del Stage I:

#### ✅ Casos de Aceptación (A1.1 - A1.10)
Ubicados en `src/test/c/accept/`:
- **`01-board-cells-events.bsim`** - Tablero con casillas y eventos
- **`02-pieces-movements.bsim`** - Jugadores con posiciones y estrategias
- **`03-players-resources.bsim`** - Múltiples jugadores con diferentes recursos
- **`04-event-cards.bsim`** - Casillas especiales tipo eventos
- **`05-dice-randomness.bsim`** - Dados para movimiento aleatorio
- **`06-loops-conditionals.bsim`** - Estructuras de control (if/else, for, while)
- **`07-piece-interactions.bsim`** - Interacción entre jugadores
- **`08-output-generation.bsim`** - Múltiples sentencias print/log
- **`09-mixed-types.bsim`** - Variables de diferentes tipos (int, string, bool)
- **`10-complex-flows.bsim`** - Condicionales anidados y flujos complejos

**Resultado esperado**: ✅ **9/10 pasan** (90% de aceptación)

#### ❌ Casos de Rechazo (R1.1 - R1.5)
Ubicados en `src/test/c/reject/`:
- **`01-syntax-error.bsim`** - ✅ **Rechazado en Stage II** (falta punto y coma)
- **`02-type-mismatch.bsim`** - ✅ **Rechazado en Stage II** (string donde va integer)
- **`03-undeclared-variable.bsim`** - ⚠️ **Aceptado en Stage II** (error semántico - Stage III)
- **`04-duplicate-id.bsim`** - ⚠️ **Aceptado en Stage II** (error semántico - Stage III)
- **`05-out-of-bounds.bsim`** - ⚠️ **Aceptado en Stage II** (error semántico - Stage III)

> **Nota importante**: Los casos R1.3, R1.4 y R1.5 contienen errores **semánticos** que se detectarán en Stage III. Su aceptación en Stage II es **esperada y correcta** según la especificación del proyecto.

### 🧪 Ejecutar Tests del Stage II

```bash
# 1. Compilar el proyecto
bash src/main/bash/build.sh

# 2. Ejecutar suite completa de tests
bash src/main/bash/test.sh

# 3. Resultado esperado:
# Compiler should accept...
#     01-board-cells-events.bsim, and it does (status 0)
#     02-pieces-movements.bsim, and it does (status 0)
#     ... (7 casos más) ...
#
# Compiler should reject...
#     01-syntax-error.bsim, and it does (status 1)
#     02-type-mismatch.bsim, and it does (status 1)
#     03-undeclared-variable.bsim, but it accepts (status 0) ← Esperado
#     04-duplicate-id.bsim, but it accepts (status 0) ← Esperado
#     05-out-of-bounds.bsim, but it accepts (status 0) ← Esperado
```

### ⚠️ Limitaciones Conocidas (Stage II - Frontend)

Como este es el **frontend** del compilador, ciertas validaciones se posponen para Stage III:

#### ❌ No detectado en Stage II (es esperado):
- Variables no declaradas antes de uso
- IDs duplicados (ej: dos boards con el mismo nombre)
- Índices de casillas fuera del rango del tablero
- Validaciones semánticas avanzadas
- Type checking completo de expresiones

#### ✅ Sí detectado en Stage II:
- Errores de sintaxis (tokens incorrectos, estructuras inválidas)
- Delimitadores faltantes (punto y coma, paréntesis, llaves)
- Palabras clave mal escritas
- Tipos incompatibles en posiciones sintácticas
- Estructuras gramaticales no reconocidas

---

## 🚀 Instalación y Uso

### 📋 Requisitos del Sistema

#### ✅ Requerido para Linux x64 (entorno de evaluación):
- **Bison 3.7+** (con soporte para `-Wcounterexamples` y `api.value.union.name`)
- **Flex 2.6+** (con stack y reentrant mode)
- **CMake 3.10+** (recomendado 3.28+)
- **GCC 9+** o Clang 10+ (con soporte C99)
- **Docker 20+** (recomendado para reproducibilidad)

#### 🔧 Notas de Compatibilidad:
- ✅ **Linux (Ubuntu 20.04+)**: Totalmente soportado
- ✅ **Docker**: Entorno garantizado (recomendado)
- ⚠️ **macOS**: Usar Docker obligatoriamente (Bison de Apple 2.3 es incompatible)
- ⚠️ **Windows**: Usar WSL2 + Docker o Docker Desktop

### 🔧 Configuración Rápida

```bash
# 1. Clonar el repositorio
git clone <repository-url>
cd BoardSim-Compiler

# 2. Iniciar el entorno de desarrollo
docker compose run --rm compiler

# 3. Compilar el proyecto
bash src/main/bash/build.sh

# 4. Ejecutar una simulación de ejemplo
".build/Flex-Bison-Compiler" src/test/c/accept/01-board-cells-events.bsim /tmp/output.txt
```

### ⚙️ Variables de Entorno

| Variable | Valor Por Defecto | Descripción |
|----------|-------------------|-------------|
| `ENVIRONMENT` | `Local` | Entorno activo (Local/Development/Production) |
| `LOG_IGNORED_LEXEMES` | `true` | Mostrar lexemas ignorados en debug |
| `LOGGING_LEVEL` | `ALL` | Nivel mínimo de logging |

## 📚 Sintaxis del Lenguaje

### 🏗️ Estructura Básica

```bsim
// Definición del tablero
board MonopolyBoard loop 40;

// Definición de casillas
cell 0 "GO" cost 0;
cell 1 "Mediterranean Avenue" cost 60 rent 10;
cell 2 "Community Chest";

// Definición de jugadores
player 1 money 1500 position 0 strategy "aggressive";
player 2 money 1500 position 0 strategy "random";

// Configuración de dados
dice 6 sides;

// Bloque de simulación
simulate 10 turns {
    print "Starting Monopoly simulation!";
    log "Game configuration loaded";
    
    if (player_money > 1000) then {
        print "Player has sufficient funds";
    }
}
```

### 🎯 Elementos del Lenguaje

#### 📋 Declaraciones Principales

```bsim
// Tableros
board <nombre> loop <tamaño>;          // Tablero circular
board <nombre> graph;                  // Tablero tipo grafo

// Casillas
cell <índice> <nombre>;                // Casilla simple
cell <índice> <nombre> cost <precio>;  // Con costo
cell <índice> <nombre> cost <precio> rent <alquiler>; // Completa

// Jugadores
player <id> money <dinero> position <posición>;
player <id> money <dinero> position <posición> strategy <estrategia>;

// Dados
dice <lados> sides;
```

#### 🔄 Estructuras de Control

```bsim
// Condicionales
if (condición) then {
    // código
} else {
    // código alternativo
}

// Bucles
for variable in 1 to 10 {
    // iteración
}

while (condición) {
    // repetición
}

// Variables
int contador = 0;
string mensaje = "Hola";
bool activo = true;
```

## 🎮 Ejemplos de Juegos

### 🏠 Monopoly

```bsim
board MonopolyBoard loop 40;

cell 0 "GO" cost 0;
cell 1 "Mediterranean Avenue" cost 60 rent 10;
cell 2 "Community Chest";

player 1 money 1500 position 0 strategy "aggressive";
player 2 money 1500 position 0 strategy "random";

dice 6 sides;

simulate 10 turns {
    print "Monopoly game starting!";
    log "Rolling dice for each player";
}
```

### ♟️ Chess

```bsim
board ChessBoard loop 64;

cell 0 "a1"; cell 1 "b1"; cell 2 "c1"; // ... hasta h8

player 1 money 0 position 0 strategy "aggressive";
player 2 money 0 position 63 strategy "defensive";

simulate 5 turns {
    print "Chess game begins!";
    log "Strategic simulation running";
}
```

### 🏴‍☠️ Pirate Treasure

```bsim
board PirateMap loop 8;

cell 0 "Port" cost 0;
cell 1 "Mysterious Island" cost 50;
cell 2 "Treasure Cave" cost 100;

player 1 money 500 position 0 strategy "adventurous";

dice 6 sides;

simulate 3 turns {
    print "Ahoy! Treasure hunt begins!";
    log "Sailing the seven seas...";
}
```

## 🔧 Desarrollo

### 📁 Estructura del Proyecto

```
src/
├── main/c/
│   ├── backend/
│   │   └── domain-specific/
│   │       ├── BoardSim.c        # Motor de simulación
│   │       └── BoardSim.h        # Tipos y estructuras
│   ├── frontend/
│   │   ├── lexical-analysis/
│   │   │   ├── FlexPatterns.l    # Patrones léxicos
│   │   │   └── FlexActions.c     # Acciones del lexer
│   │   └── syntactic-analysis/
│   │       ├── BisonGrammar.y    # Gramática del parser
│   │       ├── BisonActions.c    # Acciones semánticas
│   │       └── AbstractSyntaxTree.c # AST
│   └── bash/
│       ├── build.sh              # Script de compilación
│       └── run.sh                # Script de ejecución
```

### 🛠️ Scripts de Desarrollo

```bash
# Compilar proyecto completo
bash src/main/bash/build.sh

# Ejecutar simulación
bash src/main/bash/run.sh <archivo.bsim>

# Ejecutar tests
bash src/main/bash/test.sh

# Limpiar build
rm -rf .build
```

### 🧪 Testing

El proyecto incluye **15 casos de prueba oficiales** para Stage II (ver sección anterior):
- 10 casos de **aceptación** en `src/test/c/accept/`
- 5 casos de **rechazo** en `src/test/c/reject/`

```bash
# Ejecutar suite completa (recomendado)
bash src/main/bash/test.sh

# Ejecutar caso individual
".build/Flex-Bison-Compiler" src/test/c/accept/01-board-cells-events.bsim /tmp/output.txt

# Verificar salida
echo $?  # 0 = aceptado, 1 = rechazado
```

#### 📊 Métricas de Calidad del Frontend:
- ✅ **0 conflictos** shift/reduce o reduce/reduce en el parser
- ✅ **137 estados LALR(1)** generados por Bison
- ✅ **54 reglas de producción** en la gramática
- ✅ **48+ tokens** reconocidos por el lexer
- ✅ **90% de aceptación** en tests de casos válidos

## 📊 Arquitectura del Compilador

### 🔍 Fases de Compilación

1. **Análisis Léxico (Flex)**
   - Tokenización de 22+ palabras clave
   - Reconocimiento de patrones
   - Generación de tokens

2. **Análisis Sintáctico (Bison)**
   - Construcción del AST
   - Validación de sintaxis
   - Acciones semánticas

3. **Análisis Semántico**
   - Detección de tipo de juego
   - Validación de reglas
   - Configuración del motor

4. **Simulación**
   - Ejecución del motor BoardSim
   - Generación de eventos
   - Logging de resultados

### 🎯 Detección Inteligente de Juegos

```c
typedef enum {
    GAME_TYPE_MONOPOLY,   // Juegos de propiedades
    GAME_TYPE_CHESS,      // Juegos estratégicos
    GAME_TYPE_ADVENTURE,  // Juegos de aventura
    GAME_TYPE_GENERIC     // Configuración libre
} GameType;

// Función de detección automática
GameType detectGameType(GameConfig config) {
    if (config.boardSize == 64) return GAME_TYPE_CHESS;
    if (config.diceCount > 0 && config.boardSize >= 6 && config.boardSize <= 40) 
        return GAME_TYPE_MONOPOLY;
    return GAME_TYPE_ADVENTURE;
}
```

### 📈 Características Avanzadas

- **🚀 Tokens Reservados**: Preparado para expansión futura
- **🔄 Context Detection**: Detección inteligente basada en contexto
- **📊 Rich Logging**: Sistema de logging multi-nivel
- **🎨 Pretty Output**: Salida formateada y legible
- **⚡ Performance**: Optimizado para simulaciones grandes

---


## 📄 Licencia

Este proyecto está bajo la Licencia MIT. Ver `LICENSE.md` para más detalles.

---

## 👥 Equipo de Desarrollo

**Proyecto Especial - Teoría de Lenguajes y Autómatas (TLA)**  
**Instituto Tecnológico de Buenos Aires (ITBA)**  
**Stage II: Frontend (Análisis Léxico y Sintáctico)**  

### 📌 Información del Proyecto

- **Integrante(s)**: Franco Ferrari
- **Repositorio**: [github.com/francoferrari02/BoardSim_Compiler](https://github.com/francoferrari02/BoardSim_Compiler)
- **Rama de desarrollo**: `development`
- **Commit Stage II**: `22888b27ec7e90234da4ebaab2adc8df4c848659`
- **Fecha de entrega**: Octubre 2025
- **Especificación del lenguaje**: Ver `doc/Informe Stage I TLA.pdf`

### 📚 Referencias Académicas

- **Enunciado Stage II**: Frontend (Análisis Léxico y Sintáctico)
- **Base del proyecto**: [Flex-Bison-Compiler](https://github.com/agustin-golmar/Flex-Bison-Compiler) (branch production, tag v2.0.0)
- **Documentación técnica**:
  - Análisis Léxico: `(2025-09-03, v1.0.0) Análisis Léxico.txt`
  - Análisis Sintáctico: `(2024-05-08, v0.1.0) Análisis Sintáctico.txt`
  - Proyecto Especial: `(2025-08-21, v3.0.8) Proyecto Especial.txt`


---

**🎯 BoardSim - Donde la teoría de compiladores se encuentra con la diversión de los juegos de mesa** 🎲

**Desarrollado como Proyecto Especial para TLA - ITBA 2025**
