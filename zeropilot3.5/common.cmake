# attitude manager files
set(AM_SRC

)
set(AM_INC

)

# system manager files
set(SM_SRC

)
set(SM_INC

)

# combined files
set(ZP_SRC
    ${AM_SRC}
    ${SM_SRC}
)
set(ZP_INC
    "include/driver_ifaces"
    ${AM_INC}
    ${SM_INC}
)