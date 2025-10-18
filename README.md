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

## 🚀 Instalación y Uso

### 📋 Requisitos

- **Docker v28.3.2+**
- **CMake 3.10+**
- **GCC/Clang**
- **Flex & Bison**

### 🔧 Configuración Rápida

```bash
# 1. Clonar el repositorio
git clone <repository-url>
cd BoardSim-Compiler

# 2. Iniciar el entorno de desarrollo
docker compose run --rm compiler

# 3. Compilar el proyecto
bash src/main/bash/build.sh

# 4. Ejecutar una simulación
".build/Flex-Bison-Compiler" monopoly.bsim monopoly-output.txt
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

El proyecto incluye tres tests representativos:

- **`monopoly.bsim`**: Simulación clásica de Monopoly
- **`chess.bsim`**: Partida estratégica de ajedrez  
- **`pirate-treasure.bsim`**: Aventura de búsqueda del tesoro

```bash
# Ejecutar todos los tests
".build/Flex-Bison-Compiler" monopoly.bsim monopoly-output.txt
".build/Flex-Bison-Compiler" chess.bsim chess-output.txt
".build/Flex-Bison-Compiler" pirate-treasure.bsim pirate-output.txt
```

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

## 🤝 Contribuir

¡Las contribuciones son bienvenidas! Por favor:

1. Fork el proyecto
2. Crea una rama feature (`git checkout -b feature/AmazingFeature`)
3. Commit tus cambios (`git commit -m 'Add some AmazingFeature'`)
4. Push a la rama (`git push origin feature/AmazingFeature`)
5. Abre un Pull Request

## 📄 Licencia

Este proyecto está bajo la Licencia MIT. Ver `LICENSE.md` para más detalles.

---

**🎯 BoardSim - Donde la teoría de compiladores se encuentra con la diversión de los juegos de mesa** 🎲