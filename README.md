# Augmented Reality

Programa de visión artificial desarrollado en C++ con OpenCV para superponer elementos virtuales sobre un tablero de ajedrez utilizando una cámara o un vídeo.

## Funcionamiento

El programa utiliza los parámetros de una cámara previamente calibrada y realiza los siguientes pasos:

1. Captura un fotograma de la cámara o del vídeo de entrada.
2. Detecta las esquinas interiores del tablero.
3. Calcula la posición y orientación de la cámara respecto al tablero.
4. Proyecta los elementos virtuales sobre la escena.
5. Muestra el resultado en una ventana.

Permite visualizar ejes de coordenadas, un modelo 3D animado y una imagen o un vídeo adaptados a la perspectiva del tablero.

## Requisitos

- Compilador compatible con C++11.
- CMake 3.10 o posterior.
- OpenCV 4.6 o posterior.
- Archivo XML o YAML con los parámetros de calibración.
- Un tablero de ajedrez visible en la cámara o en el vídeo de entrada.

La calibración debe corresponder a la cámara y a la resolución utilizadas para capturar la escena.

## Compilación

Desde la raíz del proyecto:

```bash
cmake -S . -B build
cmake --build build
```

Los siguientes ejemplos se ejecutan desde el directorio `build/`:

```bash
cd build
```

## Preparación de los datos

Coloca en `data/` los archivos necesarios:

- `calibracion.yml`, generado previamente con la práctica de calibración.
- `tablero.mp4`, un vídeo donde aparezca el tablero.
- `imagen.png`, si quieres proyectar una imagen.
- `contenido.mp4`, si quieres proyectar un vídeo.

Los nombres son ejemplos y pueden sustituirse por los de tus archivos.

En los comandos siguientes se utiliza un tablero de 5 filas y 6 columnas de casillas, con un lado de 3 cm por casilla:

- `--rows=5` indica las filas de casillas.
- `--cols=6` indica las columnas de casillas.
- `--size=0.03` indica el lado de cada casilla en metros.

Este tablero contiene 4 × 5 esquinas interiores. Ajusta los valores a las dimensiones de tu tablero y utiliza las mismas unidades que en la calibración.

## Visualización de ejes sobre un vídeo

```bash
./aug_real --rows=5 --cols=6 --size=0.03 \
  ../data/calibracion.yml ../data/tablero.mp4
```

Por defecto, el programa dibuja los ejes de coordenadas cuando detecta el tablero.

## Uso con una cámara

```bash
./aug_real --is_camera --rows=5 --cols=6 --size=0.03 \
  ../data/calibracion.yml 0
```

El último argumento indica el índice de la cámara. El valor `0` corresponde a la primera cámara.

## Modelo 3D animado

Añade `--draw_model` para visualizar un modelo de una casa dibujada mediante aristas, con movimiento horizontal:

```bash
./aug_real --draw_model --rows=5 --cols=6 --size=0.03 \
  ../data/calibracion.yml ../data/tablero.mp4
```

## Proyección de una imagen

La opción `--render_image` permite superponer una imagen sobre la región delimitada por las esquinas interiores extremas del tablero:

```bash
./aug_real --render_image=../data/imagen.png \
  --rows=5 --cols=6 --size=0.03 \
  ../data/calibracion.yml ../data/tablero.mp4
```

La imagen se transforma para ajustarse a la perspectiva del tablero. Los ejes se siguen mostrando por defecto.

## Proyección de un vídeo

La opción `--render_video` permite utilizar los fotogramas de otro vídeo como contenido proyectado:

```bash
./aug_real --render_video=../data/contenido.mp4 \
  --rows=5 --cols=6 --size=0.03 \
  ../data/calibracion.yml ../data/tablero.mp4
```

El vídeo de entrada muestra la escena con el tablero. El archivo indicado en `--render_video` aporta el contenido que se superpone.

Los fotogramas del contenido avanzan cuando se detecta el tablero.

## Controles

Pulsa **Esc** para cerrar la visualización y terminar el programa.

Los elementos virtuales se dibujan únicamente cuando se detecta el tablero.

## Ayuda

```bash
./aug_real --help
```





