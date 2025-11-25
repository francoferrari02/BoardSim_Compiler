# BoardSim DSL - Guía de Sintaxis

## 🎯 **Abstracción de Recursos**

BoardSim utiliza una sintaxis **abstracta** que se reinterpreta según el contexto del juego:

### 📊 **Campo `money` - Recurso Universal**

```bsim
player 1 money <valor> position <pos>;
```

El campo `money` es un **recurso genérico** que se interpreta de manera inteligente:

| Juego | Sintaxis | Interpretación Semántica |
|-------|----------|-------------------------|
| **Monopoly** | `money 1500` | 💰 $1500 (dinero real) |
| **TEG** | `money 15` | ⚔️ 15 ejércitos |
| **Chess** | `money 16` | ♛ 16 piezas |

### 🎮 **Ejemplos Prácticos**

**🏠 Monopoly** (Económico):
```bsim
board Monopoly loop 40;
player 1 money 1500 position 0;  // $1500, empieza en GO
```

**⚔️ TEG** (Militar):
```bsim
board TEG_Argentina loop 20;
player 1 money 15 position 0;  // 15 ejércitos, empieza en Buenos_Aires
```

**♛ Chess** (Estratégico):
```bsim
board Chess_Board loop 64;
player 1 money 16 position 0;  // 16 piezas, empieza en a1
```

## 🧠 **Detección Automática**

El compilador **detecta automáticamente** el tipo de juego:
- **4 jugadores + 3 dados** = TEG (militar)
- **4 jugadores + 2 dados** = Monopoly (económico)  
- **2 jugadores + 1 dado** = Chess (estratégico)

## 📈 **Ventajas de la Abstracción**

1. **Sintaxis unificada**: Un solo campo para todos los recursos
2. **Semántica inteligente**: Interpretación contextual automática
3. **Flexibilidad**: Fácil extensión a nuevos tipos de juegos
4. **Simplicidad**: Sin necesidad de keywords específicos por juego

## 🚀 **Expansión Futura**

En versiones futuras, se podría agregar sintaxis específica:
```bsim
// Sintaxis específica (futuro)
player 1 armies 15 position 0;   // TEG
player 1 pieces 16 position 0;   // Chess
player 1 money 1500 position 0;  // Monopoly
```

Pero por ahora, la **abstracción universal** con `money` es elegante y funcional.
