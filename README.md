# material_recognition_project
in ordine di analisi:

induttivo:
carta & plastica & vetro: analog = 1
metallo: analog = 0


laser:
carta: analog <= 50
plastica: 80 < analog <= 190
vetro: 230 < analog < 800
nulla: analog > 900


infrarosso:
carta: 0 < analog < 40
plastica: 40 <= analog < 200
vetro & nulla: analog > 650



(ldr & resistore 1k in pulldown)
