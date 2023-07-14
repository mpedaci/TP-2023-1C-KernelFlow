#!/bin/bash

echo "Seleccione el tipo de prueba a realizar:"
echo "1. Base"
echo "2. Deadlock"
echo "3. Memoria"
echo "4. File System"
echo "5. Errores"

read -p "Ingrese el numero de la prueba: " prueba

case $prueba in
    1)
        echo "Prueba Base"
        cp "./tests/Configs/kernel/kernel.t1.config" "./kernel/config/kernel.config"
        cp "./tests/Configs/memoria/memoria.t1.config" "./memoria/config/memoria.config"
        cp "./tests/Configs/cpu/cpu.t1.config" "./cpu/config/cpu.config"
        ;;
    2)
        echo "Prueba Deadlock"
        cp "./tests/Configs/kernel/kernel.t2.config" "./kernel/config/kernel.config"
        cp "./tests/Configs/memoria/memoria.t1.config" "./memoria/config/memoria.config"
        cp "./tests/Configs/cpu/cpu.t1.config" "./cpu/config/cpu.config"
        ;;
    3)
        echo "Prueba Memoria"
        cp "./tests/Configs/kernel/kernel.t3.config" "./kernel/config/kernel.config"
        cp "./tests/Configs/memoria/memoria.t2.config" "./memoria/config/memoria.config"
        cp "./tests/Configs/cpu/cpu.t2.config" "./cpu/config/cpu.config"
        ;;
    4)
        echo "Prueba File System"
        cp "./tests/Configs/kernel/kernel.t4.config" "./kernel/config/kernel.config"
        cp "./tests/Configs/memoria/memoria.t3.config" "./memoria/config/memoria.config"
        cp "./tests/Configs/cpu/cpu.t2.config" "./cpu/config/cpu.config"
        ;;
    5)
        echo "Prueba Errores"
        cp "./tests/Configs/kernel/kernel.t1.config" "./kernel/config/kernel.config"
        cp "./tests/Configs/memoria/memoria.t2.config" "./memoria/config/memoria.config"
        cp "./tests/Configs/cpu/cpu.t1.config" "./cpu/config/cpu.config"
        ;;
    *)
        echo "Opcion incorrecta"
        ;;
esac

exit 0