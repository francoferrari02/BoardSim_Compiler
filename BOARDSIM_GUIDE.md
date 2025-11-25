# 🎲 BoardSim - Guía Completa para Creadores de Juegos de Mesa

**BoardSim** es un **Lenguaje de Dominio Específico (DSL)** que permite a los creadores de juegos de mesa **definir, simular y probar** sus juegos usando una sintaxis simple y poderosa.

## 🚀 ¿Qué puedes hacer con BoardSim?

- ✅ **Crear cualquier juego de mesa** con tableros, jugadores, dados y reglas
- ✅ **Simular partidas completas** automáticamente  
- ✅ **Probar mecánicas** antes de crear el prototipo físico
- ✅ **Generar estadísticas** de balance y diversión
- ✅ **Exportar logs detallados** para análisis

---

## 📥 Instalación y Uso

### 1. **Clonar el proyecto**
```bash
git clone <tu-repositorio>
cd TLA
```

### 2. **Compilar el sistema**
```bash
./src/main/bash/build.sh
```

### 3. **Ejecutar tu juego**
```bash
./src/main/bash/run.sh mi-juego.bsim
```

### 4. **Ver los resultados**
Los resultados se guardan automáticamente en `mi-juego.txt`

---

## 📚 Sintaxis del Lenguaje BoardSim

### 🏗️ **1. Definir el Tablero**

```bsim
board NombreDelJuego tipo tamaño;
```

**Ejemplo:**
```bsim
board MiJuegoEpico loop 20;  // Tablero circular de 20 casillas
```

### 🏠 **2. Definir Casillas**

```bsim
cell índice "Nombre" cost costo rent ganancia;
```

**Parámetros:**
- **`índice`**: Número de la casilla (0, 1, 2, ...)
- **`"Nombre"`**: Nombre descriptivo entre comillas
- **`cost`**: Dinero que cuesta al caer (se pierde)
- **`rent`**: Dinero que se gana al caer

**Ejemplos:**
```bsim
cell 0 "Inicio" cost 0 rent 200;           // Casilla de inicio: ganas $200
cell 1 "Propiedad Cara" cost 150 rent 50;  // Pierdes $150, pero ganas $50
cell 2 "Trampa" cost 100 rent 0;           // Solo pierdes $100
cell 3 "Tesoro" cost 0 rent 300;           // Solo ganas $300
```

### 🎭 **3. Definir Jugadores**

```bsim
player número money dinero_inicial position posición_inicial strategy "estrategia";
```

**Estrategias disponibles:**
- `"aggressive"` - Juego agresivo
- `"conservative"` - Juego conservador  
- `"balanced"` - Juego equilibrado
- `"tactical"` - Juego táctico
- `"random"` - Juego aleatorio

**Ejemplos:**
```bsim
player 1 money 1500 position 0 strategy "aggressive";
player 2 money 1500 position 0 strategy "conservative";
player 3 money 1000 position 5 strategy "balanced";
```

### 🎲 **4. Definir Dados**

```bsim
dice caras sides;
```

**Ejemplos:**
```bsim
dice 6 sides;    // Dado estándar de 6 caras
dice 8 sides;    // Dado de 8 caras
dice 12 sides;   // Dado de 12 caras
```

### ⏱️ **5. Configurar la Simulación**

```bsim
simulate turnos turns {}
```

**Ejemplo:**
```bsim
simulate 50 turns {}  // Simular 50 turnos
```

---

## 🎮 Ejemplos Completos por Tipo de Juego

### 🏠 **Juego Económico (tipo Monopoly)**

