R0 = 0.002;
R1 = 0.002;

z0 = 0;
z1 = 0.008;
z2 = 0.008;

slope = 0.58;
If ( slope > 0 )
  R1 = 0.0002;
  z1 = z0 + (R0-R1)/slope;
  z2 = z1;
EndIf

Printf("z1 = %g mm", z1*1e3);

nR = 31;
nZ = 126;

qR_bot = 1.04;
qR_top = 1.0;
qZ = 0.3;

Point(1) = {0,  z0, 0};
Point(2) = {R0, z0, 0};
Point(3) = {R1, z1, 0};
Point(4) = {0,  z2, 0};

Line(1) = {1, 2};
Line(2) = {2, 3};
Line(3) = {3, 4};
Line(4) = {4, 1};

Line Loop(1) = {1, 2, 3, 4};

Plane Surface(1) = {1};

Physical Line(0) = {1};
Physical Line(1) = {2,3};
Physical Line(2) = {4};
Physical Surface(0) = {1};

Transfinite Line {-1} = nR Using Progression qR_bot;
Transfinite Line {3}  = nR Using Progression qR_top;
Transfinite Line {2,-4} = nZ Using Bump qZ;
Transfinite Surface "*";

Recombine Surface "*";
