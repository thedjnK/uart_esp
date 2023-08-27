#cmake -GNinja -DBOARD=bl654_sensor_board ../../../bootloader/mcuboot/boot/zephyr/ -DAPPLICATION_CONFIG_DIR=../../../../uart_esp/bootloader/config/bl654_sensor_board/
cmake -GNinja -DBOARD=bl654_sensor_board ../../../bootloader/mcuboot/boot/zephyr/ -DAPPLICATION_CONFIG_DIR=${PWD}/../config/bl654_sensor_board/
