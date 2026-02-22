# material_recognition_project
(ldr & resistore 1k in pulldown)

test:

induttivo:
carta & plastica & vetro: analog = 1
metallo: analog = 0


laser:
carta: analog <= 50
plastica: 80 < analog <= 210
vetro: 210 < analog < 850
nulla: analog > 850


infrarosso:
carta: 0 < analog < 40
plastica: 40 <= analog < 200
vetro & nulla: analog > 500



decisioni in ordine:

se ottico > 850 -> non c'è nulla

se induttivo è falso -> è metallo

se induttivo è vero:

se ottico <= 100 & infrarosso < 60 -> è carta

se infrarosso in [40, 200] ***& ottico in (40, 210]*** -> è plastica

se ottico in (100, 850) & infrarosso > 500 -> è vetro