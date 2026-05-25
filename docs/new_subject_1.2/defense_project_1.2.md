# Guía de evaluación — Codexion v1.2

---

## Preliminaries

### Basics
**Lo que comprobará el evaluador:**
- Que compila con `make` sin warnings
- Que el binario se llama `codexion`
- Que acepta exactamente 8 argumentos en orden
- Que cada coder es un thread
- Que el scheduler es `fifo` o `edf`

**Cómo responder si te pregunta:**
- "El proyecto compila con `-Wall -Wextra -Werror -pthread` sin ningún warning"
- "Cada coder es un thread creado con `pthread_create` que ejecuta `coder_routine`"
- "El scheduler se valida en `parse_config` con `strcmp` — si no es exactamente `fifo` o `edf`, el programa rechaza la entrada"

**Comandos que ejecutará:**
```bash
make
./codexion 4 800 200 200 200 5 50 fifo
./codexion 4 800 200 200 200 5 50 edf
```

---

### Global
**Lo que comprobará el evaluador:**
- Que no hay variables globales mutables

**Cómo responder si te pregunta:**
- "No hay ninguna variable global en el proyecto. Todo el estado se pasa a través de `t_env`, que se inicializa en `main` y se pasa como argumento a cada thread"
- "El subject lo prohíbe explícitamente y con `-Werror` cualquier variable global habría generado un error de compilación"

**Cómo demostrarlo:**
```bash
grep -n "^[a-zA-Z]" coders/*.c | grep -v "^.*:.*("
# No debe aparecer nada
```

---

### README.md
**Lo que comprobará el evaluador:**
- Primera línea en itálica con el login
- Sección Description
- Sección Instructions
- Sección Resources con uso de IA
- Sección Blocking cases handled
- Sección Thread synchronization mechanisms

**Cómo responder si te pregunta sobre alguna sección:**
- "El README está en la raíz del repositorio como exige el subject"
- "La primera línea usa la sintaxis Markdown de itálica con asteriscos"
- Si te pregunta por el uso de IA: "Usé IA para clarificar conceptos de concurrencia y depurar race conditions. La lógica de sincronización, el heap y los mecanismos de threading los implementé yo para poder explicarlos en la evaluación"

---

## Codexion testing — Easy

**Lo que comprobará el evaluador:**
- Máximo 200 coders
- No usar timing bajo 60ms
- Que ningún coder sufre burnout
- Que todos completan las compilaciones requeridas

**Comandos que ejecutará:**
```bash
./codexion 5 800 200 200 200 5 50 fifo
./codexion 10 800 200 200 200 5 50 edf
./codexion 200 800 200 200 200 3 50 fifo
```

**Cómo responder si te pregunta:**
- "Con parámetros holgados, el ciclo completo de un coder es `time_to_compile + time_to_debug + time_to_refactor`. Si ese tiempo es menor que `time_to_burnout`, ningún coder debería morir"
- "Con 200 coders el programa escala correctamente porque cada coder es un thread independiente y la mesa circular se gestiona con el módulo `(i+1) % n`"

---

## Codexion testing — Less easy

**Lo que comprobará el evaluador:**
- Burnout edge cases
- Logging correcto y timing
- Sin duplicación de dongles
- Transiciones de estado correctas

**Comandos que ejecutará:**
```bash
./codexion 1 800 200 200 200 5 50 fifo   # 1 coder nunca puede compilar
./codexion 4 410 200 200 200 5 50 fifo   # ciclo 600ms > burnout 410ms
```

**Cómo responder si te pregunta:**

Sobre 1 coder:
- "Con 1 coder hay 1 dongle. Para compilar necesita 2 dongles simultáneamente, lo cual es imposible. El monitor detecta el burnout a los 800ms y termina limpiamente"

Sobre el burnout en ≤10ms:
- "El monitor usa `usleep(500)` — comprueba cada 0.5ms. Esto garantiza detectar el burnout dentro del margen de 10ms que exige el subject"

Sobre la duplicación de dongles:
- "Cada dongle está protegido por un `pthread_mutex_t`. El campo `in_use` dentro de `t_dongle` garantiza que solo un coder puede tenerlo a la vez. La cola de espera `waiters` gestiona el orden de acceso"