```bsim
// === IMPERIO INMOBILIARIO ===
board ImperioInmobiliario loop 16;

// Casillas con propiedades y costos
cell 0 "Inicio" cost 0 rent 200;
cell 1 "Casa Modesta" cost 60 rent 30;
cell 2 "Apartamento" cost 100 rent 50;
cell 3 "Impuesto" cost 150 rent 0;
cell 4 "Villa" cost 200 rent 80;
cell 5 "Mansion" cost 300 rent 120;
cell 6 "Rascacielos" cost 500 rent 200;
cell 7 "Banco Central" cost 0 rent 300;

// 4 jugadores con economías diferentes
player 1 money 2000 position 0 strategy "aggressive";
player 2 money 2000 position 0 strategy "conservative";
player 3 money 1500 position 0 strategy "balanced";
player 4 money 1500 position 0 strategy "tactical";

// 2 dados para más variabilidad
dice 6 sides;
dice 6 sides;

// Simulación larga para desarrollo económico
simulate 40 turns {}
```

### ⚔️ **Juego de Estrategia (tipo Risk/TEG)**

```bsim
// === CONQUISTA MUNDIAL ===
board ConquistaMundial loop 25;

// Territorios estratégicos
cell 0 "Capital" cost 0 rent 100;
cell 1 "Frontera Norte" cost 50 rent 25;
cell 2 "Puerto Estratégico" cost 80 rent 40;
cell 3 "Montañas" cost 30 rent 15;
cell 4 "Desierto" cost 20 rent 10;
cell 5 "Ciudad Industrial" cost 120 rent 60;
cell 6 "Base Militar" cost 200 rent 100;

// Generales con diferentes estilos
player 1 money 1000 position 0 strategy "aggressive";
player 2 money 1000 position 6 strategy "tactical";
player 3 money 1000 position 12 strategy "conservative";
player 4 money 1000 position 18 strategy "balanced";

// Múltiples dados para batallas complejas
dice 6 sides;
dice 8 sides;
dice 6 sides;

simulate 30 turns {}
```

### 🏴‍☠️ **Juego de Aventuras**

```bsim
// === BUSQUEDA DEL TESORO PERDIDO ===
board Tesoro​Perdido loop 18;

// Lugares de aventura
cell 0 "Puerto Pirata" cost 0 rent 50;
cell 1 "Isla Misteriosa" cost 0 rent 100;
cell 2 "Cueva Peligrosa" cost 80 rent 0;
cell 3 "Templo Antiguo" cost 0 rent 150;
cell 4 "Trampa de Arena" cost 120 rent 0;
cell 5 "Cofre del Tesoro" cost 0 rent 300;
cell 6 "Kraken Gigante" cost 200 rent 0;
cell 7 "Isla del Tesoro" cost 0 rent 500;

// Aventureros valientes
player 1 money 800 position 0 strategy "aggressive";
player 2 money 800 position 0 strategy "balanced";
player 3 money 600 position 0 strategy "tactical";

// Dado único para suspense
dice 8 sides;

simulate 25 turns {}
```

### 🚀 **Juego Futurista (Completamente Original)**

```bsim
// === ODISEA GALACTICA ===
board OdiseaGalactica loop 22;

// Estaciones espaciales
cell 0 "Base Tierra" cost 0 rent 100;
cell 1 "Estacion Lunar" cost 50 rent 30;
cell 2 "Cinturon Asteroides" cost 100 rent 0;
cell 3 "Colonia Marte" cost 150 rent 75;
cell 4 "Agujero Negro" cost 300 rent 0;
cell 5 "Planeta Diamante" cost 0 rent 400;
cell 6 "Portal Dimensional" cost 500 rent 200;
cell 7 "Nexus Galactico" cost 0 rent 1000;

// Comandantes espaciales
player 1 money 2000 position 0 strategy "aggressive";
player 2 money 2000 position 0 strategy "conservative";
player 3 money 1500 position 0 strategy "tactical";

// Sistema de dados futurista
dice 10 sides;
dice 12 sides;

simulate 60 turns {}
```

---

## 📊 Interpretando los Resultados

### **Archivo de Salida (.txt)**

El sistema genera automáticamente un archivo con:

