# material_recognition_project
ldr & resistore 1k in pulldown:

nulla: 940
vetro: 905
plastica: 890
carta: 255

carta: analog <= 500
plastica: 500 < analog <= 890
vetro: 890 < analog < 940
nulla: analog > 940
