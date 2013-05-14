
    .sect ".onchip_code"
    .def _hwafft_wrapper

; HWAFFT code burned in C5515 ROM seems to support small memory model only.
; The following wrapper helps to use HWAFFT for large/huge memory models.
; CVN 20110920
_hwafft_wrapper:
    XAR2 = XAR1
    XAR3 = XAR0
    call AC0
    return

    .end