1. **📋 Estado Inicial**
   - Configuración del tablero
   - Dinero inicial de jugadores
   - Posiciones de inicio

2. **🎯 Log Turno por Turno**
   - Qué jugador juega
   - Resultado de los dados
   - Nueva posición
   - Dinero ganado/perdido
   - Eventos especiales

3. **🏆 Estado Final**
   - Dinero final de cada jugador
   - Posiciones finales
   - Estadísticas del juego

### **Ejemplo de Salida:**
```
=== Turn 15 ===
Player 2 rolls 7
Player 2 moves to position 12 (Villa)
Player 2 loses $200 (cost) but gains $80 (rent)
Player 2: Money=1340 -> 1220
```

---

## 🎯 Tips para Creadores de Juegos

### ⚖️ **Balanceando el Juego**

1. **Ratio Costo/Ganancia**: Las casillas muy costosas deberían dar más ganancia
2. **Dinero Inicial**: Debe permitir sobrevivir a varias casillas "malas"
3. **Número de Turnos**: Suficientes para ver evolución, no demasiados para aburrir

### 🎲 **Uso de Dados**

- **1 dado**: Movimiento predecible, más estratégico
- **2 dados**: Distribución normal, más balanceado  
- **3+ dados**: Más caótico, más suerte

### 👥 **Número de Jugadores**

- **2 jugadores**: Competencia directa (tipo Chess)
- **3 jugadores**: Dinámicas de alianzas
- **4+ jugadores**: Juegos de grupo complejos

### 🏗️ **Diseño de Tablero**

- **Tableros pequeños (10-15)**: Partidas rápidas
- **Tableros medianos (20-30)**: Partidas estándar
- **Tableros grandes (40+)**: Épicas, largas

---

## 🚀 Comandos Útiles

### **Compilar y Probar Rápido:**
```bash
./src/main/bash/build.sh && ./src/main/bash/run.sh mi-juego.bsim
```

### **Ver Solo el Resultado Final:**
```bash
tail -20 mi-juego.txt
```

### **Comparar Múltiples Ejecuciones:**
```bash
for i in {1..5}; do 
  ./src/main/bash/run.sh mi-juego.bsim
  echo "=== Ejecución $i ===" >> resultados.txt
  tail -10 mi-juego.txt >> resultados.txt
done
```

---

## 🏆 Juegos de Ejemplo Incluidos

El proyecto incluye **5 juegos completos** que demuestran todas las capacidades:

1. **`monopoly-ultimate.bsim`** - Economía compleja
2. **`teg-ultimate.bsim`** - Estrategia militar  
3. **`chess-ultimate.bsim`** - Juego de piezas
4. **`treasure-hunt-ultimate.bsim`** - Aventuras
5. **`galactic-conquest-ultimate.bsim`** - Sci-fi épico

**¡Úsalos como referencia y inspiración!**

---

## 🔧 Solución de Problemas

### **Error de Sintaxis**
- Verifica que todos los strings estén entre comillas `""`
- Revisa que los números no tengan espacios  
- Asegúrate de que cada línea termine en `;`

### **El Juego No Compila**
- Ejecuta `./src/main/bash/build.sh` primero
- Verifica que el archivo `.bsim` esté en el directorio raíz

### **Resultados Extraños**
- Revisa el balance entre `cost` y `rent`
- Ajusta el dinero inicial si los jugadores quiebran muy rápido
- Aumenta el número de turnos si el juego termina muy pronto

---

## 📞 ¿Necesitas Ayuda?

**BoardSim** está diseñado para ser **intuitivo y poderoso**. Si tienes dudas:

1. 📖 Revisa los ejemplos incluidos
2. 🧪 Experimenta con valores pequeños primero  
3. 📊 Analiza los logs para entender el comportamiento
4. 🎮 ¡Diviértete creando tu juego perfecto!

---

**¡Feliz creación de juegos! 🎲🎮**

*BoardSim - Donde las ideas se convierten en juegos*
