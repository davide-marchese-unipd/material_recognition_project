# material_recognition_project
(ldr & resistore 1k in pulldown)

decisioni in ordine:

se ottico > 850 -> non c'è nulla

se induttivo è falso -> è metallo

se induttivo è vero:

se ottico <= 100 & infrarosso < 60 -> è carta

se infrarosso in [40, 400] -> è plastica

se ottico > 100 -> è vetro
