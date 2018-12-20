1 # (c) 2014 David Douard <david.douard@gmail.com>
2 # Based on https://github.com/attoparsec/inkscape-extensions.git
3 # Based on gearUtils-03.js by Dr A.R.Collins
4 # http://www.arc.id.au/gearDrawing.html
5 #
6 # Calculation of Bezier coefficients for
7 # Higuchi et al. approximation to an involute.
8 # ref: YNU Digital Eng Lab Memorandum 05-1
9 #
10 # This program is free software; you can redistribute it and/or modify
11 # it under the terms of the GNU Lesser General Public License (LGPL)
12 # as published by the Free Software Foundation; either version 2 of
13 # the License, or (at your option) any later version.
14 # for detail see the LICENCE text file.
15 #
16 # FCGear is distributed in the hope that it will be useful,
17 # but WITHOUT ANY WARRANTY; without even the implied warranty of
18 # MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
19 # GNU Library General Public License for more details.
20 #
21 # You should have received a copy of the GNU Library General Public
22 # License along with FCGear; if not, write to the Free Software
23 # Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
24 
25 from math import cos, sin, pi, acos, asin, atan, sqrt
26 
27 def CreateExternalGear(w, m, Z, phi, split=True):
28  """
29  Create an external gear
30 
31  w is wirebuilder object (in which the gear will be constructed)
32 
33  if split is True, each profile of a teeth will consist in 2 Bezier
34  curves of degree 3, otherwise it will be made of one Bezier curve
35  of degree 4
36  """
37  # ****** external gear specifications
38  addendum = m # distance from pitch circle to tip circle
39  dedendum = 1.25 * m # pitch circle to root, sets clearance
40  clearance = dedendum - addendum
41 
42  # Calculate radii
43  Rpitch = Z * m / 2 # pitch circle radius
44  Rb = Rpitch*cos(phi * pi / 180) # base circle radius
45  Ra = Rpitch + addendum # tip (addendum) circle radius
46  Rroot = Rpitch - dedendum # root circle radius
47  fRad = 1.5 * clearance # fillet radius, max 1.5*clearance
48  Rf = sqrt((Rroot + fRad)**2 - fRad**2) # radius at top of fillet
49  if (Rb < Rf):
50  Rf = Rroot + clearance
51 
52  # ****** calculate angles (all in radians)
53  pitchAngle = 2 * pi / Z # angle subtended by whole tooth (rads)
54  baseToPitchAngle = genInvolutePolar(Rb, Rpitch)
55  pitchToFilletAngle = baseToPitchAngle # profile starts at base circle
56  if (Rf > Rb): # start profile at top of fillet (if its greater)
57  pitchToFilletAngle -= genInvolutePolar(Rb, Rf)
58 
59  filletAngle = atan(fRad / (fRad + Rroot)) # radians
60 
61  # ****** generate Higuchi involute approximation
62  fe = 1 # fraction of profile length at end of approx
63  fs = 0.01 # fraction of length offset from base to avoid singularity
64  if (Rf > Rb):
65  fs = (Rf**2 - Rb**2) / (Ra**2 - Rb**2) # offset start to top of fillet
66 
67  if split:
68  # approximate in 2 sections, split 25% along the involute
69  fm = fs + (fe - fs) / 4 # fraction of length at junction (25% along profile)
70  dedInv = BezCoeffs(m, Z, phi, 3, fs, fm)
71  addInv = BezCoeffs(m, Z, phi, 3, fm, fe)
72 
73  # join the 2 sets of coeffs (skip duplicate mid point)
74  inv = dedInv + addInv[1:]
75  else:
76  inv = BezCoeffs(m, Z, phi, 4, fs, fe)
77 
78  # create the back profile of tooth (mirror image)
79  invR = []
80  for i, pt in enumerate(inv):
81  # rotate all points to put pitch point at y = 0
82  ptx, pty = inv[i] = rotate(pt, -baseToPitchAngle - pitchAngle / 4)
83  # generate the back of tooth profile nodes, mirror coords in X axis
84  invR.append((ptx, -pty))
85 
86  # ****** calculate section junction points R=back of tooth, Next=front of next tooth)
87  fillet = toCartesian(Rf, -pitchAngle / 4 - pitchToFilletAngle) # top of fillet
88  filletR = [fillet[0], -fillet[1]] # flip to make same point on back of tooth
89  rootR = toCartesian(Rroot, pitchAngle / 4 + pitchToFilletAngle + filletAngle)
90  rootNext = toCartesian(Rroot, 3 * pitchAngle / 4 - pitchToFilletAngle - filletAngle)
91  filletNext = rotate(fillet, pitchAngle) # top of fillet, front of next tooth
92 
93  # Build the shapes using FreeCAD.Part
94  t_inc = 2.0 * pi / float(Z)
95  thetas = [(x * t_inc) for x in range(Z)]
96 
97  w.move(fillet) # start at top of fillet
98 
99  for theta in thetas:
100  w.theta = theta
101  if (Rf < Rb):
102  w.line(inv[0]) # line from fillet up to base circle
103 
104  if split:
105  w.curve(inv[1], inv[2], inv[3])
106  w.curve(inv[4], inv[5], inv[6])
107  w.arc(invR[6], Ra, 1) # arc across addendum circle
108  w.curve(invR[5], invR[4], invR[3])
109  w.curve(invR[2], invR[1], invR[0])
110  else:
111  w.curve(*inv[1:])
112  w.arc(invR[-1], Ra, 1) # arc across addendum circle
113  w.curve(*invR[-2::-1])
114 
115  if (Rf < Rb):
116  w.line(filletR) # line down to topof fillet
117 
118  if (rootNext[1] > rootR[1]): # is there a section of root circle between fillets?
119  w.arc(rootR, fRad, 0) # back fillet
120  w.arc(rootNext, Rroot, 1) # root circle arc
121 
122  w.arc(filletNext, fRad, 0)
123 
124  w.close()
125  return w
126 
127 def CreateInternalGear(w, m, Z, phi, split=True):
128  """
129  Create an internal gear
130 
131  w is wirebuilder object (in which the gear will be constructed)
132 
133  if split is True, each profile of a teeth will consist in 2 Bezier
134  curves of degree 3, otherwise it will be made of one Bezier curve
135  of degree 4
136  """
137  # ****** external gear specifications
138  addendum = 0.6 * m # distance from pitch circle to tip circle (ref G.M.Maitra)
139  dedendum = 1.25 * m # pitch circle to root, sets clearance
140  clearance = 0.25 * m
141 
142  # Calculate radii
143  Rpitch = Z * m / 2 # pitch circle radius
144  Rb = Rpitch*cos(phi * pi / 180) # base circle radius
145  Ra = Rpitch - addendum # tip (addendum) circle radius
146  Rroot = Rpitch + dedendum # root circle radius
147  fRad = 1.5 * clearance # fillet radius, max 1.5*clearance
148  Rf = Rroot - clearance # radius at top of fillet (end of profile)
149 
150  # ****** calculate angles (all in radians)
151  pitchAngle = 2 * pi / Z # angle subtended by whole tooth (rads)
152  baseToPitchAngle = genInvolutePolar(Rb, Rpitch)
153  tipToPitchAngle = baseToPitchAngle
154  if (Ra > Rb): # start profile at top of fillet (if its greater)
155  tipToPitchAngle -= genInvolutePolar(Rb, Ra)
156  pitchToFilletAngle = genInvolutePolar(Rb, Rf) - baseToPitchAngle;
157  filletAngle = 1.414*clearance/Rf # // to make fillet tangential to root
158 
159  # ****** generate Higuchi involute approximation
160  fe = 1 # fraction of profile length at end of approx
161  fs = 0.01 # fraction of length offset from base to avoid singularity
162  if (Ra > Rb):
163  fs = (Ra**2 - Rb**2) / (Rf**2 - Rb**2) # offset start to top of fillet
164 
165  if split:
166  # approximate in 2 sections, split 25% along the involute
167  fm = fs + (fe - fs) / 4 # fraction of length at junction (25% along profile)
168  addInv = BezCoeffs(m, Z, phi, 3, fs, fm)
169  dedInv = BezCoeffs(m, Z, phi, 3, fm, fe)
170 
171  # join the 2 sets of coeffs (skip duplicate mid point)
172  invR = addInv + dedInv[1:]
173  else:
174  invR = BezCoeffs(m, Z, phi, 4, fs, fe)
175 
176  # create the back profile of tooth (mirror image)
177  inv = []
178  for i, pt in enumerate(invR):
179  # rotate involute to put center of tooth at y = 0
180  ptx, pty = invR[i] = rotate(pt, pitchAngle / 4 - baseToPitchAngle)
181  # generate the back of tooth profile nodes, flip Y coords
182  inv.append((ptx, -pty))
183 
184  # ****** calculate section junction points R=back of tooth, Next=front of next tooth)
185  #fillet = inv[6] # top of fillet, front of tooth #toCartesian(Rf, -pitchAngle / 4 - pitchToFilletAngle) # top of fillet
186  fillet = [ptx,-pty]
187  tip = toCartesian(Ra, -pitchAngle/4+tipToPitchAngle) # tip, front of tooth
188  tipR = [ tip[0], -tip[1] ]
189  #filletR = [fillet[0], -fillet[1]] # flip to make same point on back of tooth
190  rootR = toCartesian(Rroot, pitchAngle / 4 + pitchToFilletAngle + filletAngle)
191  rootNext = toCartesian(Rroot, 3 * pitchAngle / 4 - pitchToFilletAngle - filletAngle)
192  filletNext = rotate(fillet, pitchAngle) # top of fillet, front of next tooth
193 
194  # Build the shapes using FreeCAD.Part
195  t_inc = 2.0 * pi / float(Z)
196  thetas = [(x * t_inc) for x in range(Z)]
197 
198  w.move(fillet) # start at top of front profile
199 
200 
201  for theta in thetas:
202  w.theta = theta
203  if split:
204  w.curve(inv[5], inv[4], inv[3])
205  w.curve(inv[2], inv[1], inv[0])
206  else:
207  w.curve(*inv[-2::-1])
208 
209  if (Ra < Rb):
210  w.line(tip) # line from fillet up to base circle
211 
212  if split:
213  w.arc(tipR, Ra, 0) # arc across addendum circle
214  else:
215  #w.arc(tipR[-1], Ra, 0) # arc across addendum circle
216  w.arc(tipR, Ra, 0)
217 
218  if (Ra < Rb):
219  w.line(invR[0]) # line down to topof fillet
220 
221  if split:
222  w.curve(invR[1], invR[2], invR[3])
223  w.curve(invR[4], invR[5], invR[6])
224  else:
225  w.curve(*invR[1:])
226 
227  if (rootNext[1] > rootR[1]): # is there a section of root circle between fillets?
228  w.arc(rootR, fRad, 1) # back fillet
229  w.arc(rootNext, Rroot, 0) # root circle arc
230 
231  w.arc(filletNext, fRad, 1)
232 
233 
234  w.close()
235  return w
236 
237 
238 def genInvolutePolar(Rb, R):
239  """returns the involute angle as function of radius R.
240  Rb = base circle radius
241  """
242  return (sqrt(R*R - Rb*Rb) / Rb) - acos(Rb / R)
243 
244 
245 def rotate(pt, rads):
246  "rotate pt by rads radians about origin"
247  sinA = sin(rads)
248  cosA = cos(rads)
249  return (pt[0] * cosA - pt[1] * sinA,
250  pt[0] * sinA + pt[1] * cosA)
251 
252 
253 
254 def toCartesian(radius, angle):
255  "convert polar coords to cartesian"
256  return [radius * cos(angle), radius * sin(angle)]
257 
258 
259 def chebyExpnCoeffs(j, func):
260  N = 50 # a suitably large number N>>p
261  c = 0
262  for k in xrange(1, N + 1):
263  c += func(cos(pi * (k - 0.5) / N)) * cos(pi * j * (k - 0.5) / N)
264  return 2 *c / N
265 
266 
267 def chebyPolyCoeffs(p, func):
268  coeffs = [0]*(p+1)
269  fnCoeff = []
270  T = [coeffs[:] for i in range(p+1)]
271  T[0][0] = 1
272  T[1][1] = 1
273  # now generate the Chebyshev polynomial coefficient using
274  # formula T(k+1) = 2xT(k) - T(k-1) which yields
275  # T = [ [ 1, 0, 0, 0, 0, 0], # T0(x) = +1
276  # [ 0, 1, 0, 0, 0, 0], # T1(x) = 0 +x
277  # [-1, 0, 2, 0, 0, 0], # T2(x) = -1 0 +2xx
278  # [ 0, -3, 0, 4, 0, 0], # T3(x) = 0 -3x 0 +4xxx
279  # [ 1, 0, -8, 0, 8, 0], # T4(x) = +1 0 -8xx 0 +8xxxx
280  # [ 0, 5, 0,-20, 0, 16], # T5(x) = 0 5x 0 -20xxx 0 +16xxxxx
281  # ... ]
282 
283  for k in xrange(1, p):
284  for j in xrange(len(T[k]) - 1):
285  T[k + 1][j + 1] = 2 * T[k][j]
286  for j in xrange(len(T[k - 1])):
287  T[k + 1][j] -= T[k - 1][j]
288 
289  # convert the chebyshev function series into a simple polynomial
290  # and collect like terms, out T polynomial coefficients
291  for k in xrange(p + 1):
292  fnCoeff.append(chebyExpnCoeffs(k, func))
293 
294  for k in xrange(p + 1):
295  for pwr in xrange(p + 1):
296  coeffs[pwr] += fnCoeff[k] * T[k][pwr]
297 
298  coeffs[0] -= fnCoeff[0] / 2 # fix the 0th coeff
299  return coeffs
300 
301 
302 def binom(n, k):
303  coeff = 1
304  for i in xrange(n - k + 1, n + 1):
305  coeff *= i
306 
307  for i in xrange(1, k + 1):
308  coeff /= i
309 
310  return coeff
311 
312 
313 def bezCoeff(i, p, polyCoeffs):
314  '''generate the polynomial coeffs in one go'''
315  return sum(binom(i, j) * polyCoeffs[j] / binom(p, j) for j in range(i+1))
316 
317 
318  # Parameters:
319  # module - sets the size of teeth (see gear design texts)
320  # numTeeth - number of teeth on the gear
321  # pressure angle - angle in degrees, usually 14.5 or 20
322  # order - the order of the Bezier curve to be fitted [3, 4, 5, ..]
323  # fstart - fraction of distance along tooth profile to start
324  # fstop - fraction of distance along profile to stop
325 def BezCoeffs(module, numTeeth, pressureAngle, order, fstart, fstop):
326  Rpitch = module * numTeeth / 2 # pitch circle radius
327  phi = pressureAngle # pressure angle
328  Rb = Rpitch * cos(phi * pi / 180) # base circle radius
329  Ra = Rpitch + module # addendum radius (outer radius)
330  ta = sqrt(Ra * Ra - Rb * Rb) / Rb # involute angle at addendum
331  te = sqrt(fstop) * ta # involute angle, theta, at end of approx
332  ts = sqrt(fstart) * ta # involute angle, theta, at start of approx
333  p = order # order of Bezier approximation
334 
335  def involuteXbez(t):
336  "Equation of involute using the Bezier parameter t as variable"
337  # map t (0 <= t <= 1) onto x (where -1 <= x <= 1)
338  x = t * 2 - 1
339  # map theta (where ts <= theta <= te) from x (-1 <=x <= 1)
340  theta = x * (te - ts) / 2 + (ts + te) / 2
341  return Rb * (cos(theta) + theta * sin(theta))
342 
343  def involuteYbez(t):
344  "Equation of involute using the Bezier parameter t as variable"
345  # map t (0 <= t <= 1) onto x (where -1 <= x <= 1)
346  x = t * 2 - 1
347  # map theta (where ts <= theta <= te) from x (-1 <=x <= 1)
348  theta = x * (te - ts) / 2 + (ts + te) / 2
349  return Rb * (sin(theta) - theta * cos(theta))
350 
351  # calc Bezier coeffs
352  bzCoeffs = []
353  polyCoeffsX = chebyPolyCoeffs(p, involuteXbez)
354  polyCoeffsY = chebyPolyCoeffs(p, involuteYbez)
355  for i in xrange(p + 1):
356  bx = bezCoeff(i, p, polyCoeffsX)
357  by = bezCoeff(i, p, polyCoeffsY)
358  bzCoeffs.append((bx, by))
359  return bzCoeffs
360 
