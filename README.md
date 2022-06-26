# 72.44 Criptografia y Seguridad 1er cuatrimestre 2022

## Trabajo practico de implementacion

### Instituto Tecnológico de Buenos Aires (ITBA)

## Autores (Grupo 7)

- [Agustin Tormakh](https://github.com/atormakh) - Legajo 60041
- [Salustiano Zavalia](https://github.com/szavalia) - Legajo 60312
- [Igal Leonel Revich](https://github.com/irevich) - Legajo 60390

## Indice

- [72.44 Criptografia y Seguridad 1er cuatrimestre 2022](#7244-criptografia-y-seguridad-1er-cuatrimestre-2022)
  - [Trabajo practico de implementacion](#trabajo-practico-de-implementacion)
    - [Instituto Tecnológico de Buenos Aires (ITBA)](#instituto-tecnológico-de-buenos-aires--itba-)
  - [Autores (Grupo 7)](#autores--grupo--7--)
  - [Indice](#indice)
  - [Descripcion del trabajo practico](#descripcion-del-trabajo-practico)
  - [Requerimientos previos](#requerimientos-previos)
  - [Compilacion](#compilacion)
  - [Ejecucion](#ejecucion)
    - [Embed](#embed)
    - [Extract](#extract)
  - [Consideraciones adicionales](#consideraciones-adicionales)
  - [Ejemplos de ejecucion](#ejemplos-de-ejecucion)
    - [Ejemplo 1 : Ejecucion con arquitecturas con 3 capas ocultas entre encoder y decoder](#ejemplo-1---ejecucion-con-arquitecturas-con-3-capas-ocultas-entre-encoder-y-decoder)
    - [Ejemplo 2 : Ejecucion con arquitecturas con 1 capa oculta entre encoder y decoder](#ejemplo-2---ejecucion-con-arquitecturas-con-1-capa-oculta-entre-encoder-y-decoder)

## Descripcion del trabajo practico

Este consiste en implementar diversos metodos de esteganografia para ocultar y extraer informacion en archivos portadores .BMP. A su vez, dicha informacion puede encriptarse a traves de los distintos algoritmos y modos vistos en clase. El archivo se oculta en el BMP en el formato de (tamaño||datos del archivo||extension), y en el caso de ocultarse encriptado, lo hace en el formato de (tamaño del cifrado||encripcion(tamaño||datos del archivo||extension)). Los metodos de esteganografia a implementar son:
- LSB1
- LSB4
- LSB Improved

Una vez hecho esto, se realiza un estegoanalisis a partir de archivos facilitados por la catedra.

## Requerimientos previos

Este se realizo en lenguaje C, con lo cual, se necesita previamente tener instalado lo siguiente:
- gcc
- openssl (Version 1.1.1 o mayor)

## Compilacion

Para compilar el trabajo practico, se dispone de un makefile. Para que este realice la compilacion, deben realizarse los siguientes comandos:

```
$ make clean

$ make all
```
Una vez realizado esto, se obtendra el ejecutable <i>stegobmp</i>, al cual se le deben pasar diversos parametros para su correcta ejecucion, explicados en la siguiente seccion.

## Ejecucion

Una vez obtenido el stegobmp mencionado anteriormente, este se ejecutara con distintos parametros, dependiendo si se esta ocultando (<i>embed</i>) o extrayendo (<i>extract</i>) informacion. A continuacion se explican los parametros correspondientes a cada caso

### Embed

Para ocultar un archivo con el stegobmp, debe ejecutarse de la siguiente forma:

```
$ ./stegobmp --embed -in PATH_ARCHIVO_A_OCULTAR -p PATH_BMP_PORTADOR --out PATH_BMP_SALIDA --steg METODO_ESTEGANOGRAFIA

```

En caso de querer ejecutarse con encripcion, debe hacerse asi:

```
$ ./stegobmp --embed -in PATH_ARCHIVO_A_OCULTAR -p PATH_BMP_PORTADOR --out PATH_BMP_SALIDA --steg METODO_ESTEGANOGRAFIA -a METODO_ENCRIPCION -m MODO_ENCRIPCION --pass PASSWORD_ENCRIPCION

```

Para dichos casos se cuenta con los siguientes parametros:
- Obligatorios
    - embed: Indica que se va a ocultar un archivo
    - in: Path del archivo a ocultar
    - p: Path del BMP donde se ocultara el archivo
    - out: Path del BMP con el archivo oculto
    - steg: Metodo de esteganografiado. Este puede tomar los siguientes valores:
        - LSB1
        - LSB4
        - LSBI
- Opcionales
    - a: Metodo de encripcion a utilizar. Este puede tomar los siguientes valores:
        - aes128
        - aes192
        - aes256
        - des (en caso de no especificarse y utilizar encripcion toma este por defecto)
    - m: Modo de encripcion a utilizar. Este puede tomar los siguientes valores:
        - ecb
        - cfb
        - ofb
        - cbc (en caso de no especificarse y utilizar encripcion toma este por defecto)
    - pass: Password que se utilizara para la encripcion (en caso de no especificarse, se hara el esteganografiado sin encripcion)

 
### Extract

Para extraer un archivo con el stegobmp, debe ejecutarse de la siguiente forma:

```
$ ./stegobmp --extract -p PATH_BMP_PORTADOR --out PATH_ARCHIVO_SALIDA_SIN_EXTENSION --steg METODO_ESTEGANOGRAFIA

```

En caso de querer ejecutarse con encripcion, debe hacerse asi:

```
$ ./stegobmp --extract -p PATH_BMP_PORTADOR --out PATH_ARCHIVO_SALIDA_SIN_EXTENSION --steg METODO_ESTEGANOGRAFIA -a METODO_ENCRIPCION -m MODO_ENCRIPCION --pass PASSWORD_ENCRIPCION

```

Para dichos casos se cuenta con los siguientes parametros:
- Obligatorios
    - extract: Indica que se va a ocultar un archivo
    - p: Path del BMP con el archivo oculto
    - out: Path del archivo a extraer (sin extension)
    - steg: Metodo de esteganografiado. Este puede tomar los siguientes valores:
        - LSB1
        - LSB4
        - LSBI
- Opcionales
    - a: Metodo de encripcion a utilizar. Este puede tomar los siguientes valores:
        - aes128
        - aes192
        - aes256
        - des (en caso de no especificarse y utilizar encripcion toma este por defecto)
    - m: Modo de encripcion a utilizar. Este puede tomar los siguientes valores:
        - ecb
        - cfb
        - ofb
        - cbc (en caso de no especificarse y utilizar encripcion toma este por defecto)
    - pass: Password que se utilizara para la encripcion (en caso de no especificarse, se hara el esteganografiado sin encripcion)

## Consideraciones adicionales

A la hora de utilizar BMPs para la ejecucion del stegobmp, tener en cuenta lo siguiente:
- El BMP debe ser version 3 (header de 54 bytes y 3 bytes por pixel)
- El BMP no debe estar comprimido (los bytes de compresion pertenecientes l header deben estar en 0)

## Ejemplos de ejecucion

### Ejemplo 1 : Ocultar un PNG con LSB1

```
$ ./stegobmp --embed --in resources/respuesta.png -p resources/lado.bmp --out ladoModificadoLSB1.bmp --steg LSB1

```

### Ejemplo 2 : Extraer un archivo con LSB Improved, encriptado con AES256, modo OFB y password "secreto"

```
$ ./stegobmp --extract -p resources/ladoLSBIaes256ofb.bmp --out archivoOcultoLSBIEncriptado --steg LSBI -a aes256 -m ofb --pass "secreto"

```