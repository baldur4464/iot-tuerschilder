idf_component_register(SRCS "src/tuerschild_app.c"
					"src/tuerschild_global.c"
					"src/tuerschild_esp32.c"
					"src/tuerschild_wifi_esp32.c"
					"src/tuerschild_storage_esp32.c"
					"src/tuerschild_mqtt_esp32.c"
					"src/tuerschild_httpd_esp32.c"
					"src/tuerschild_conf.c"
					"src/tuerschild_eink_esp32.cpp"
					"src/esp32_eink_driver/font8.cpp"
					"src/esp32_eink_driver/font12.cpp"
					"src/esp32_eink_driver/font12CN.c"
					"src/esp32_eink_driver/font16.cpp"
					"src/esp32_eink_driver/font20.cpp"
					"src/esp32_eink_driver/font24.cpp"
					"src/esp32_eink_driver/font24CN.c"
					"src/esp32_eink_driver/GUI_Paint.cpp"
					"src/esp32_eink_driver/utility/EPD_7in5_V2.cpp"
					"src/esp32_eink_driver/utility/DEV_Config.cpp"
					"src/tuerschild_btn_esp32.c"
                    INCLUDE_DIRS "./inc" "." "./src/esp32_eink_driver")


target_compile_options( ${COMPONENT_LIB} PRIVATE -Wno-error=missing-braces)