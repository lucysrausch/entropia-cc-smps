v 20130925 2
C 40000 40000 0 0 0 title-B.sym
T 50700 41100 9 10 1 0 0 0 1
Buck-Boost Full H-Bridge with PMOS
T 54000 40100 9 10 1 0 0 0 1
Ken Sarkies
T 54000 40400 9 10 1 0 0 0 1
9/11/2015
T 50600 40100 9 10 1 0 0 0 1
2
T 52100 40100 9 10 1 0 0 0 1
1
C 42600 47700 1 0 0 npn-2.sym
{
T 43200 48200 5 10 0 0 0 0 1
device=NPN_TRANSISTOR
T 43200 48300 5 10 1 1 0 0 1
refdes=Q3
T 43100 48100 5 10 1 1 0 0 1
value=2N3904
T 42600 47700 5 10 0 0 0 0 1
file=/home/ksarkies/Development-Electronics/Spice-models/models.lib
}
C 42600 47700 1 180 1 pnp-2.sym
{
T 43200 47300 5 10 0 0 180 6 1
device=PNP_TRANSISTOR
T 43200 47400 5 10 1 1 180 6 1
refdes=Q4
T 43100 47000 5 10 1 1 0 0 1
value=Q2N3906
T 42600 47700 5 10 0 0 0 0 1
file=/home/ksarkies/Development-Electronics/Spice-models/models.lib
}
C 54100 45200 1 270 0 capacitor-4.sym
{
T 55200 45000 5 10 0 0 270 0 1
device=POLARIZED_CAPACITOR
T 53700 45000 5 10 1 1 0 0 1
refdes=C4
T 54800 45000 5 10 0 0 270 0 1
symversion=0.1
T 53600 44800 5 10 1 1 0 0 1
value=47uF
}
C 45500 46000 1 0 0 coil-2.sym
{
T 45700 46500 5 10 0 0 0 0 1
device=COIL
T 45900 46700 5 10 1 1 0 0 1
refdes=L1
T 45700 46700 5 10 0 0 0 0 1
symversion=0.1
T 45700 46400 5 10 1 1 0 0 1
value=800uH
}
N 44600 46100 45500 46100 4
N 44600 44700 44600 47100 4
N 43100 47700 44100 47700 4
N 42600 48200 42400 48200 4
N 42400 48200 42400 47200 4
N 42400 47200 42600 47200 4
N 44600 47900 44600 50300 4
N 44600 42400 44600 43900 4
N 54300 42400 54300 44300 4
N 41500 49300 43900 49300 4
N 44600 42400 55800 42400 4
N 43900 46700 43900 48000 4
C 43700 48900 1 270 0 capacitor-1.sym
{
T 44400 49300 5 10 0 0 270 0 1
device=CAPACITOR
T 44600 49300 5 10 0 0 270 0 1
symversion=0.1
T 43700 49500 5 10 0 0 180 0 1
footprint=CK05_type_Capacitor
T 44100 48900 5 10 1 1 0 0 1
refdes=C1
T 44000 48600 5 10 1 1 0 0 1
value=0.1uF
}
C 41600 47700 1 90 0 resistor-1.sym
{
T 41200 48000 5 10 0 0 90 0 1
device=RESISTOR
T 41300 48500 5 10 1 1 180 0 1
refdes=R1
T 41300 48200 5 10 1 1 180 0 1
value=1K
T 41600 47700 5 10 0 0 0 0 1
footprint=R025
}
N 41500 48600 41500 49300 4
N 54300 45200 54300 46100 4
N 43100 48700 43100 49300 4
C 41000 46700 1 0 0 nmos-3.sym
{
T 41600 47200 5 10 0 0 0 0 1
device=NMOS_TRANSISTOR
T 41700 46900 5 10 1 1 0 0 1
refdes=Q1
T 41700 46700 5 10 1 1 0 0 1
value=2N7000
T 41000 46700 5 10 0 0 0 0 1
file=/home/ksarkies/Development-Electronics/Spice-models/models.lib
}
C 48300 43900 1 0 1 nmos-3.sym
{
T 47700 44400 5 10 0 0 0 6 1
device=NMOS_TRANSISTOR
T 47600 44400 5 10 1 1 0 6 1
refdes=Q9
T 46900 43800 5 10 1 1 0 0 1
value=IRF1405
}
N 49400 44100 48300 44100 4
N 47800 42400 47800 43900 4
N 49900 44600 50100 44600 4
N 50100 43600 49900 43600 4
N 50100 43600 50100 44600 4
N 49400 45100 49400 45400 4
C 49900 44100 1 0 1 npn-2.sym
{
T 49300 44600 5 10 0 0 0 6 1
device=NPN_TRANSISTOR
T 49300 44600 5 10 1 1 0 6 1
refdes=Q10
T 49300 44400 5 10 1 1 0 6 1
value=2N3904
}
C 49900 44100 1 180 0 pnp-2.sym
{
T 49300 43700 5 10 0 0 180 0 1
device=PNP_TRANSISTOR
T 49300 43800 5 10 1 1 180 0 1
refdes=Q11
T 49300 43400 5 10 1 1 0 6 1
value=2N3906
}
N 47800 44700 47800 50300 4
N 46500 46100 47800 46100 4
C 45100 49300 1 270 0 capacitor-4.sym
{
T 46200 49100 5 10 0 0 270 0 1
device=POLARIZED_CAPACITOR
T 45700 48900 5 10 1 1 0 0 1
refdes=C2
T 45800 49100 5 10 0 0 270 0 1
symversion=0.1
T 45700 48700 5 10 1 1 0 0 1
value=220uF
}
C 44500 42100 1 0 0 gnd-2.sym
N 43900 48900 43900 49300 4
C 45800 43800 1 90 0 diode-2.sym
{
T 45200 44200 5 10 0 0 90 0 1
device=DIODE
T 46200 44500 5 10 1 1 180 0 1
refdes=D1
T 45900 44100 5 10 1 1 0 0 1
value=1N5819
T 45800 43800 5 10 0 0 0 0 1
file=/home/ksarkies/Development-Electronics/Spice-models/models.lib
}
C 44100 43900 1 0 0 nmos-3.sym
{
T 44700 44400 5 10 0 0 0 0 1
device=NMOS_TRANSISTOR
T 44900 45100 5 10 1 1 0 0 1
refdes=Q8
T 44800 44900 5 10 1 1 0 0 1
value=IRF1405
}
N 43100 44100 44100 44100 4
N 42600 44600 42400 44600 4
N 42400 43600 42600 43600 4
N 42400 43600 42400 44600 4
N 43100 45100 43100 45400 4
C 42600 44100 1 0 0 npn-2.sym
{
T 43200 44600 5 10 0 0 0 0 1
device=NPN_TRANSISTOR
T 43200 44600 5 10 1 1 0 0 1
refdes=Q5
T 43200 44400 5 10 1 1 0 0 1
value=2N3904
}
C 42600 44100 1 180 1 pnp-2.sym
{
T 43200 43700 5 10 0 0 180 6 1
device=PNP_TRANSISTOR
T 43200 43800 5 10 1 1 180 6 1
refdes=Q6
T 43200 43400 5 10 1 1 0 0 1
value=2N3906
}
N 45500 44700 44600 44700 4
N 45500 43800 44600 43800 4
C 44100 47900 1 180 1 pmos-3.sym
{
T 44700 47400 5 10 0 0 180 6 1
device=PMOS_TRANSISTOR
T 44800 47700 5 10 1 1 180 6 1
refdes=Q7
T 44700 47300 5 10 1 1 0 0 1
value=IRF5305
}
C 42900 45400 1 0 0 vcc-1.sym
C 53300 47700 1 0 1 npn-2.sym
{
T 52700 48200 5 10 0 0 0 6 1
device=NPN_TRANSISTOR
T 52700 48300 5 10 1 1 0 6 1
refdes=Q14
T 52800 48100 5 10 1 1 0 6 1
value=2N3904
T 53300 47700 5 10 0 0 0 6 1
file=/home/ksarkies/Development-Electronics/Spice-models/models.lib
}
C 53300 47700 1 180 0 pnp-2.sym
{
T 52700 47300 5 10 0 0 180 0 1
device=PNP_TRANSISTOR
T 52700 47400 5 10 1 1 180 0 1
refdes=Q15
T 52800 47000 5 10 1 1 0 6 1
value=Q2N3906
T 53300 47700 5 10 0 0 0 6 1
file=/home/ksarkies/Development-Electronics/Spice-models/models.lib
}
N 52800 47700 51300 47700 4
N 53300 48200 53500 48200 4
N 53500 48200 53500 47200 4
N 53500 47200 53300 47200 4
C 54300 47700 1 270 1 resistor-1.sym
{
T 54700 48000 5 10 0 0 90 2 1
device=RESISTOR
T 54600 48500 5 10 1 1 180 6 1
refdes=R4
T 54600 48200 5 10 1 1 180 6 1
value=1K
T 54300 47700 5 10 0 0 0 6 1
footprint=R025
}
N 54400 48600 54400 49300 4
N 52800 48700 52800 49300 4
C 54900 46700 1 0 1 nmos-3.sym
{
T 54300 47200 5 10 0 0 0 6 1
device=NMOS_TRANSISTOR
T 54200 46900 5 10 1 1 0 6 1
refdes=Q16
T 54200 46700 5 10 1 1 0 6 1
value=2N7000
T 54900 46700 5 10 0 0 0 6 1
file=/home/ksarkies/Development-Electronics/Spice-models/models.lib
}
C 49700 47000 1 270 1 diode-2.sym
{
T 50300 47400 5 10 0 0 270 6 1
device=DIODE
T 50200 48300 5 10 1 1 180 2 1
refdes=D2
T 49800 48200 5 10 1 1 0 2 1
value=1N5819
}
N 50800 47000 50000 47000 4
N 50800 47900 50800 50300 4
N 50800 47900 50000 47900 4
C 49600 45400 1 0 1 vcc-1.sym
C 51300 47900 1 180 0 pmos-3.sym
{
T 50700 47400 5 10 0 0 180 0 1
device=PMOS_TRANSISTOR
T 50800 47600 5 10 1 1 180 0 1
refdes=Q12
T 50700 46700 5 10 1 1 0 6 1
value=IRF5305
}
N 55800 44800 56100 44800 4
N 55800 45200 56100 45200 4
T 56300 44700 9 10 1 0 0 0 3
+
Load
-
N 55800 45200 55800 46100 4
C 41600 46400 1 0 1 gnd-1.sym
C 54300 46400 1 0 0 gnd-1.sym
C 49300 42800 1 0 0 gnd-1.sym
N 54400 47500 54400 47700 4
N 41500 47500 41500 47700 4
C 43200 42800 1 0 1 gnd-1.sym
T 40000 43500 9 10 1 0 0 0 1
Buck Lower
T 55100 46900 9 10 1 0 0 0 1
Boost Upper
T 52700 43400 9 10 1 0 0 6 1
Boost Lower
T 40200 46600 9 10 1 0 0 0 1
Buck Upper
N 41500 47700 42400 47700 4
C 44400 50300 1 0 0 vcc-1.sym
C 42200 50200 1 270 0 zener-1.sym
{
T 42800 49800 5 10 0 0 270 0 1
device=ZENER_DIODE
T 42700 49800 5 10 1 1 0 0 1
refdes=Z1
T 42700 49500 5 10 1 1 0 0 1
value=3.3V
}
N 42400 50200 42400 50300 4
N 42400 50300 44600 50300 4
N 45300 49300 44600 49300 4
C 43200 46400 1 0 1 gnd-1.sym
N 43900 46700 44600 46700 4
C 41600 44100 1 90 0 resistor-1.sym
{
T 41200 44400 5 10 0 0 90 0 1
device=RESISTOR
T 41600 44100 5 10 0 0 0 0 1
footprint=R025
T 41300 44900 5 10 1 1 180 0 1
refdes=R2
T 41300 44600 5 10 1 1 180 0 1
value=1K
}
N 41500 45000 41500 45400 4
C 41000 43100 1 0 0 nmos-3.sym
{
T 41600 43600 5 10 0 0 0 0 1
device=NMOS_TRANSISTOR
T 41000 43100 5 10 0 0 0 0 1
file=/home/ksarkies/Development-Electronics/Spice-models/models.lib
T 40500 42900 5 10 1 1 0 0 1
refdes=Q2
T 40500 42700 5 10 1 1 0 0 1
value=2N7000
}
C 41600 42800 1 0 1 gnd-1.sym
N 41500 43900 41500 44100 4
N 41500 45400 43100 45400 4
N 41500 44100 42400 44100 4
C 51000 44100 1 270 1 resistor-1.sym
{
T 51400 44400 5 10 0 0 90 2 1
device=RESISTOR
T 51000 44100 5 10 0 0 0 6 1
footprint=R025
T 51300 44900 5 10 1 1 180 6 1
refdes=R3
T 51300 44600 5 10 1 1 180 6 1
value=1K
}
N 51100 45000 51100 45400 4
C 51600 43100 1 0 1 nmos-3.sym
{
T 51000 43600 5 10 0 0 0 6 1
device=NMOS_TRANSISTOR
T 51600 43100 5 10 0 0 0 6 1
file=/home/ksarkies/Development-Electronics/Spice-models/models.lib
T 52100 42900 5 10 1 1 0 6 1
refdes=Q13
T 52100 42700 5 10 1 1 0 6 1
value=2N7000
}
C 51000 42800 1 0 0 gnd-1.sym
N 51100 43900 51100 44100 4
N 51100 44100 50100 44100 4
N 51100 45400 49400 45400 4
N 55800 44800 55800 42400 4
N 54400 47700 53500 47700 4
C 52700 46400 1 0 0 gnd-1.sym
N 50800 47100 50800 46100 4
N 50800 46100 55800 46100 4
C 53900 50200 1 90 1 zener-1.sym
{
T 53300 49800 5 10 0 0 270 2 1
device=ZENER_DIODE
T 53400 49800 5 10 1 1 0 6 1
refdes=Z4
T 53400 49500 5 10 1 1 0 6 1
value=3.3V
}
N 53700 50200 53700 50300 4
N 47800 50300 53700 50300 4
N 54400 49300 51800 49300 4
C 52000 48900 1 90 1 capacitor-1.sym
{
T 51300 49300 5 10 0 0 270 2 1
device=CAPACITOR
T 51100 49300 5 10 0 0 270 2 1
symversion=0.1
T 52000 49500 5 10 0 0 180 6 1
footprint=CK05_type_Capacitor
T 51600 48900 5 10 1 1 0 6 1
refdes=C3
T 51700 48600 5 10 1 1 0 6 1
value=0.1uF
}
N 51800 49300 51800 48900 4
N 51800 48000 51800 46100 4
C 45400 48100 1 0 1 gnd-1.sym
C 41800 43200 1 270 1 zener-1.sym
{
T 42400 43600 5 10 0 0 90 2 1
device=ZENER_DIODE
T 42200 43200 5 10 1 1 180 6 1
refdes=Z2
T 42200 42900 5 10 1 1 180 6 1
value=5.6V
}
C 42100 42800 1 0 1 gnd-1.sym
N 42000 43100 42000 43200 4
C 50300 43200 1 270 1 zener-1.sym
{
T 50900 43600 5 10 0 0 90 2 1
device=ZENER_DIODE
T 50000 43200 5 10 1 1 180 6 1
refdes=Z3
T 50000 42900 5 10 1 1 180 6 1
value=5.6V
}
C 50600 42800 1 0 1 gnd-1.sym
N 50500 43100 50500 43200 4
C 54900 44300 1 270 1 resistor-1.sym
{
T 55300 44600 5 10 0 0 90 2 1
device=RESISTOR
T 55200 45100 5 10 1 1 180 6 1
refdes=R5
T 55200 44800 5 10 1 1 180 6 1
value=10K
T 54900 44300 5 10 0 0 0 6 1
footprint=R025
}
N 55000 45200 55000 46100 4
N 55000 44300 55000 42400 4