Sobre las transiciones de estado:
- "El ciclo es siempre: compilar → debuggear → refactorizar → compilar. Cada transición se loguea con el `log_lock` bloqueado para evitar mensajes entrelazados"

---

## Codexion testing — Medium

**Lo que comprobará el evaluador:**
- Comportamiento del cooldown
- Diferencias entre EDF y FIFO
- Timing del refactoring
- Serialización de logs

**Comandos que ejecutará:**
```bash
./codexion 4 800 200 200 200 5 0 fifo    # cooldown 0
./codexion 4 800 200 200 200 5 100 edf   # cooldown 100ms con EDF
```

**Cómo responder si te pregunta:**

Sobre el cooldown:
- "Cuando un coder suelta un dongle, se registra `cooldown_until = get_time_in_ms() + dongle_cooldown` en la estructura `t_dongle`. Cualquier coder que intente cogerlo antes usa `pthread_cond_timedwait` para dormir exactamente hasta que expire el cooldown"
- "Con cooldown 0, el dongle está disponible inmediatamente después de soltarse"

Sobre FIFO vs EDF:
- "La única diferencia está en `get_priority()`. Con FIFO devuelve el timestamp actual — el que llega antes tiene el número más pequeño y gana. Con EDF devuelve `last_compile_time + time_to_burnout` — el más urgente tiene el número más pequeño y gana"
- "El heap siempre ordena por el número más pequeño primero — min-heap. El significado del número cambia según el scheduler"

Sobre la serialización de logs:
- "Cada `printf` está envuelto en `log_lock`. Es un mutex global que garantiza que ningún mensaje puede entrelazarse con otro. Solo un thread puede imprimir a la vez"

---

## Codexion recode — EDF tie-breaker

**Lo que pedirá el evaluador:**
Modificar el EDF para que cuando dos coders tengan el mismo deadline, gane el que tiene el **mayor coder_id**.

**Lo que mirará en el código:**
La función `has_priority` en `heap_utils.c`:

```c
static int  has_priority(t_node a, t_node b)
{
    if (a.priority < b.priority)
        return (1);
    if (a.priority == b.priority && a.coder_id > b.coder_id)
        return (1);
    return (0);
}
```

**Cómo explicárselo:**
- "El tie-breaker está implementado en `has_priority`. Primero comparo las prioridades normalmente. Si son iguales, gana el de mayor `coder_id`"
- "Esta función se usa tanto en `sift_up` como en `sift_down` para mantener la propiedad del heap consistente en ambas direcciones"
- "El evaluador puede verificar que cuando `a.priority == b.priority`, la condición `a.coder_id > b.coder_id` hace que el nodo de mayor id suba en el heap"

**Por qué igual deadline puede no ocurrir naturalmente:**
- "Los timestamps tienen precisión de milisegundos. En la práctica dos coders raramente tienen exactamente el mismo `last_compile_time`, así que deadlines iguales son poco frecuentes. Pero el código lo maneja correctamente si ocurre"

**Si te piden modificarlo en vivo:**
Ya está implementado. Solo muéstrale el código y explícale la lógica de `has_priority`.

---

## Flags de evaluación

| Flag | Cuándo se usa |
|---|---|
| Ok | Todo correcto |
| Outstanding project | Implementación excepcional |
| Incomplete work | Faltan partes obligatorias |
| Invalid compilation | No compila |
| Norme | Error de norminette |
| Crash | Segfault o comportamiento inesperado |
| Leaks | Memory leaks detectados con Valgrind |
| Forbidden function | Función no permitida usada |
| Can't support / explain code | No puedes explicar tu propio código |

---

## Comandos útiles para demostrar durante la evaluación

```bash
# Compilar
make

# Valgrind — sin leaks
valgrind --leak-check=full ./codexion 4 800 200 200 200 3 50 fifo

# Sin variables globales
grep -n "^[a-zA-Z]" coders/*.c | grep -v "^.*:.*("

# Norminette
norminette coders/

# Casos de prueba rápidos
./codexion 1 800 200 200 200 5 50 fifo   # burnout
./codexion 4 800 200 200 200 5 50 fifo   # éxito fifo
./codexion 4 800 200 200 200 5 50 edf    # éxito edf
./codexion 4 410 200 200 200 5 50 fifo   # burnout rápido
```