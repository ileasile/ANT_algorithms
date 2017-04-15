task7 := function()
local R, f, g, h, dr, d, r, u, v, uvl, inp, p, a, n, i;
	SetInfoLevel(InfoWarning, 0);
	p := 7; #2; #if you need GF(2)
	R := PolynomialRing(GF(p),["x","y","z"]);
	
	#input
	inp := InputTextFile( "input.txt" );
	f := UnivariatePolynomial(GF(p), EvalString(ReadLine(inp)));
	g := UnivariatePolynomial(GF(p), EvalString(ReadLine(inp)));
	
	#output
	Print("f(x) = ", f, "\ng(x) = ", g, "\n");
	
	#multiplication
	h := f * g;
	Print("(f*g)(x) = ", h, "\n");
	
	#division and remainder
	dr := QuotientRemainder(f, g);
	Print("(f/g)(x) = (Q = ", dr[1], ", R = ", dr[2], ")\n");
	
	#gcd
	d := Gcd(f, g);
	Print("(f, g) = ", d, "\n");
	
	#extended gcd
	uvl := GcdRepresentation(f, g);
	Print("u = ", uvl[1], "\nv = ", uvl[2], "\n");
	
	#derivative of f
	d := Derivative(f);
	Print("df/dx = ", d, "\n");
	
	#f(a)
	a := 3;
	Print("f(", a, ") = ", Value(f, a), "\n");
	
	#factorisation of f
	uvl := Factors(f);
	Print("Factors of f are ", uvl, "\n");
	
	#test for irreducibility
	Print("Is f irreducible: ", Length(uvl) = 1, "\n");
	
	#multiplication of several polynomials mod f
	n := EvalString(ReadLine(inp));
	r := UnivariatePolynomial(GF(p), [1]);
	for i in [1..n] do
		h := UnivariatePolynomial(GF(p), EvalString(ReadLine(inp)));;
		r := (r * h) mod f;
	od;
	Print("Result of modular multiplication: ", r, "\n");
	
	# ????????? modular composition ????????
	
	
	CloseStream(inp);
end;

#Length of list c should be even. 
#Otherwise, only first Length(c) - 1 elements of list will be taken.
RestoreFunByLaurentCofs := function(c)
local A, B, sols, m, k, i, j;
	PolynomialRing(Rationals,["x","y","z"]);
	k := QuoInt(Length(c), 2);
	
	m := NullMat(2*k + 1, 2*k, Rationals);
	for i in [1..k] do
		for j in [i + 1 .. k + 1] do
			m[j][i] := c[j - i];
		od;
		m[k + 1 + i][i] := -1;
	od;
	for i in [k + 1 .. 2*k] do
		for j in [1 .. k + 1] do
			m[j][i] := c[j + i - k - 1];
		od;
	od;
	
	sols := NullspaceMat(m);
	A := sols[1]{[k + 2 .. 2*k + 1]};
	B := sols[1]{[1 .. k + 1]};
	
	return UnivariateRationalFunctionByCoefficients(FamilyObj(1), A, B, 0);
end;

#usage:
#Read("AlgNT_8.g");
#RestoreFunByLaurentCofs([0, 0, 1, 7, -3, 0, -2, -21, -43, 21]);
