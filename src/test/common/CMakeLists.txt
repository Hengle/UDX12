Ubpa_GetTargetName(core ${PROJECT_SOURCE_DIR}/src/core)
Ubpa_AddTarget(
  TEST
  MODE STATIC
  LIB ${core}
  L_OPTION_INTERFACE
    /SUBSYSTEM:WINDOWS
)
