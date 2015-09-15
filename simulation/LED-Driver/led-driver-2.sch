v 20110115 2
C 40000 40000 0 0 0 title-B.sym
C 44100 46200 1 0 0 MC34063-1.sym
{
T 45900 47600 5 10 1 1 0 6 1
refdes=U1
T 44400 48200 5 10 0 0 0 0 1
footprint=DIP8
T 44400 47800 5 10 0 0 0 0 1
device=MC34063
T 44400 48800 5 10 0 0 0 0 1
symversion=1.0
}
N 44100 46400 44100 46100 4
N 46200 44000 46200 46400 4
N 46200 46700 47100 46700 4
N 46200 47000 46200 48000 4
N 46200 48000 44100 48000 4
N 44100 48000 44100 47300 4
C 43200 46500 1 0 0 capacitor-1.sym
{
T 43400 47200 5 10 0 0 0 0 1
device=CAPACITOR
T 43300 46300 5 10 1 1 0 0 1
refdes=Cosc
T 43400 47400 5 10 0 0 0 0 1
symversion=0.1
}
N 43200 46700 43200 46100 4
N 42500 47000 44100 47000 4
C 41600 46900 1 0 0 inductor-1.sym
{
T 41800 47400 5 10 0 0 0 0 1
device=INDUCTOR
T 41800 47200 5 10 1 1 0 0 1
refdes=Lfilter
T 41800 47600 5 10 0 0 0 0 1
symversion=0.1
}
C 43000 46100 1 90 0 diode-2.sym
{
T 42400 46500 5 10 0 0 90 0 1
device=DIODE
T 43000 46300 5 10 1 1 180 0 1
refdes=D1
}
C 41400 47000 1 270 0 capacitor-4.sym
{
T 42500 46800 5 10 0 0 270 0 1
device=POLARIZED_CAPACITOR
T 41900 46200 5 10 1 1 0 0 1
refdes=Cfilter
T 42100 46800 5 10 0 0 270 0 1
symversion=0.1
}
C 41500 45800 1 0 0 gnd-1.sym
C 42600 45800 1 0 0 gnd-1.sym
C 43100 45800 1 0 0 gnd-1.sym
C 44000 45800 1 0 0 gnd-1.sym
N 40800 47000 41600 47000 4
C 46200 46900 1 0 0 resistor-1.sym
{
T 46500 47300 5 10 0 0 0 0 1
device=RESISTOR
T 46400 47200 5 10 1 1 0 0 1
refdes=Rlimit
}
N 47100 45400 47100 48000 4
C 46900 48000 1 0 0 12V-plus-1.sym
C 43600 45400 1 180 0 led-3.sym
{
T 42650 44750 5 10 0 0 180 0 1
device=LED
T 42850 44650 5 10 1 1 0 0 1
refdes=Dled
}
C 43700 44300 1 90 0 resistor-1.sym
{
T 43300 44600 5 10 0 0 90 0 1
device=RESISTOR
T 44400 44700 5 10 1 1 180 0 1
refdes=Rsense
}
C 45200 43900 1 0 0 resistor-1.sym
{
T 45500 44300 5 10 0 0 0 0 1
device=RESISTOR
T 45400 43700 5 10 1 1 0 0 1
refdes=Rf
}
N 43600 45200 45200 45200 4
N 46200 44000 46100 44000 4
N 45200 44800 45200 44000 4
C 45200 44100 1 180 0 resistor-1.sym
{
T 44900 43700 5 10 0 0 180 0 1
device=RESISTOR
T 44700 44200 5 10 1 1 0 0 1
refdes=R1
}
N 47100 45400 45700 45400 4
C 45600 44300 1 0 0 gnd-1.sym
C 44200 43700 1 0 0 gnd-1.sym
C 43500 43700 1 0 0 gnd-1.sym
N 40800 47000 40800 45200 4
N 40800 45200 42700 45200 4
N 43600 44000 43600 44300 4
C 45200 44600 1 0 0 lm358-1.sym
{
T 45875 45200 5 10 0 0 0 0 1
device=LM358
T 45900 45950 5 10 0 0 0 0 1
footprint=DIP8
T 45400 45500 5 10 1 1 0 0 1
refdes=U2
}