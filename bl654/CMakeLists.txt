if(CONFIG_BL654_SB_LEAVE_SPACE)
  target_sources(app PRIVATE src/magic.c)
  zephyr_linker_sources(ROM_START bl654.ld)
endif()
