v 20100214 2
C 40000 40000 0 0 0 title-B.sym
T 51800 41100 9 10 1 0 0 0 1
Synchronous SMPS, High side PMOS, Voltage Mode
T 54000 40100 9 10 1 0 0 0 1
Ken Sarkies
T 54000 40400 9 10 1 0 0 0 1
10/9/2010
T 50600 40100 9 10 1 0 0 0 1
1
T 52100 40100 9 10 1 0 0 0 1
1
T 51800 40800 9 10 1 0 0 0 1
ATMega48 Version
N 43000 46300 41800 46300 4
N 43000 44000 42600 44000 4
C 45600 47300 1 180 0 ATmega8-2.sym
{
T 45500 42500 5 10 0 0 180 0 1
footprint=DIP28N
T 43300 42700 5 10 1 1 180 6 1
refdes=U1
T 45500 41700 5 10 0 0 180 0 1
device=ATmega8
}
N 43000 46600 43000 47200 4
N 43000 47200 41800 47200 4
C 51000 45100 1 0 0 nmos-3.sym
{
T 51600 45600 5 10 0 0 0 0 1
device=NMOS_TRANSISTOR
T 51700 45600 5 10 1 1 0 0 1
refdes=Q6
T 51600 45400 5 10 1 1 0 0 1
value=IRL1104
}
C 49200 48500 1 0 0 npn-2.sym
{
T 49800 49000 5 10 0 0 0 0 1
device=NPN_TRANSISTOR
T 49800 49000 5 10 1 1 0 0 1
refdes=Q1
T 49800 48800 5 10 1 1 0 0 1
value=2N3904
}
C 51000 48700 1 180 1 pmos-3.sym
{
T 51600 48200 5 10 0 0 180 6 1
device=PMOS_TRANSISTOR
T 51700 48500 5 10 1 1 180 6 1
refdes=Q5
T 51700 48100 5 10 1 1 0 0 1
value=IRF5305
}
C 49200 48500 1 180 1 pnp-2.sym
{
T 49800 48100 5 10 0 0 180 6 1
device=PNP_TRANSISTOR
T 49800 48300 5 10 1 1 180 6 1
refdes=Q2
T 49800 47900 5 10 1 1 0 0 1
value=2N3906
}
C 54100 46300 1 270 0 capacitor-4.sym
{
T 55200 46100 5 10 0 0 270 0 1
device=POLARIZED_CAPACITOR
T 53900 45400 5 10 1 1 0 0 1
refdes=C6
T 54800 46100 5 10 0 0 270 0 1
symversion=0.1
T 53800 45200 5 10 1 1 0 0 1
value=47uF
}
C 52700 46200 1 0 0 coil-2.sym
{
T 52900 46700 5 10 0 0 0 0 1
device=COIL
T 53300 46700 5 10 1 1 0 0 1
refdes=L2
T 52900 46900 5 10 0 0 0 0 1
symversion=0.1
T 53200 46500 5 10 1 1 0 0 1
value=300uH
}
C 53000 45000 1 90 0 diode-2.sym
{
T 52400 45400 5 10 0 0 90 0 1
device=DIODE
T 53300 45600 5 10 1 1 180 0 1
refdes=D1
T 53000 45200 5 10 1 1 0 0 1
value=1N5819
}
N 51500 46300 52700 46300 4
N 51500 45900 51500 47900 4
N 52700 45900 52700 46300 4
N 49700 45300 51000 45300 4
N 49700 48500 51000 48500 4
N 49200 49000 49000 49000 4
N 49000 49000 49000 48000 4
N 49000 48000 49200 48000 4
N 48100 48500 49000 48500 4
N 51500 48700 51500 50400 4
N 51500 44300 51500 45100 4
N 52700 44300 52700 45000 4
N 49200 45800 49000 45800 4
N 49000 44800 49200 44800 4
C 49500 46700 1 0 0 vcc-1.sym
C 49600 47200 1 0 0 gnd-1.sym
N 53700 46300 56200 46300 4
N 54300 44300 54300 45400 4
C 43900 49800 1 270 0 capacitor-4.sym
{
T 45000 49600 5 10 0 0 270 0 1
device=POLARIZED_CAPACITOR
T 44500 49100 5 10 1 1 0 0 1
refdes=C3
T 44600 49600 5 10 0 0 270 0 1
symversion=0.1
T 44500 48900 5 10 1 1 0 0 1
value=22uF
}
C 42200 49200 1 0 0 lm7805-1.sym
{
T 43800 50500 5 10 0 0 0 0 1
device=7805
T 43600 50200 5 10 1 1 0 6 1
refdes=U2
}
C 45500 49900 1 180 0 resistor-1.sym
{
T 45200 49500 5 10 0 0 180 0 1
device=RESISTOR
T 45100 49400 5 10 1 1 0 0 1
refdes=R1
T 45000 49200 5 10 1 1 0 0 1
value=100
T 45500 49900 5 10 0 0 90 0 1
footprint=R025
}
N 43800 49800 44600 49800 4
N 45500 49800 51500 49800 4
C 44000 48600 1 0 0 gnd-1.sym
C 42900 48900 1 0 0 gnd-1.sym
C 42000 46300 1 90 0 capacitor-4.sym
{
T 40900 46500 5 10 0 0 90 0 1
device=POLARIZED_CAPACITOR
T 41300 47200 5 10 1 1 180 0 1
refdes=C1
T 41300 46500 5 10 0 0 90 0 1
symversion=0.1
T 41000 46800 5 10 1 1 0 0 1
value=22uF
}
C 41900 47500 1 180 0 gnd-1.sym
C 41800 46400 1 180 0 coil-2.sym
{
T 41600 45900 5 10 0 0 180 0 1
device=COIL
T 41600 46100 5 10 1 1 180 0 1
refdes=L1
T 41600 45700 5 10 0 0 180 0 1
symversion=0.1
}
N 42600 44000 42600 42300 4
N 45600 46100 49000 46100 4
N 49000 44800 49000 46100 4
N 45600 45900 47000 45900 4
N 40800 49800 42200 49800 4
N 40800 42900 40800 49800 4
C 55400 45400 1 90 0 resistor-1.sym
{
T 55000 45700 5 10 0 0 90 0 1
device=RESISTOR
T 55100 46100 5 10 1 1 180 0 1
refdes=R3
T 55100 45900 5 10 1 1 180 0 1
value=33K
T 55400 45400 5 10 0 0 0 0 1
footprint=R025
}
C 55400 44500 1 90 0 resistor-1.sym
{
T 55000 44800 5 10 0 0 90 0 1
device=RESISTOR
T 55100 45200 5 10 1 1 180 0 1
refdes=R4
T 55100 45000 5 10 1 1 180 0 1
value=10K
T 55400 44500 5 10 0 0 0 0 1
footprint=R025
}
N 49700 44300 55300 44300 4
N 55300 44300 55300 44500 4
N 42600 42300 55900 42300 4
N 55900 42300 55900 45400 4
N 55900 45400 55300 45400 4
C 51300 50400 1 0 0 vss-1.sym
N 48100 47500 50600 47500 4
N 50600 47500 50600 48900 4
N 50600 45800 50600 44300 4
N 49700 46300 49700 46700 4
N 49700 46700 50600 46700 4
N 49700 49500 49700 49800 4
C 49600 44000 1 0 0 gnd-1.sym
C 50400 49800 1 270 0 capacitor-1.sym
{
T 51100 50200 5 10 0 0 270 0 1
device=CAPACITOR
T 51300 50200 5 10 0 0 270 0 1
symversion=0.1
T 50400 50400 5 10 0 0 180 0 1
footprint=CK05_type_Capacitor
T 51000 49300 5 10 1 1 0 0 1
refdes=C4
T 51000 49100 5 10 1 1 0 0 1
value=0.1uF
}
C 48200 48900 1 90 0 resistor-1.sym
{
T 47800 49200 5 10 0 0 90 0 1
device=RESISTOR
T 47900 49300 5 10 1 1 180 0 1
refdes=R2
T 47900 49100 5 10 1 1 180 0 1
value=1K
T 48200 48900 5 10 0 0 0 0 1
footprint=R025
}
N 48100 48900 48100 48500 4
C 47300 47600 1 270 0 capacitor-1.sym
{
T 48000 48000 5 10 0 0 270 0 1
device=CAPACITOR
T 48200 48000 5 10 0 0 270 0 1
symversion=0.1
T 47300 48200 5 10 0 0 180 0 1
footprint=CK05_type_Capacitor
T 47900 47100 5 10 1 1 0 0 1
refdes=C7
T 47800 46900 5 10 1 1 0 0 1
value=100pF
}
C 47100 46700 1 90 0 resistor-1.sym
{
T 46700 47000 5 10 0 0 90 0 1
device=RESISTOR
T 46900 47600 5 10 1 1 180 0 1
refdes=R3
T 46900 47400 5 10 1 1 180 0 1
value=10K
T 47100 46700 5 10 0 0 0 0 1
footprint=R025
}
N 47500 46700 47000 46700 4
N 47000 45200 47000 46700 4
N 47500 47600 47000 47600 4
N 47000 47600 47000 48000 4
N 47000 48000 47600 48000 4
C 50400 46700 1 270 0 capacitor-1.sym
{
T 51100 47100 5 10 0 0 270 0 1
device=CAPACITOR
T 51300 47100 5 10 0 0 270 0 1
symversion=0.1
T 50400 47300 5 10 0 0 180 0 1
footprint=CK05_type_Capacitor
T 51000 46200 5 10 1 1 0 0 1
refdes=C5
T 51000 46000 5 10 1 1 0 0 1
value=0.1uF
}
C 42300 47200 1 270 0 capacitor-1.sym
{
T 43000 47600 5 10 0 0 270 0 1
device=CAPACITOR
T 43200 47600 5 10 0 0 270 0 1
symversion=0.1
T 42300 47800 5 10 0 0 180 0 1
footprint=CK05_type_Capacitor
T 42400 47600 5 10 1 1 0 0 1
refdes=C2
T 42400 47400 5 10 1 1 0 0 1
value=0.1uF
}
C 47600 47500 1 0 0 npn-2.sym
{
T 48200 48000 5 10 0 0 0 0 1
device=NPN_TRANSISTOR
T 48200 48000 5 10 1 1 0 0 1
refdes=Q7
T 48200 47800 5 10 1 1 0 0 1
value=2N3904
}
C 49200 45300 1 0 0 npn-2.sym
{
T 49800 45800 5 10 0 0 0 0 1
device=NPN_TRANSISTOR
T 49800 45800 5 10 1 1 0 0 1
refdes=Q3
T 49800 45600 5 10 1 1 0 0 1
value=2N3904
}
C 49200 45300 1 180 1 pnp-2.sym
{
T 49800 44900 5 10 0 0 180 6 1
device=PNP_TRANSISTOR
T 49800 45000 5 10 1 1 180 6 1
refdes=Q4
T 49800 44600 5 10 1 1 0 0 1
value=2N3906
}
C 41200 40300 1 0 1 connector6-3.sym
{
T 40700 42400 5 10 1 1 0 0 1
refdes=H1
T 40900 41950 5 10 0 0 0 6 1
device=CONNECTOR_6
T 40900 42150 5 10 0 0 0 6 1
footprint=CONNECTOR 1 6
}
N 43000 45200 42300 45200 4
N 42300 45200 42300 40800 4
N 42300 40800 41200 40800 4
N 45600 46300 46100 46300 4
N 46100 46300 46100 41700 4
N 46100 41700 41200 41700 4
N 45600 46500 46400 46500 4
N 46400 46500 46400 41400 4
N 46400 41400 44400 41400 4
C 44400 41500 1 180 0 resistor-1.sym
{
T 44100 41100 5 10 0 0 180 0 1
device=RESISTOR
T 44400 41500 5 10 1 1 0 0 1
refdes=R5
T 44400 41200 5 10 1 1 0 0 1
value=220
T 44400 41500 5 10 0 0 90 0 1
footprint=R025
}
N 43500 41400 41200 41400 4
N 45600 46700 46700 46700 4
N 46700 46700 46700 41100 4
N 46700 41100 41200 41100 4
C 41500 41900 1 90 0 gnd-1.sym
C 42300 45300 1 180 0 resistor-1.sym
{
T 42000 44900 5 10 0 0 180 0 1
device=RESISTOR
T 41700 44800 5 10 1 1 0 0 1
refdes=R6
T 41600 44600 5 10 1 1 0 0 1
value=10K
T 42300 45300 5 10 0 0 90 0 1
footprint=R025
}
C 41400 43700 1 0 0 capacitor-1.sym
{
T 41000 44400 5 10 0 0 0 0 1
device=CAPACITOR
T 41000 44600 5 10 0 0 0 0 1
symversion=0.1
T 40800 43700 5 10 0 0 270 0 1
footprint=CK05_type_Capacitor
T 41900 43600 5 10 1 1 180 0 1
refdes=C8
T 42100 43400 5 10 1 1 180 0 1
value=0.1uF
}
C 41100 43800 1 270 1 gnd-1.sym
C 48200 44300 1 90 0 resistor-1.sym
{
T 47800 44600 5 10 0 0 90 0 1
device=RESISTOR
T 48600 45200 5 10 1 1 180 0 1
refdes=R8
T 48600 45000 5 10 1 1 180 0 1
value=10K
T 48200 44300 5 10 0 0 0 0 1
footprint=R025
}
C 48000 44000 1 0 0 gnd-1.sym
N 48100 45200 48100 46100 4
C 47100 44300 1 90 0 resistor-1.sym
{
T 46700 44600 5 10 0 0 90 0 1
device=RESISTOR
T 47500 45200 5 10 1 1 180 0 1
refdes=R7
T 47500 45000 5 10 1 1 180 0 1
value=10K
T 47100 44300 5 10 0 0 0 0 1
footprint=R025
}
C 46900 44000 1 0 0 gnd-1.sym
N 40800 45200 41400 45200 4
N 40800 42900 41800 42900 4
N 41800 42900 41800 40500 4
N 41800 40500 41200 40500 4
