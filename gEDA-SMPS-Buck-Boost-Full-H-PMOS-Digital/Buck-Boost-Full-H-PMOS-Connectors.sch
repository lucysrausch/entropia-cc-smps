v 20130925 2
C 40000 40000 0 0 0 title-B.sym
T 51300 41000 9 10 1 0 0 0 1
Battery Charger using H-Bridge Buck-Boost
T 54000 40100 9 10 1 0 0 0 1
Ken Sarkies
T 50600 40100 9 10 1 0 0 0 1
3
T 52100 40100 9 10 1 0 0 0 1
3
C 45900 44300 1 0 0 output-2.sym
{
T 46100 45000 5 10 0 0 0 0 1
device=none
T 46800 44400 5 10 0 1 0 1 1
value=UVsignal
T 46800 44300 5 10 1 1 0 0 1
net=BuckLow:1
}
C 54800 46700 1 0 0 output-2.sym
{
T 55000 47400 5 10 0 0 0 0 1
device=none
T 55700 46800 5 10 0 1 0 1 1
value=UVsignal
T 55700 46700 5 10 1 1 0 0 1
net=BoostUp:1
}
C 54800 46900 1 0 0 output-2.sym
{
T 55000 47600 5 10 0 0 0 0 1
device=none
T 55700 47000 5 10 0 1 0 1 1
value=UVsignal
T 55700 46900 5 10 1 1 0 0 1
net=BoostLow:1
}
C 45900 49100 1 0 0 output-2.sym
{
T 46100 49800 5 10 0 0 0 0 1
device=none
T 46800 49200 5 10 0 1 0 1 1
value=UVsignal
T 46800 49100 5 10 1 1 0 0 1
net=BuckUp:1
}
C 54800 41800 1 0 0 output-2.sym
{
T 55100 41600 5 10 1 1 0 0 1
net=iBatt-:1
T 55000 42500 5 10 0 0 0 0 1
device=none
T 55700 41900 5 10 0 1 0 1 1
value=UVsignal
}
C 54800 49200 1 0 0 output-2.sym
{
T 55000 49000 5 10 1 1 0 0 1
net=iSwitch+:1
T 55000 49900 5 10 0 0 0 0 1
device=none
T 55700 49300 5 10 0 1 0 1 1
value=UVsignal
}
C 54800 48700 1 0 0 output-2.sym
{
T 55000 48500 5 10 1 1 0 0 1
net=iSwitch-:1
T 55000 49400 5 10 0 0 0 0 1
device=none
T 55700 48800 5 10 0 1 0 1 1
value=UVsignal
}
C 54800 44800 1 0 0 output-2.sym
{
T 55000 44600 5 10 1 1 0 0 1
net=VBatt+:1
T 55000 45500 5 10 0 0 0 0 1
device=none
T 55700 44900 5 10 0 1 0 1 1
value=UVsignal
}
C 54800 44300 1 0 0 output-2.sym
{
T 55000 44100 5 10 1 1 0 0 1
net=VBatt+:1
T 55000 45000 5 10 0 0 0 0 1
device=none
T 55700 44400 5 10 0 1 0 1 1
value=UVsignal
}
C 54800 42300 1 0 0 output-2.sym
{
T 55100 42100 5 10 1 1 0 0 1
net=iBatt-:1
T 55000 43000 5 10 0 0 0 0 1
device=none
T 55700 42400 5 10 0 1 0 1 1
value=UVsignal
}
C 45900 45600 1 0 1 connector10-2.sym
{
T 45200 50100 5 10 1 1 0 0 1
refdes=CONN1
T 45600 50050 5 10 0 0 0 6 1
device=CONNECTOR_10
T 45600 50250 5 10 0 0 0 6 1
footprint=SIP10N
}
C 45900 40800 1 0 1 connector10-2.sym
{
T 45200 45300 5 10 1 1 0 0 1
refdes=CONN2
T 45600 45250 5 10 0 0 0 6 1
device=CONNECTOR_10
T 45600 45450 5 10 0 0 0 6 1
footprint=SIP10N
}
C 51300 49200 1 0 0 output-2.sym
{
T 51500 49000 5 10 1 1 0 0 1
net=Comparator:1
T 51500 49900 5 10 0 0 0 0 1
device=none
T 52200 49300 5 10 0 1 0 1 1
value=UVsignal
}
C 51300 48500 1 0 0 output-2.sym
{
T 51600 48300 5 10 1 1 0 0 1
net=iLimit:1
T 51500 49200 5 10 0 0 0 0 1
device=none
T 52200 48600 5 10 0 1 0 1 1
value=UVsignal
}
T 54000 40400 9 10 1 0 0 0 1
11/11/2010
C 42200 47900 1 0 1 connector2-2.sym
{
T 41500 49200 5 10 1 1 0 0 1
refdes=TERM1
T 41900 49150 5 10 0 0 0 6 1
device=CONNECTOR_2
T 41900 49350 5 10 0 0 0 6 1
footprint=terminal-block-2
}
C 42200 48200 1 0 0 output-2.sym
{
T 42400 48000 5 10 1 1 0 0 1
net=VBatt+:1
T 42400 48900 5 10 0 0 0 0 1
device=none
T 43100 48300 5 10 0 1 0 1 1
value=UVsignal
}
C 42200 48600 1 0 0 output-2.sym
{
T 42400 48900 5 10 1 1 0 0 1
net=VBatt+:1
T 42400 49300 5 10 0 0 0 0 1
device=none
T 43100 48700 5 10 0 1 0 1 1
value=UVsignal
}
C 42000 46700 1 0 0 vcc-1.sym
C 42100 46000 1 0 0 gnd-2.sym
C 42200 45900 1 0 1 connector2-2.sym
{
T 41500 47200 5 10 1 1 0 0 1
refdes=TERM2
T 41900 47150 5 10 0 0 0 6 1
device=CONNECTOR_2
T 41900 47350 5 10 0 0 0 6 1
footprint=terminal-block-2
}
