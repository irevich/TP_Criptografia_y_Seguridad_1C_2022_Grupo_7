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
    ./stegobmp.out --embed --in resources/respuesta.png -p resources/lado.bmp --out ladoModificado.bmp --steg $STEGO 2> errors.log
    ./stegobmp.out --extract -p ladoModificado.bmp --out ladoExtraido --steg $STEGO 2> errors.log
    compare_files resources/respuesta.png ladoExtraido.png
    clean_files
    for ALGORITHM in aes128 #aes192 aes256 des
    do
        for MODE in ecb
        #for MODE in ecb #efb ofb cbc
        do
            print_test_name "   Add with encryption $ALGORITHM $MODE"
            ./stegobmp.out --embed --in resources/respuesta.png -p resources/lado.bmp --out ladoModificado.bmp --steg $STEGO -a $ALGORITHM -m $MODE --pass "postergation" 2> errors.log
            status=$?
            if [[ $status != 0 ]]; then
                echo -e "\e[1;31m   ✗ Error running embed script"
                cat errors.log
            fi
            ./stegobmp.out --extract -p ladoModificado.bmp --out ladoExtraido --steg $STEGO -a $ALGORITHM -m $MODE --pass "postergation" 2> errors.log
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

