add_test (NAME KWinEffectSupport COMMAND sh ${CMAKE_CURRENT_SOURCE_DIR}/tools/isSupported.sh)
set_property (TEST KWinEffectSupport PROPERTY PASS_REGULAR_EXPRESSION "true")
