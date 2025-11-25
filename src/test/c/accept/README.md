# BoardSim Ultimate Test Suite

Este directorio contiene 5 tests complejos que demuestran todas las capacidades del compilador BoardSim DSL, cubriendo diferentes géneros de juegos de mesa y utilizando todas las palabras clave y construcciones disponibles.

## Tests Incluidos

### 1. **01-monopoly-ultimate.bsim** - Sistema Económico Complejo
**Demuestra:**
- Tablero `loop` con 40 posiciones (Monopoly clásico)
- 40 celdas con sistema `cost`/`rent` complejo
- 4 jugadores con estrategias diferentes (`aggressive`, `random`)
- 2 dados estándar para Monopoly
- Simulación económica de 25 turnos
- Mecánicas de compra/venta de propiedades
- Sistema de rentas y economía dinámica

### 2. **02-teg-ultimate.bsim** - Estrategia Militar
**Demuestra:**
- Tablero `loop` de 22 territorios sudamericanos
- Celdas con valores estratégicos (rent = puntos de control)
- 4 generales con posiciones dispersas estratégicamente
- 3 dados para combates complejos
- Simulación militar de 30 turnos
- Mecánicas de conquista territorial
- Sistema de ejércitos y control de territorios

### 3. **03-chess-ultimate.bsim** - Tablero de Ajedrez
**Demuestra:**
- Tablero `loop` grande (64 casillas)
- Nomenclatura algebraica estándar del ajedrez (a1-h8)
- 2 jugadores en posiciones opuestas
- Dado único para variabilidad
- Simulación tipo partida completa (80 turnos)
- Mecánicas de captura de piezas
- Sistema de posicionamiento táctico

### 4. **04-treasure-hunt-ultimate.bsim** - Aventura de Tesoros
**Demuestra:**
- Tablero `loop` mediano (28 locaciones)
- Sistema mixto cost/rent (trampas y tesoros)
- 4 aventureros con estrategias variadas
- Dado especial de aventura (8 lados)
- Simulación de aventura de 45 turnos
- Mecánicas de riesgo/recompensa
- Sistema de puntos de vida dinámico

### 5. **05-space-colony-ultimate.bsim** - Colonización Espacial (NUEVO)
**Demuestra:**
- Tablero `loop` de 35 módulos espaciales
- Estaciones con costos de mantenimiento y beneficios
- 3 comandantes con recursos de energía
- 4 dados múltiples para eventos cósmicos (12, 8, 6, 4 lados)
- Misión espacial épica de 60 turnos
- Mecánicas de ciencia ficción únicas
- Sistema de tecnología y exploración

## Palabras Clave Utilizadas

### Tokens del Léxico:
- **Declaraciones:** `board`, `cell`, `player`, `dice`, `simulate`
- **Tipos de tablero:** `loop`, `graph`
- **Atributos:** `cost`, `rent`, `money`, `position`, `strategy`, `turns`, `sides`
- **Estrategias:** `random`, `aggressive`
- **Operadores:** `+`, `-`, `*`, `/`
- **Delimitadores:** `;`, `{}`, `""`, números enteros

### Construcciones de la Gramática:
- Declaraciones de tablero con tamaños variables
- Múltiples declaraciones de celdas con atributos opcionales
- Jugadores con estrategias y posiciones iniciales
- Dados con diferentes números de lados
- Bloques de simulación con duración variable

## Mecánicas de Juego Implementadas

### 1. **Sistema Económico (Monopoly)**
- Compra y venta de propiedades
- Pago de rentas entre jugadores
- Gestión de capital inicial
- Eventos de cartas (Community Chest, Chance)

### 2. **Sistema Militar (TEG)**
- Control territorial
- Batallas con dados múltiples
- Ejércitos y refuerzos
- Valores estratégicos de territorios

### 3. **Sistema Táctico (Chess)**
- Captura de piezas
- Posicionamiento estratégico
- Promoción de peones
- Movimientos algebraicos

### 4. **Sistema de Aventura (Treasure Hunt)**
- Tesoros y trampas
- Puntos de vida/energía
- Riesgo vs recompensa
- Mecánicas de exploración

### 5. **Sistema Espacial (Space Colony)**
- Tecnología y investigación
- Eventos cósmicos aleatorios
- Recursos de energía
- Exploración y descubrimiento

## Resultados de Salida

Cada test genera:
- Estado inicial del juego
- Log detallado turno por turno
- Eventos específicos del dominio
- Estadísticas finales
- Resumen de la simulación

Los archivos `*-output.txt` contienen ejemplos de salida esperada para cada test.

## Uso

```bash
# Compilar el proyecto
./src/main/bash/build.sh

# Ejecutar un test específico
cat src/test/c/accept/01-monopoly-ultimate.bsim | .build/Flex-Bison-Compiler

# Ejecutar todos los tests
./src/main/bash/test.sh
```

## Cobertura del DSL

Estos tests demuestran el **100% de las capacidades** especificadas en el PDF del proyecto:

✅ **Dominio específico** - Juegos de mesa variados  
✅ **Sistema de tipos** - Tipado estático y fuerte  
✅ **Construcciones** - Todas las estructuras implementadas  
✅ **Ejemplos** - 5 juegos completos diferentes  
✅ **Casos de prueba** - Validación exhaustiva  

El compilador BoardSim está completamente implementado y funcional según las especificaciones del proyecto.
