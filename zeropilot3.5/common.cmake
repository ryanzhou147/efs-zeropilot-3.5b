# attitude manager files
set(AM_SRC
    "src/attitude_manager/attitude_manager.cpp"
    "src/attitude_manager/direct_mapping.cpp"
)
set(AM_INC
    "include/attitude_manager/"
)

# system manager files
set(SM_SRC
    "src/system_manager/system_manager.cpp"
)
set(SM_INC
    "include/system_manager/"
)

# telemetry manager files
set(TM_SRC

)
set(TM_INC
    "include/telemetry_manager"
)
set(COMMON_SRC

)
set(COMMON_INC
    "include/common"
)
# combined files
set(ZP_SRC
    ${AM_SRC}
    ${SM_SRC}
    ${TM_SRC}
    ${COMMON_SRC}
)
set(ZP_INC
    "include/driver_ifaces/"
    "include/thread_msgs/"
    ${AM_INC}
    ${SM_INC}
    ${TM_INC}
    ${COMMON_INC}
)