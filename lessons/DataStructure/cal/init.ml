(multi (let i 0) (for (not (equ i 10)) (multi (if (equ (mod i 2) 1) (multi (print i) (br)) (br)) (let i (add i 1)))))
