#!/bin/bash
echo "PRUEBAS PRUEBAS Y MAS PRUEBAS"
echo "Cleaning environment"
make clean
echo "Build Project"
make all
compare_files(){
    cmp $1 $2 2> compare_logs.txt
    status=$?
    if [[ $status = 0 ]]; then
        echo -e "\e[1;32m ✓ Files are equal"
    else
        echo -e "\e[1;31m ✗ Files are different"
    fi
}
print_test_name(){
    echo -e "\e[1;33m$1"
}
clean_files(){
    rm -f ladoModificado.bmp
    rm -f ladoExtraido.png
}
for STEGO in LSB1 LSB4 LSBI
do
    print_test_name "Test file+secret --> $STEGO embed --> $STEGO extract --> file + secret"
    ./stegobmp --embed --in resources/respuesta.png -p resources/lado.bmp --out ladoModificado.bmp --steg $STEGO 2> errors.log
    ./stegobmp --extract -p ladoModificado.bmp --out ladoExtraido --steg $STEGO 2> errors.log
    compare_files resources/respuesta.png ladoExtraido.png
    clean_files
    for ALGORITHM in aes128 aes192 aes256 des
    do
        for MODE in ecb efb ofb cbc
        do
            print_test_name "   Add with encryption $ALGORITHM $MODE"
            ./stegobmp --embed --in resources/respuesta.png -p resources/lado.bmp --out ladoModificado.bmp --steg $STEGO -a $ALGORITHM -m $MODE --pass "postergation" 2> errors.log
            status=$?
            if [[ $status != 0 ]]; then
                echo -e "\e[1;31m   ✗ Error running embed script"
                cat errors.log
            fi
            ./stegobmp --extract -p ladoModificado.bmp --out ladoExtraido --steg $STEGO -a $ALGORITHM -m $MODE --pass "postergation" 2> errors.log
            status=$?
            if [[ $status != 0 ]]; then
                echo -e "\e[1;31m   ✗ Error running extract script"
                cat errors.log
            fi
            compare_files resources/respuesta.png ladoExtraido.png
            clean_files
        done
    done
done

print_test_name "Extract png from ladoLSB1aes192cbc"
./stegobmp --extract -p resources/ladoLSB1aes192cbc.bmp --out ladoExtraido --steg LSB1 -a aes192 -m cbc --pass "escondite" 2> errors.log
status=$?
if [[ $status != 0 ]]; then
    echo -e "\e[1;31m   ✗ Error running extract script"
    cat errors.log
fi
compare_files resources/respuesta.png ladoExtraido.png
clean_files

print_test_name "Create ladoLSB1aes192cbc with lado.bmp + respuesta.png"
./stegobmp --embed --in resources/respuesta.png -p resources/lado.bmp --out ladoModificado.bmp --steg LSB1 -a aes192 -m cbc --pass "escondite" 2> errors.log
status=$?
if [[ $status != 0 ]]; then
    echo -e "\e[1;31m   ✗ Error running extract script"
    cat errors.log
fi
compare_files resources/ladoLSB1aes192cbc.bmp ladoModificado.bmp
clean_files

print_test_name "Extract png from ladoLSBIaes256ofb"
./stegobmp --extract -p resources/ladoLSBIaes256ofb.bmp --out ladoExtraido --steg LSBI -a aes256 -m ofb --pass "secreto" 2> errors.log
status=$?
if [[ $status != 0 ]]; then
    echo -e "\e[1;31m   ✗ Error running extract script"
    cat errors.log
fi
compare_files resources/respuesta.png ladoExtraido.png
clean_files

print_test_name "Create ladoLSBIaes256ofb with lado.bmp + respuesta.png"
./stegobmp --embed --in resources/respuesta.png -p resources/lado.bmp --out ladoModificado.bmp --steg LSBI -a aes256 -m ofb --pass "secreto" 2> errors.log
status=$?
if [[ $status != 0 ]]; then
    echo -e "\e[1;31m   ✗ Error running extract script"
    cat errors.log
fi
compare_files resources/ladoLSBIaes256ofb.bmp ladoModificado.bmp
clean_files

print_test_name "Extract png from ladoLSB1.bmp"
./stegobmp --extract -p resources/ladoLSB1.bmp --out ladoExtraido --steg LSB1 2> errors.log
status=$?
if [[ $status != 0 ]]; then
    echo -e "\e[1;31m   ✗ Error running extract script"
    cat errors.log
fi
compare_files resources/respuesta.png ladoExtraido.png
clean_files

print_test_name "Create ladoLSB1 with lado.bmp + respuesta.png"
./stegobmp --embed --in resources/respuesta.png -p resources/lado.bmp --out ladoModificado.bmp --steg LSB1 2> errors.log
status=$?
if [[ $status != 0 ]]; then
    echo -e "\e[1;31m   ✗ Error running extract script"
    cat errors.log
fi
compare_files resources/ladoLSB1.bmp ladoModificado.bmp
clean_files

print_test_name "Extract png from ladoLSB4.bmp"
./stegobmp --extract -p resources/ladoLSB4.bmp --out ladoExtraido --steg LSB4 2> errors.log
status=$?
if [[ $status != 0 ]]; then
    echo -e "\e[1;31m   ✗ Error running extract script"
    cat errors.log
fi
compare_files resources/respuesta.png ladoExtraido.png
clean_files

print_test_name "Create ladoLSB4 with lado.bmp + respuesta.png"
./stegobmp --embed --in resources/respuesta.png -p resources/lado.bmp --out ladoModificado.bmp --steg LSB4 2> errors.log
status=$?
if [[ $status != 0 ]]; then
    echo -e "\e[1;31m   ✗ Error running extract script"
    cat errors.log
fi
compare_files resources/ladoLSB4.bmp ladoModificado.bmp
clean_files

print_test_name "Extract png from ladoLSBI.bmp"
./stegobmp --extract -p resources/ladoLSB4.bmp --out ladoExtraido --steg LSB4 2> errors.log
status=$?
if [[ $status != 0 ]]; then
    echo -e "\e[1;31m   ✗ Error running extract script"
    cat errors.log
fi
compare_files resources/respuesta.png ladoExtraido.png
clean_files

print_test_name "Create ladoLSBI with lado.bmp + respuesta.png"
./stegobmp --embed --in resources/respuesta.png -p resources/lado.bmp --out ladoModificado.bmp --steg LSBI 2> errors.log
status=$?
if [[ $status != 0 ]]; then
    echo -e "\e[1;31m   ✗ Error running extract script"
    cat errors.log
fi
compare_files resources/ladoLSBI.bmp ladoModificado.bmp
clean_files