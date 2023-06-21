# What is it 

This is a study of numerical precision errors in various representations of 2D hyperbolic geometry.
It is generally the best to combine a representation with tiling; the tests take this into
account.

# Representations studied

The following representations are studied:

* **linear**: points in the hyperboloid model; isometries as linear transformation matrices.

* **mixed**: points in the hyperboloid model; isometries using Clifford algebras. (Clifford algebras
  are a generalization of 'quaternions' commonly used in 3D graphics.)

* **clifford**: points are also represented using Clifford algebras, that is, p is represented as
  the isometry u such as u(C0) = p and u does not introduce extra rotations.

* **halfplane (2D)**: points are represented using the half-plane model; isometries are represented using
  SL(2,R).

* **halfspace (3D)**: points are represented using the half-space model; isometries are represented using
  SL(2,C).

* **polar 2D**: points are represented using polar coordinates; isometries need one extra angle.

* **general polar**: like polar 2D, but instead of angles, we use rotated unit vectors and rotation
  matrices; this also makes it work in higher dimension.

## Variations

Both in linear and Clifford representations, there is the correct "normalized" representation;
if the normalized representation is multiplied by some factor x, most formulas still work, 
and for those which do not, it is easy to compute x. This yields the following variations:

* **invariant**: keep the invariant that the points and isometries are normalized
  (that is: output is normalized under the assumption that the input is normalized)

* **careless**: do not care about normalization
  (advantages: some computations are avoided; possible to represent ultra-ideal points in
  linear representations)

* **forced**: normalize the output after every computation
  (might be a good idea for points/isometries close to the center, but generally a bad 
  idea if they are far away -- in that case, the norm generally cannot be computed, but 
  distances and angles still tend to be correct in the invariant computations)

* **weakly forced**: like forced, but do not normalize if the norm could not be computed
  due to precision errors

* **flatten**: instead of normal normalization, make the leading coordinate equal to 1.
  The leading coordinate is the 'timelike' coordinate of linear representations
  of points, and the 'unit' coordinate of Clifford representations.
  (advantage: save memory: H2 represented only 2 coordinates instead of 3; 
  disadvantage: might not represent ultra-ideal points if they would be infinite)

* **binary**: in careless, values may easily explode and cause underflow/overflow; avoid this
  by making the leading coordinate in \[0.5, 2) range (by multiplying by powers of 2, which is
  presumably fast)

Furthermore:
* in linear, matrices can be **fixed** by replacing them by a correct orthogonal matrix close
  to the current computation
* in (non-general) polar, forcing angles into [-pi,pi] may be needed to prevent explosion
* in **improved** polar, one of three variants of the cosine rule can be used, depending on the angle,
  to improve the numerical precision; also even more precise computation to avoid numerical
  precision errors for angles very close to 0 or pi
* in the Clifford representation, the **gyro** variant splits the isometries into 
  the translational part (which is flattened, making it equivalent to the Poincare disk model)
  and the rotational part (for which 'invariant' is used). This fixes the problem 
  with full flattening where rotations by 180° are flattened to infinity. (AFAIK
  Hyperbolica uses roughly this)

## Observations

* except linear, all the methods of representing isometries can only represent
  orientation-preserving ones

* Clifford isometries of H2 is essentially the same as SL(2,R) of halfplane -- it is
  just the change of the basis

* linear/Clifford representations are not that good at representing points close to the
  boundary of the disk (invariant can somewhat tell the distance but flattened cannot); 
  halfplane is better here

# Tests

## test_loop_iso

In this test, for each i, we construct a path in the tiling by always moving to a random
adjacent tile, until we get to a tile i afar; then, we return to the start (also randomly,
may stray further from the path). We compose all the relative tile isometries into T and see
if T(C0) = C0. The score is the first i for which it fails.

Discussion: This makes rep_mixed worse than rep_lorentz.

## test_loop_point

Same as test_loop_iso but we apply the consecutive isometries to point right away.

Discussion: This makes rep_mixed worse than rep_lorentz.

## test_angledist

For each i (skipping some), construct a path outwards in the tiling, compose isometries,
and see if the distance and angle to that tile have been computed correctly.

Discussion: Invariant representations have no problem with this, even if the points obtained are beyond the precision otherwise.

## test_similarity, test_dissimilarity, test_other

For each i, compute the distance between two points in distance i from the starting point. 
The angle between them is very small (test_similarity), close to 180° (test_dissimilarity),
close to 1° (test_other). 

Discussion: Similarity is obviously the most difficult. Halfplane is surprisingly powerful in all cases.

## test_walk

This is essentially walking in a straight line in HyperRogue. After some time, it can be often clearly observed that we
have 'deviated' from the original straight line. This test checks how long we can walk.

We construct an isometry T representing a random direction. In each step, we compose this isometry with a translation (T := T * translate(1/16)).
Whenever the point T * C0 is closer to the center of another tile, we rebase to that new tile.

For a test, we actually do this in parallel with two isometries T0 and T1, where T1 = T0 * translate(1/32). We count the number of steps
until the paths diverge. Numbers over 1000 are not actually that good, 1000+n means that, after n steps, the implementation no longer detects tile
changes. Numbers of 10000 signify that some even weirder problem happened.

Discussion: Since the isometry matrices are always small (constrained to tiles), fixing definitely helps here. Without fixing, T stops
being an isometry (an effect visible in HyperRogue when fixing is disabled).

## test_close

Here we see whether small errors accumulate when moving close to the center. In test i, we move randomly until we reach distance i+1,
after which we return to the start (always reducing the distance). After each return to the start, we check if the representation is
still fine (if not, we restart with the original representation). The number given is the number of errors in 10000 steps.

Discussion: Errors do not appear to accumulate when we simply move close to the start (or rather, they accumulate very slowly).

## test_count

This simply computes the number of numerical operations performed for every geometric operation. Numerical operations are categorized as:
* Addition/subtraction
* Multiplication (multiplication by constant not counted)
* Division (division by constant not counted)
* Functions: exp, log, (a)sin/cos/tan(h), sqrt, inverse sqrt

Geometric operations are:
* spin: return rotation by given angle in given axes
* L0: return translation by given value in axis 0
* L1: return translation by given value in axis 1
* ip: apply isometry to point
* ii: compose isometries
* d0: compute the distance of point from 0
* angle: compute the (2D) angle of point
* inverse: compute the inverse of an isometry
* push: convert a point into a translation

# Implementation notes

Note: the program currently assumes hyperbolic geometry (it was intended to support spherical
geometry but not everywhere the correct handling is implemented).

# Results

## Results on the {7,3} tiling

```
  test_loop_iso
  linear+F  invariant: (17,16,17,16,17,17,17,17,16,17,17,17,17,16,16,17,17,16,17,17)
  linear+F  forced   : (20,19,19,20,20,19,20,20,19,20,20,18,19,19,19,19,20,19,19,19)
  linear+F  weak     : (21,19,20,20,20,19,24,20,19,25,24,18,19,19,19,19,20,19,19,19)
  linear+F  flatten  : (19,19,21,20,20,20,20,20,19,19,19,21,19,20,19,19,19,19,19,20)
  linear+F  careless : (19,19,19,19,20,19,20,20,19,20,18,18,19,18,19,19,19,19,19,19)
  linear+F  binary   : (19,19,19,19,20,19,20,20,19,20,18,18,19,18,19,19,19,19,19,19)
  linear-F  invariant: (17,17,17,18,18,18,21,17,16,17,19,18,17,18,23,17,18,17,19,17)
  linear-F  forced   : (19,19,19,19,20,19,19,20,19,20,20,20,19,19,19,19,19,19,19,19)
  linear-F  weak     : (19,19,20,19,20,19,19,20,19,21,20,20,21,19,19,19,20,19,19,19)
  linear-F  flatten  : (20,19,20,20,21,19,20,18,19,19,20,21,19,21,20,19,19,19,19,20)
  linear-F  careless : (20,19,19,21,19,19,20,17,19,20,19,20,19,19,19,19,19,19,19,20)
  linear-F  binary   : (20,19,19,21,19,19,20,17,19,20,19,20,19,19,19,19,19,19,19,20)
  mixed     invariant: (34,35,34,36,34,35,34,34,36,35,34,35,36,34,35,34,33,35,32,36)
  mixed     forced   : (34,34,34,35,34,36,33,34,36,35,35,36,36,34,35,34,34,35,34,34)
  mixed     weak     : (34,34,34,35,34,36,33,34,36,35,35,36,36,34,35,34,34,35,34,34)
  mixed     flatten  : (20,5,18,13,13,13,4,13,9,29,25,9,36,22,19,30,5,35,14,2)
  mixed     careless : (34,34,34,35,34,34,35,34,36,35,35,36,36,34,35,34,35,35,34,34)
  mixed     binary   : (34,34,34,35,34,34,35,34,36,35,35,36,36,34,35,34,35,35,34,34)
  Clifford  invariant: (32,35,34,36,34,35,34,34,36,33,34,35,36,34,35,34,33,34,32,36)
  Clifford  forced   : (34,34,34,35,34,36,33,34,36,35,35,36,36,34,35,34,34,35,34,34)
  Clifford  weak     : (34,34,34,35,34,36,33,34,36,35,35,36,36,34,35,34,34,35,34,34)
  Clifford  flatten  : (34,34,34,35,34,36,35,34,36,35,35,36,37,34,35,34,36,35,34,34)
  Clifford  careless : (34,34,34,35,34,34,35,34,36,35,35,36,36,34,35,34,35,35,34,34)
  Clifford  binary   : (34,34,34,35,34,34,35,34,36,35,35,36,36,34,35,34,35,35,34,34)
  Clifford  gyro     : (34,34,34,35,34,36,35,34,36,35,35,36,36,34,35,34,35,35,34,34)
  halfplane invariant: (34,34,34,35,34,36,35,34,36,35,35,36,36,34,35,34,35,35,34,34)
  polar     basic    : (34,34,34,35,34,34,33,34,36,35,35,24,36,34,35,34,35,35,34,34)
  polar     improved : (34,34,34,34,33,36,35,33,36,35,35,36,35,34,35,34,34,35,34,35)
  polar     F/F      : (34,36,35,35,33,34,33,33,36,35,35,36,36,34,35,34,35,35,34,34)
  polar     F/T      : (34,34,34,34,33,36,35,33,36,35,35,36,35,34,35,34,34,35,34,35)
  polar     T/F      : (34,34,35,36,34,36,35,34,35,35,35,36,35,34,35,34,35,35,34,36)
  polar     T/T      : (34,36,35,34,34,36,35,34,36,35,35,36,35,34,35,34,34,35,34,35)
  test_loop_point
  linear+F  invariant: (17,16,17,16,17,17,17,17,16,17,17,17,17,16,16,17,17,16,17,17)
  linear+F  forced   : (20,19,19,20,20,19,20,20,19,20,20,18,19,19,19,19,20,19,19,19)
  linear+F  weak     : (21,19,20,20,20,19,24,20,19,25,24,18,19,19,19,19,20,19,19,19)
  linear+F  flatten  : (19,19,21,20,20,20,20,20,19,19,19,21,19,20,19,19,19,19,19,20)
  linear+F  careless : (19,19,19,19,20,19,20,20,19,20,18,18,19,18,19,19,19,19,19,19)
  linear+F  binary   : (19,19,19,19,20,19,20,20,19,20,18,18,19,18,19,19,19,19,19,19)
  linear-F  invariant: (17,17,17,18,18,18,21,17,16,17,19,18,17,18,23,17,18,17,19,17)
  linear-F  forced   : (19,19,19,19,20,19,19,20,19,20,20,20,19,19,19,19,19,19,19,19)
  linear-F  weak     : (19,19,20,19,20,19,19,20,19,21,20,20,21,19,19,19,20,19,19,19)
  linear-F  flatten  : (20,19,20,20,21,19,20,18,19,19,20,21,19,21,20,19,19,19,19,20)
  linear-F  careless : (20,19,19,21,19,19,20,17,19,20,19,20,19,19,19,19,19,19,19,20)
  linear-F  binary   : (20,19,19,21,19,19,20,17,19,20,19,20,19,19,19,19,19,19,19,20)
  mixed     invariant: (18,17,17,19,20,18,19,19,17,17,18,17,17,16,16,18,25,17,17,19)
  mixed     forced   : (19,19,19,19,19,19,19,17,19,20,19,20,19,19,19,20,19,19,20,20)
  mixed     weak     : (19,23,19,19,19,19,21,17,19,23,19,20,24,20,19,20,22,19,23,20)
  mixed     flatten  : (20,19,19,20,19,19,19,18,20,20,19,18,18,19,18,20,19,19,19,19)
  mixed     careless : (19,19,19,19,19,20,19,18,19,19,19,21,19,20,18,19,20,19,19,20)
  mixed     binary   : (19,19,19,19,19,20,19,18,19,19,19,21,19,20,18,19,20,19,19,20)
  Clifford  invariant: (32,34,31,34,33,36,31,35,32,33,32,36,32,34,33,36,33,32,34,34)
  Clifford  forced   : (34,34,34,35,34,36,35,34,35,35,35,36,35,34,35,34,35,35,34,34)
  Clifford  weak     : (34,34,34,35,34,36,35,34,35,35,35,36,35,34,35,34,35,35,34,34)
  Clifford  flatten  : (34,34,34,35,34,36,35,34,36,35,34,36,36,34,35,34,35,35,34,34)
  Clifford  careless : (3,1,3,2,2,2,2,2,2,3,2,2,3,2,2,3,2,2,1,3)
  Clifford  binary   : (34,34,34,35,34,34,35,34,36,35,35,36,35,34,35,34,35,35,34,34)
  Clifford  gyro     : (34,36,34,35,34,36,35,34,36,35,35,36,36,34,35,34,35,35,34,34)
  halfplane invariant: (34,36,34,35,34,36,35,34,36,35,35,36,36,34,35,34,35,35,34,34)
  polar     basic    : (34,34,34,35,34,34,33,34,36,35,35,24,36,34,35,34,35,35,34,34)
  polar     improved : (34,34,34,34,33,36,35,33,36,35,35,36,35,34,35,34,34,35,34,35)
  polar     F/F      : (34,36,35,35,33,34,33,33,36,35,35,36,36,34,35,34,35,35,34,34)
  polar     F/T      : (34,34,34,34,33,36,35,33,36,35,35,36,35,34,35,34,34,35,34,35)
  polar     T/F      : (34,34,35,36,34,36,35,34,35,35,35,36,35,34,35,34,35,35,34,36)
  polar     T/T      : (34,36,35,34,34,36,35,34,36,35,35,36,35,34,35,34,34,35,34,35)
  test_angledist
  linear+F  invariant: (767,767,767)
  linear+F  forced   : (21,21,21)
  linear+F  weak     : (21,21,21)
  linear+F  flatten  : (21,21,21)
  linear+F  careless : (21,21,21)
  linear+F  binary   : (21,21,21)
  linear-F  invariant: (767,767,767)
  linear-F  forced   : (21,21,21)
  linear-F  weak     : (21,21,21)
  linear-F  flatten  : (21,21,21)
  linear-F  careless : (21,21,21)
  linear-F  binary   : (21,21,21)
  mixed     invariant: (767,767,767)
  mixed     forced   : (21,21,21)
  mixed     weak     : (21,21,21)
  mixed     flatten  : (21,21,21)
  mixed     careless : (21,21,21)
  mixed     binary   : (21,21,21)
  Clifford  invariant: (767,767,767)
  Clifford  forced   : (39,39,47)
  Clifford  weak     : (39,39,39)
  Clifford  flatten  : (39,39,39)
  Clifford  careless : (2,3,3)
  Clifford  binary   : (39,47,39)
  Clifford  gyro     : (39,47,39)
  halfplane invariant: (767,767,767)
  polar     basic    : (443,443,443)
  polar     improved : (443,443,443)
  polar     F/F      : (767,767,767)
  polar     F/T      : (767,767,767)
  polar     T/F      : (767,767,767)
  polar     T/T      : (767,767,767)
  test_similarity
  linear+F  invariant: (18,17,17,18,17,18,18,17,18,17,18,18,17,17,17,18,17,17,17,17)
  linear+F  forced   : (19,18,18,18,19,18,18,19,19,18,18,18,19,18,19,19,18,19,19,18)
  linear+F  weak     : (19,18,18,18,19,18,18,19,19,18,18,18,19,18,19,19,18,19,19,18)
  linear+F  flatten  : (18,19,18,18,18,19,19,19,19,18,18,18,19,18,19,19,19,19,18,19)
  linear+F  careless : (19,18,19,18,19,18,18,19,19,18,18,18,19,19,19,19,18,18,19,18)
  linear+F  binary   : (19,18,19,18,19,18,18,19,19,18,18,18,19,19,19,19,18,18,19,18)
  linear-F  invariant: (18,18,19,18,18,18,18,18,18,18,18,19,19,19,18,18,19,18,18,18)
  linear-F  forced   : (18,19,19,19,19,19,18,19,18,19,19,19,19,19,18,19,19,19,19,19)
  linear-F  weak     : (18,19,19,19,19,19,18,19,18,19,19,19,19,19,18,19,19,19,19,19)
  linear-F  flatten  : (19,18,19,19,19,19,19,19,18,19,19,19,19,19,18,19,19,19,19,19)
  linear-F  careless : (18,19,19,19,19,19,19,19,18,19,19,19,19,19,19,19,19,19,19,19)
  linear-F  binary   : (18,19,19,19,19,19,19,19,18,19,19,19,19,19,19,19,19,19,19,19)
  mixed     invariant: (18,19,18,19,18,18,18,18,18,19,19,19,18,19,18,18,18,19,19,18)
  mixed     forced   : (19,19,19,19,19,18,19,19,18,18,18,19,20,19,19,20,19,19,19,19)
  mixed     weak     : (19,19,19,19,19,18,19,19,18,18,19,19,21,20,19,19,19,20,19,20)
  mixed     flatten  : (19,19,19,19,19,19,19,19,20,18,19,19,19,19,20,19,19,19,19,19)
  mixed     careless : (19,20,19,19,19,18,19,19,19,19,19,20,20,19,19,19,19,19,19,19)
  mixed     binary   : (19,20,19,19,19,18,19,19,19,19,19,20,20,19,19,19,19,19,19,19)
  Clifford  invariant: (33,33,33,32,32,34,34,33,32,34,33,33,34,33,33,33,33,34,33,33)
  Clifford  forced   : (36,35,35,35,35,35,37,35,36,36,36,35,35,35,35,37,36,36,35,35)
  Clifford  weak     : (36,35,35,35,35,35,37,35,36,36,36,35,35,35,35,37,36,36,35,35)
  Clifford  flatten  : (35,37,36,35,36,35,35,38,36,37,36,35,37,35,37,38,36,38,35,36)
  Clifford  careless : (37,35,36,35,35,36,35,35,37,35,38,36,38,35,37,36,35,35,35,37)
  Clifford  binary   : (37,35,36,35,35,36,35,35,37,35,38,36,38,35,37,36,35,35,35,37)
  Clifford  gyro     : (37,35,37,37,37,36,35,36,36,36,37,36,35,36,37,37,36,37,37,35)
  halfplane invariant: (35,36,36,36,36,35,35,37,37,37,37,36,35,36,38,37,36,36,37,35)
  polar     basic    : (19,18,18,18,19,18,18,18,18,18,18,18,18,19,18,18,18,18,18,18)
  polar     improved : (37,37,37,37,37,38,38,37,37,36,37,37,38,39,38,37,37,39,37,38)
  polar     F/F      : (19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19)
  polar     F/T      : (35,35,35,35,35,35,35,35,35,35,35,35,35,35,35,35,35,35,35,35)
  polar     T/F      : (19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19)
  polar     T/T      : (35,35,35,35,35,36,35,35,35,36,36,35,35,35,35,35,35,35,35,36)
  test_dissimilarity
  linear+F  invariant: (125,124,147,123,134,130,126,128,123,125,130,130,127,125,125,131,123,124,125,127)
  linear+F  forced   : (7,6,7,7,7,7,8,7,6,7,7,7,7,7,7,7,7,7,7,7)
  linear+F  weak     : (7,7,7,7,9,13,8,7,9,7,9,7,7,7,11,9,7,8,7,7)
  linear+F  flatten  : (7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7)
  linear+F  careless : (7,7,7,7,7,7,7,7,6,7,8,7,7,7,7,7,7,7,7,7)
  linear+F  binary   : (7,7,7,7,7,7,7,7,6,7,8,7,7,7,7,7,7,7,7,7)
  linear-F  invariant: (356,356,356,356,356,356,356,356,356,356,356,356,356,356,356,356,356,356,356,356)
  linear-F  forced   : (10,10,10,10,10,10,10,10,10,9,9,10,10,10,10,10,10,9,10,10)
  linear-F  weak     : (9,15,13,12,11,11,10,9,16,9,10,9,13,10,12,11,10,9,10,9)
  linear-F  flatten  : (9,10,10,10,10,10,9,9,10,10,10,10,10,10,9,10,10,10,10,10)
  linear-F  careless : (10,10,10,9,10,9,10,10,10,10,9,10,10,9,9,10,9,10,10,9)
  linear-F  binary   : (10,10,10,9,10,9,10,10,10,10,9,10,10,9,9,10,9,10,10,9)
  mixed     invariant: (356,356,356,356,356,356,356,356,356,356,356,356,356,356,356,356,356,356,356,356)
  mixed     forced   : (10,9,10,9,10,10,10,9,10,10,10,10,10,10,10,10,10,9,10,10)
  mixed     weak     : (10,9,12,9,9,10,10,11,10,9,12,11,10,9,11,11,11,9,11,10)
  mixed     flatten  : (10,9,10,10,9,10,10,10,10,10,10,10,10,10,9,10,10,10,10,10)
  mixed     careless : (10,10,10,10,10,10,10,10,10,10,10,10,9,9,9,10,10,10,10,9)
  mixed     binary   : (10,10,10,10,10,10,10,10,10,10,10,10,9,9,9,10,10,10,10,9)
  Clifford  invariant: (356,356,356,356,356,356,356,356,356,356,356,356,356,356,356,356,356,356,356,356)
  Clifford  forced   : (18,19,18,18,19,18,19,18,19,19,18,19,18,17,19,19,19,19,19,19)
  Clifford  weak     : (18,18,18,18,19,18,18,18,19,18,18,19,18,18,18,18,19,18,18,19)
  Clifford  flatten  : (18,19,18,19,19,18,18,18,19,19,18,18,19,19,18,18,18,19,19,18)
  Clifford  careless : (19,18,19,18,18,19,19,18,18,18,18,18,19,18,19,19,19,18,19,18)
  Clifford  binary   : (19,18,19,18,18,19,19,18,18,18,18,18,19,18,19,19,19,18,19,18)
  Clifford  gyro     : (18,18,18,19,19,18,19,20,18,18,18,18,18,19,19,19,18,19,18,18)
  halfplane invariant: (35,35,35,35,35,35,35,35,34,36,37,34,35,36,35,35,35,35,36,35)
  polar     basic    : (356,356,356,356,356,356,356,356,356,356,356,356,356,356,356,356,356,356,356,356)
  polar     improved : (356,356,356,356,356,356,356,356,356,356,356,356,356,356,356,356,356,356,356,356)
  polar     F/F      : (356,356,356,356,356,356,356,356,356,356,356,356,356,356,356,356,356,356,356,356)
  polar     F/T      : (356,356,356,356,356,356,356,356,356,356,356,356,356,356,356,356,356,356,356,356)
  polar     T/F      : (356,356,356,356,356,356,356,356,356,356,356,356,356,356,356,356,356,356,356,356)
  polar     T/T      : (356,356,356,356,356,356,356,356,356,356,356,356,356,356,356,356,356,356,356,356)
  test_other
  linear+F  invariant: (97,97,101,107,97,97,126,107,101,101,112,112,99,101,112,131,107,97,97,94)
  linear+F  forced   : (10,10,10,10,10,10,10,10,11,10,10,10,10,11,10,11,10,10,10,10)
  linear+F  weak     : (10,11,10,11,12,13,11,11,13,14,10,10,10,11,11,11,12,10,13,10)
  linear+F  flatten  : (10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,11,10)
  linear+F  careless : (10,11,10,10,10,10,10,10,10,10,10,10,10,11,10,10,10,10,10,10)
  linear+F  binary   : (10,11,10,10,10,10,10,10,10,10,10,10,10,11,10,10,10,10,10,10)
  linear-F  invariant: (356,356,356,356,356,356,356,356,356,356,356,356,356,356,356,356,356,356,356,356)
  linear-F  forced   : (12,12,12,13,13,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12)
  linear-F  weak     : (12,12,12,18,13,12,12,14,14,14,12,12,12,12,13,12,12,12,12,12)
  linear-F  flatten  : (12,12,12,12,12,12,12,12,12,12,13,12,12,12,12,12,12,12,12,12)
  linear-F  careless : (12,12,13,12,12,12,13,12,13,13,12,12,12,12,12,12,12,12,12,12)
  linear-F  binary   : (12,12,13,12,12,12,13,12,13,13,12,12,12,12,12,12,12,12,12,12)
  mixed     invariant: (359,360,359,359,359,359,359,359,359,359,359,359,359,359,360,359,359,359,359,359)
  mixed     forced   : (14,13,14,14,13,13,13,14,13,14,14,14,14,13,14,13,13,14,13,13)
  mixed     weak     : (13,13,14,14,13,13,13,14,13,18,13,16,14,13,15,14,13,14,13,13)
  mixed     flatten  : (13,14,14,14,14,14,13,14,14,14,13,14,13,14,14,13,13,14,14,13)
  mixed     careless : (14,13,14,14,13,14,14,14,14,14,14,14,13,14,13,14,13,14,13,13)
  mixed     binary   : (14,13,14,14,13,14,14,14,14,14,14,14,13,14,13,14,13,14,13,13)
  Clifford  invariant: (361,361,361,361,361,361,361,361,361,361,361,361,361,361,361,361,361,361,361,361)
  Clifford  forced   : (21,21,23,21,21,21,22,21,23,21,22,21,21,21,21,22,21,21,21,22)
  Clifford  weak     : (21,21,23,21,21,21,22,21,23,21,21,21,21,21,21,22,21,21,21,22)
  Clifford  flatten  : (21,21,22,21,22,22,21,23,21,21,22,22,22,22,22,21,21,22,22,22)
  Clifford  careless : (21,22,22,21,21,22,22,23,21,21,21,22,22,21,21,21,22,22,22,23)
  Clifford  binary   : (21,22,22,21,21,22,22,23,21,21,21,22,22,21,21,21,22,22,22,23)
  Clifford  gyro     : (23,23,23,23,24,23,23,24,23,24,23,23,23,23,23,23,23,23,23,23)
  halfplane invariant: (35,35,35,35,36,38,37,35,36,36,37,36,35,36,35,36,36,38,38,35)
  polar     basic    : (356,356,356,356,356,356,356,356,356,356,356,356,356,356,356,356,356,356,356,356)
  polar     improved : (360,360,360,360,360,360,360,360,360,360,360,360,360,360,360,360,360,360,360,360)
  polar     F/F      : (356,356,356,356,356,356,356,356,356,356,356,356,356,356,356,356,356,356,356,356)
  polar     F/T      : (360,360,360,360,360,360,360,360,360,360,360,360,360,360,360,360,360,360,360,360)
  polar     T/F      : (356,356,356,356,356,356,356,356,356,356,356,356,356,356,356,356,356,356,356,356)
  polar     T/T      : (360,360,360,360,360,360,360,360,360,360,360,360,360,360,360,360,360,360,360,360)
  test_walk
  linear+F  invariant: (658,606,622,603,606,608,667,620,615,616,616,619,631,609,613,614,632,635,592,612)
  linear+F  forced   : (632,617,606,617,608,607,607,628,627,632,626,627,596,652,623,639,615,617,615,616)
  linear+F  weak     : (613,626,622,636,606,605,640,594,609,615,592,639,625,607,613,600,635,620,622,604)
  linear+F  flatten  : (605,618,608,625,677,649,612,607,614,649,621,609,602,599,615,645,609,597,597,638)
  linear+F  careless : (617,615,620,608,603,606,654,597,612,598,626,624,601,613,616,609,602,627,601,619)
  linear+F  binary   : (617,615,620,608,603,606,654,597,612,598,626,624,601,613,616,609,602,627,601,619)
  linear-F  invariant: (309,298,341,329,349,304,302,301,292,314,305,310,322,307,314,1315,1312,295,10000,10000)
  linear-F  forced   : (1294,1290,1318,1308,1297,1287,1297,1295,1348,1304,1301,1309,1303,1318,292,1299,1294,1291,1288,1298)
  linear-F  weak     : (298,295,301,289,299,289,287,301,297,294,305,297,316,286,292,291,301,293,303,299)
  linear-F  flatten  : (297,1324,1299,1293,1315,1301,1313,1305,1305,306,1282,1310,1307,1309,1292,1306,1296,1315,1313,1289)
  linear-F  careless : (1305,1310,1299,1298,1308,307,1318,1296,1293,1300,302,1306,1298,1287,1299,1315,1312,290,1290,1298)
  linear-F  binary   : (1305,1310,1299,1298,1308,307,1318,1296,1293,1300,302,1306,1298,1287,1299,1315,1312,290,1290,1298)
  mixed     invariant: (622,634,587,620,607,619,609,612,599,646,607,623,616,590,615,592,637,636,659,613)
  mixed     forced   : (599,666,588,608,607,602,630,671,601,602,618,630,618,601,599,599,614,601,596,617)
  mixed     weak     : (599,666,588,608,607,602,630,671,601,602,618,630,618,601,599,599,614,601,596,617)
  mixed     flatten  : (611,616,607,605,610,603,595,605,593,614,617,593,602,642,610,616,625,593,636,617)
  mixed     careless : (622,634,587,620,607,619,609,612,599,646,607,623,616,590,615,592,637,636,659,613)
  mixed     binary   : (622,634,587,620,607,619,609,612,599,646,607,623,616,590,615,592,637,636,659,613)
  Clifford  invariant: (622,634,587,620,607,619,609,612,599,646,607,623,616,590,615,592,637,636,659,613)
  Clifford  forced   : (599,666,588,608,607,602,630,671,601,602,618,630,618,601,599,599,614,601,596,617)
  Clifford  weak     : (599,666,588,608,607,602,630,671,601,602,618,630,618,601,599,599,614,601,596,617)
  Clifford  flatten  : (611,616,607,605,610,603,595,605,593,614,617,593,602,642,610,616,625,593,636,617)
  Clifford  careless : (622,634,587,620,607,619,609,612,599,646,607,623,616,590,615,592,637,636,659,613)
  Clifford  binary   : (622,634,587,620,607,619,609,612,599,646,607,623,616,590,615,592,637,636,659,613)
  Clifford  gyro     : (600,586,602,621,625,621,625,603,593,630,634,600,586,597,600,609,601,592,617,615)
  halfplane invariant: (600,586,602,621,625,621,625,603,593,630,634,600,586,597,600,609,601,592,617,615)
  polar     basic    : (1055,67,1078,66,72,1050,66,1073,70,1052,66,1070,66,67,71,73,1064,1070,67,66)
  polar     improved : (71,1068,1073,1059,67,55,67,1071,65,1052,1067,1078,67,63,69,1067,57,66,69,1059)
  polar     F/F      : (605,566,605,563,566,583,565,591,578,616,591,568,601,569,584,559,621,579,589,601)
  polar     F/T      : (573,596,633,569,581,590,565,588,590,581,600,614,597,571,595,619,576,573,582,631)
  polar     T/F      : (594,662,662,598,591,686,590,610,593,592,588,588,600,581,598,572,618,578,589,588)
  polar     T/T      : (583,594,601,586,570,601,594,579,585,581,582,614,649,614,674,639,588,580,587,588)
  test_close
  linear+F  invariant: (0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,18,78,130,126,118)
  linear+F  forced   : (0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,11,70,110,118)
  linear+F  weak     : (0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,11,70,110,118)
  linear+F  flatten  : (0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,10,55,99,116)
  linear+F  careless : (0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,15,67,107,113)
  linear+F  binary   : (0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,15,67,107,113)
  linear-F  invariant: (0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,27,89,117,118)
  linear-F  forced   : (0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,19,73,117,117)
  linear-F  weak     : (0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,19,73,117,116)
  linear-F  flatten  : (0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,15,49,103,115)
  linear-F  careless : (0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,17,62,112,115)
  linear-F  binary   : (0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,17,62,112,115)
  mixed     invariant: (0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,2,36,101,117,115)
  mixed     forced   : (0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,21,72,114,117)
  mixed     weak     : (0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,21,72,114,115)
  mixed     flatten  : (0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,20,75,115,116)
  mixed     careless : (0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,17,73,113,112)
  mixed     binary   : (0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,17,73,113,112)
  Clifford  invariant: (0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0)
  Clifford  forced   : (0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0)
  Clifford  weak     : (0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0)
  Clifford  flatten  : (0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0)
  Clifford  careless : (1666,1698,1241,859,666,545,447,378,339,298,262,245,244,207,196,175,170,168,157,144)
  Clifford  binary   : (0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0)
  Clifford  gyro     : (0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0)
  halfplane invariant: (0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0)
  polar     basic    : (0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0)
  polar     improved : (0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0)
  polar     F/F      : (0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0)
  polar     F/T      : (0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0)
  polar     T/F      : (0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0)
  polar     T/T      : (0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0)
  test_count
  linear+F  invariant: (spin(24A 34M 9D 4F) L0(24A 34M 9D 4F) L1(24A 34M 9D 4F) ip(9A 9M) ii(51A 51M 9D) d0(1F) angle(1F) inverse() push(29A 42M 10D 2F))
  linear+F  forced   : (spin(27A 46M 9D 5F) L0(27A 46M 9D 5F) L1(27A 46M 9D 5F) ip(12A 15M 1F) ii(54A 73M 9D 3F) d0(1F) angle(1F) inverse() push(32A 54M 10D 3F))
  linear+F  weak     : (spin(30A 49M 9D 5F) L0(30A 49M 9D 5F) L1(30A 49M 9D 5F) ip(12A 15M 1F) ii(57A 76M 9D 3F) d0(3A 4M 2F) angle(1F) inverse() push(38A 63M 10D 4F))
  linear+F  flatten  : (spin(27A 37M 17D 4F) L0(27A 37M 17D 4F) L1(27A 37M 17D 4F) ip(9A 9M 2D) ii(54A 54M 17D) d0(3A 4M 2F) angle(1F) inverse() push(35A 51M 18D 3F))
  linear+F  careless : (spin(27A 37M 9D 4F) L0(27A 37M 9D 4F) L1(27A 37M 9D 4F) ip(9A 9M) ii(54A 64M 9D 2F) d0(3A 4M 2F) angle(1F) inverse() push(35A 51M 10D 3F))
  linear+F  binary   : (spin(27A 37M 9D 4F) L0(27A 37M 9D 4F) L1(27A 37M 9D 4F) ip(9A 9M) ii(54A 64M 9D 2F) d0(3A 4M 2F) angle(1F) inverse() push(35A 51M 10D 3F))
  linear-F  invariant: (spin(2F) L0(2F) L1(2F) ip(9A 9M) ii(27A 27M) d0(1F) angle(1F) inverse() push(5A 8M 1D))
  linear-F  forced   : (spin(3A 12M 3F) L0(3A 12M 3F) L1(3A 12M 3F) ip(12A 15M 1F) ii(30A 39M 1F) d0(1F) angle(1F) inverse() push(8A 20M 1D 1F))
  linear-F  weak     : (spin(3A 12M 3F) L0(3A 12M 3F) L1(3A 12M 3F) ip(12A 15M 1F) ii(30A 39M 1F) d0(3A 4M 2F) angle(1F) inverse() push(11A 26M 1D 2F))
  linear-F  flatten  : (spin(8D 2F) L0(8D 2F) L1(8D 2F) ip(9A 9M 2D) ii(27A 27M 8D) d0(3A 4M 2F) angle(1F) inverse() push(8A 14M 9D 1F))
  linear-F  careless : (spin(2F) L0(2F) L1(2F) ip(9A 9M) ii(27A 27M) d0(3A 4M 2F) angle(1F) inverse() push(8A 14M 1D 1F))
  linear-F  binary   : (spin(2F) L0(2F) L1(2F) ip(9A 9M) ii(27A 27M) d0(3A 4M 2F) angle(1F) inverse() push(8A 14M 1D 1F))
  mixed     invariant: (spin(2F) L0(2F) L1(2F) ip(17A 24M) ii(12A 16M) d0(1F) angle(1F) inverse() push(5A 7M))
  mixed     forced   : (spin(2F) L0(2F) L1(2F) ip(20A 30M 1F) ii(15A 28M 1F) d0(1F) angle(1F) inverse() push(7A 18M 1F))
  mixed     weak     : (spin(2F) L0(2F) L1(2F) ip(20A 30M 1F) ii(15A 28M 1F) d0(3A 4M 2F) angle(1F) inverse() push(10A 24M 2F))
  mixed     flatten  : (spin(1F) L0(1F) L1(1F) ip(17A 15M 2D) ii(12A 12M) d0(3A 4M 2F) angle(1F) inverse() push(8A 15M 1F))
  mixed     careless : (spin(2F) L0(2F) L1(2F) ip(17A 24M) ii(12A 16M) d0(3A 4M 2F) angle(1F) inverse() push(8A 13M 1F))
  mixed     binary   : (spin(2F) L0(2F) L1(2F) ip(17A 24M) ii(12A 16M) d0(3A 4M 2F) angle(1F) inverse() push(8A 13M 1F))
  Clifford  invariant: (spin(2F) L0(2F) L1(2F) ip(12A 28M 1F) ii(12A 16M) d0(1F) angle(1F) inverse() push())
  Clifford  forced   : (spin(2F) L0(2F) L1(2F) ip(13A 29M 1F) ii(15A 28M 1F) d0(1F) angle(1F) inverse() push())
  Clifford  weak     : (spin(2F) L0(2F) L1(2F) ip(13A 29M 1F) ii(15A 28M 1F) d0(2A 4M 2F) angle(1F) inverse() push())
  Clifford  flatten  : (spin(1F) L0(1F) L1(1F) ip(11A 20M) ii(12A 19M) d0(2A 4M 2F) angle(1F) inverse() push())
  Clifford  careless : (spin(2F) L0(2F) L1(2F) ip(11A 18M) ii(12A 16M) d0(2A 4M 2F) angle(1F) inverse() push())
  Clifford  binary   : (spin(2F) L0(2F) L1(2F) ip(11A 18M) ii(12A 16M) d0(2A 4M 2F) angle(1F) inverse() push())
  Clifford  gyro     : (spin(2F) L0(2F) L1(2F) ip(5A 10M 2D) ii(4A 8M) d0(9A 12M 2D 5F) angle(7A 8M 2D 4F) inverse() push(11A 8M 2D 3F))
  halfplane invariant: (spin(2F) L0(2F) L1(2F) ip(5A 10M 2D) ii(4A 8M) d0(8A 16M 2D 5F) angle(8A 16M 2D 5F) inverse() push(12A 16M 2D 4F))
  polar     basic    : (spin(2F) L0() L1() ip(15A 25M 2D 12F) ii(53A 73M 2D 17F) d0() angle(1F) inverse(4A 4M) push())
  polar     improved : (spin(2F) L0() L1() ip(20A 41M 2D 10F) ii(59A 88M 2D 15F) d0() angle(1F) inverse(4A 4M) push())
  polar     F/F      : (spin() L0() L1() ip(5A 7M 14F) ii(10A 11M 21F) d0() angle() inverse(14A) push())
  polar     F/T      : (spin() L0() L1() ip(5A 7M 14F) ii(14A 8M 18F) d0() angle() inverse(14A) push())
  polar     T/F      : (spin() L0() L1() ip(5A 7M 14F) ii(11A 11M 21F) d0() angle() inverse(2A) push())
  polar     T/T      : (spin() L0() L1() ip(5A 7M 14F) ii(15A 8M 18F) d0() angle() inverse(2A) push())
```

## Results on the {4,3,5} honeycomb

```
  test_loop_iso
  linear+F  invariant: (32,32,34,34,41,39,60,43,32,31,38,36,36,36,41,50,33,37,33,53)
  linear+F  forced   : (22,22,24,25,25,23,25,25,25,26,23,26,24,26,24,25,24,24,24,26)
  linear+F  weak     : (24,25,26,25,28,26,25,25,27,25,23,27,27,27,27,26,24,24,27,26)
  linear+F  flatten  : (22,22,24,25,25,23,27,26,25,26,25,26,24,26,26,26,24,24,24,26)
  linear+F  careless : (26,25,26,25,25,26,27,25,26,26,23,26,27,27,26,26,25,24,27,26)
  linear+F  binary   : (26,25,26,25,25,26,27,25,26,26,23,26,27,27,26,26,25,24,27,26)
  linear-F  invariant: (35,72,27,37,38,43,27,25,29,44,24,47,26,27,28,29,42,35,29,29)
  linear-F  forced   : (26,25,26,25,25,26,25,25,26,27,25,26,27,27,26,27,24,25,24,26)
  linear-F  weak     : (31,25,30,25,26,29,25,25,26,27,25,26,29,27,27,27,24,31,24,26)
  linear-F  flatten  : (22,22,26,25,25,23,25,26,25,26,23,24,24,26,24,27,24,24,24,26)
  linear-F  careless : (22,22,24,25,26,23,25,25,25,26,23,24,27,26,24,26,24,24,27,26)
  linear-F  binary   : (22,22,24,25,26,23,25,25,25,26,23,24,27,26,24,26,24,24,27,26)
  mixed     invariant: (49,47,44,47,42,44,47,45,46,47,45,49,46,50,45,49,42,48,49,45)
  mixed     forced   : (51,50,49,49,47,47,46,47,48,47,48,49,50,50,49,47,52,47,48,49)
  mixed     weak     : (51,50,49,49,47,47,46,47,48,47,48,49,50,50,49,47,52,47,48,49)
  mixed     flatten  : (1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1)
  mixed     careless : (51,50,46,50,47,47,48,47,48,47,48,49,50,50,49,51,52,51,48,49)
  mixed     binary   : (51,50,46,50,47,47,48,47,48,47,48,49,50,50,49,51,52,51,48,49)
  Clifford  invariant: (46,47,44,44,42,44,47,45,45,46,45,50,46,50,45,39,42,44,49,45)
  Clifford  forced   : (51,50,49,49,47,47,46,47,48,47,48,49,50,50,49,47,52,47,48,49)
  Clifford  weak     : (51,50,49,49,47,47,46,47,48,47,48,49,50,50,49,47,52,47,48,49)
  Clifford  flatten  : (55,78,999,58,57,81,55,79,98,999,58,51,66,52,99,51,58,54,88,66)
  Clifford  careless : (51,50,46,50,47,47,48,47,48,47,48,49,50,50,49,51,52,51,48,49)
  Clifford  binary   : (51,50,46,50,47,47,48,47,48,47,48,49,50,50,49,51,52,51,48,49)
  Clifford  gyro     : (51,50,49,50,50,47,48,47,51,47,50,49,50,50,49,51,52,54,50,51)
  halfplane invariant: (51,50,49,50,47,47,48,47,48,47,50,49,50,50,49,51,52,51,48,51)
  polar     basic    : (12,50,46,49,47,47,48,47,48,3,46,49,50,50,47,7,52,3,48,49)
  polar     improved : (49,46,49,50,50,47,48,47,51,47,50,49,50,50,49,51,48,51,48,49)
  test_loop_point
  linear+F  invariant: (999,999,999,999,999,999,999,999,999,999,999,999,999,999,999,999,999,999,999,999)
  linear+F  forced   : (22,22,24,25,25,23,25,25,25,26,23,26,24,26,24,25,24,24,24,26)
  linear+F  weak     : (24,25,26,25,28,26,25,25,27,25,23,27,27,27,27,26,24,24,27,26)
  linear+F  flatten  : (22,22,24,25,25,23,27,26,25,26,25,27,24,26,26,26,24,24,24,26)
  linear+F  careless : (26,25,26,25,25,26,27,25,26,26,23,26,27,27,26,26,25,24,27,26)
  linear+F  binary   : (26,25,26,25,25,26,27,25,26,26,23,26,27,27,26,26,25,24,27,26)
  linear-F  invariant: (35,72,27,37,38,43,27,25,29,44,24,47,26,27,28,29,42,35,29,29)
  linear-F  forced   : (26,25,26,25,25,26,25,25,26,27,25,26,27,27,26,27,24,25,24,26)
  linear-F  weak     : (31,25,30,25,26,29,25,25,26,27,25,26,29,27,27,27,24,31,24,26)
  linear-F  flatten  : (22,22,26,25,25,23,25,26,25,26,23,24,24,26,24,27,24,24,24,26)
  linear-F  careless : (22,22,24,25,26,23,25,25,25,26,23,24,27,26,24,26,24,24,27,26)
  linear-F  binary   : (22,22,24,25,26,23,25,25,25,26,23,24,27,26,24,26,24,24,27,26)
  mixed     invariant: (24,22,22,25,23,25,26,22,24,22,23,25,26,27,24,23,25,22,23,24)
  mixed     forced   : (26,25,26,25,25,23,25,25,26,27,23,26,27,26,26,26,25,24,24,26)
  mixed     weak     : (27,25,30,25,25,23,25,25,26,27,23,26,27,26,26,28,25,24,24,28)
  mixed     flatten  : (1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1)
  mixed     careless : (26,25,24,25,25,26,27,25,26,26,26,26,27,27,26,26,24,25,24,26)
  mixed     binary   : (26,25,24,25,25,26,27,25,26,26,26,26,27,27,26,26,24,25,24,26)
  Clifford  invariant: (46,45,48,44,42,44,45,48,47,48,46,41,45,49,48,39,50,48,51,45)
  Clifford  forced   : (49,50,46,49,47,47,46,45,48,47,45,49,50,50,49,51,52,47,48,49)
  Clifford  weak     : (49,50,46,49,47,47,46,45,48,47,45,49,50,50,49,51,52,47,48,49)
  Clifford  flatten  : (50,48,46,49,47,46,48,47,48,47,45,47,50,50,49,47,42,47,48,49)
  Clifford  careless : (3,3,1,2,2,2,2,2,3,3,3,2,3,2,1,2,3,3,1,3)
  Clifford  binary   : (51,48,49,49,47,47,48,47,48,47,45,49,50,50,49,47,52,47,48,49)
  Clifford  gyro     : (51,50,46,50,50,47,48,48,51,47,50,49,50,50,49,51,52,51,48,49)
  halfplane invariant: (49,50,46,49,47,47,48,47,51,47,50,49,50,50,49,51,52,51,48,49)
  polar     basic    : (12,50,46,49,47,47,48,47,48,3,46,49,50,50,47,7,52,3,48,49)
  polar     improved : (49,46,49,50,50,47,48,47,51,47,50,49,50,50,49,51,48,51,48,49)
  test_angledist
  linear+F  invariant: (999,999,999)
  linear+F  forced   : (26,26,32)
  linear+F  weak     : (26,32,32)
  linear+F  flatten  : (26,26,32)
  linear+F  careless : (26,26,32)
  linear+F  binary   : (26,26,32)
  linear-F  invariant: (999,999,999)
  linear-F  forced   : (26,26,32)
  linear-F  weak     : (32,26,32)
  linear-F  flatten  : (26,26,32)
  linear-F  careless : (26,32,32)
  linear-F  binary   : (26,32,32)
  mixed     invariant: (999,999,999)
  mixed     forced   : (26,26,32)
  mixed     weak     : (26,26,32)
  mixed     flatten  : (2,2,1)
  mixed     careless : (26,26,32)
  mixed     binary   : (26,26,32)
  Clifford  invariant: (999,999,999)
  Clifford  forced   : (57,57,47)
  Clifford  weak     : (69,57,47)
  Clifford  flatten  : (57,57,47)
  Clifford  careless : (5,4,4)
  Clifford  binary   : (57,57,47)
  Clifford  gyro     : (57,69,57)
  halfplane invariant: (999,999,999)
  polar     basic    : (532,532,5)
  polar     improved : (532,532,532)
  test_similarity
  linear+F  invariant: (17,16,16,16,16,16,17,16,17,16,17,17,16,16,16,17,17,16,16,17)
  linear+F  forced   : (17,17,17,18,19,18,18,18,18,17,18,18,17,18,17,18,18,17,17,17)
  linear+F  weak     : (17,17,17,18,18,18,18,18,17,17,18,18,17,18,17,18,17,17,17,17)
  linear+F  flatten  : (17,17,17,18,18,18,18,18,17,17,18,18,17,18,17,18,18,17,17,17)
  linear+F  careless : (17,17,17,18,19,18,17,18,17,17,18,18,17,18,17,17,18,17,17,17)
  linear+F  binary   : (17,17,17,18,19,18,17,18,17,17,18,18,17,18,17,17,18,17,17,17)
  linear-F  invariant: (17,18,18,17,17,17,17,18,17,18,17,18,17,18,17,17,17,17,17,17)
  linear-F  forced   : (18,19,18,18,18,18,18,18,18,18,18,19,19,18,19,19,18,19,18,18)
  linear-F  weak     : (18,19,18,18,18,17,18,18,18,18,19,19,19,18,19,19,18,19,18,18)
  linear-F  flatten  : (19,18,19,18,18,18,18,20,18,19,18,18,20,19,19,19,18,19,18,18)
  linear-F  careless : (19,18,18,18,19,18,18,20,19,18,18,19,19,19,18,18,19,18,18,18)
  linear-F  binary   : (19,18,18,18,19,18,18,20,19,18,18,19,19,19,18,18,19,18,18,18)
  mixed     invariant: (18,19,19,19,19,19,18,19,19,18,18,19,19,19,19,19,19,18,19,18)
  mixed     forced   : (19,19,19,18,18,19,19,18,19,19,19,19,19,19,19,20,19,19,19,19)
  mixed     weak     : (19,19,19,18,18,19,19,18,18,19,19,19,19,20,19,18,19,18,18,19)
  mixed     flatten  : (19,19,18,19,19,18,19,20,18,19,19,20,19,19,20,18,19,19,19,20)
  mixed     careless : (19,19,19,19,19,20,20,19,19,19,19,19,19,19,19,18,19,19,19,19)
  mixed     binary   : (19,19,19,19,19,20,20,19,19,19,19,19,19,19,19,18,19,19,19,19)
  Clifford  invariant: (34,33,34,35,33,34,33,32,34,34,33,34,34,34,34,35,33,34,33,34)
  Clifford  forced   : (35,36,35,37,36,35,35,36,36,36,35,37,35,36,36,36,36,36,36,35)
  Clifford  weak     : (35,36,35,37,36,35,35,36,36,36,35,37,35,36,36,36,36,36,36,35)
  Clifford  flatten  : (35,36,35,35,36,36,37,37,36,36,36,36,36,36,38,35,37,36,37,36)
  Clifford  careless : (36,36,38,36,37,35,35,37,36,36,37,35,37,35,35,35,35,37,36,38)
  Clifford  binary   : (36,36,38,36,37,35,35,37,36,36,37,35,37,35,35,35,35,37,36,38)
  Clifford  gyro     : (36,37,38,36,35,37,35,36,38,36,36,36,36,36,36,37,35,34,35,36)
  halfplane invariant: (36,35,36,36,37,38,38,36,37,36,37,36,35,36,35,37,35,36,36,35)
  polar     basic    : (17,18,18,17,17,17,17,18,18,18,17,17,17,18,17,17,17,17,17,17)
  polar     improved : (34,37,35,36,37,36,34,36,35,35,35,34,36,35,36,36,35,35,35,35)
  test_dissimilarity
  linear+F  invariant: (63,63,64,75,63,63,64,64,64,64,63,63,73,66,63,63,76,74,64,64)
  linear+F  forced   : (6,7,7,7,7,7,7,7,7,7,7,7,6,7,7,7,7,7,7,6)
  linear+F  weak     : (7,9,8,8,7,8,8,7,13,8,7,8,6,7,7,8,8,7,8,7)
  linear+F  flatten  : (8,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,6,7,7,7)
  linear+F  careless : (7,7,6,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,6)
  linear+F  binary   : (7,7,6,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,6)
  linear-F  invariant: (356,356,356,356,356,356,356,356,356,356,356,356,356,356,356,356,356,356,356,356)
  linear-F  forced   : (10,10,10,10,10,10,10,10,10,10,10,9,9,10,9,10,10,9,10,9)
  linear-F  weak     : (12,9,9,14,10,9,9,11,11,11,11,13,9,12,10,13,9,10,15,10)
  linear-F  flatten  : (10,10,10,9,9,9,10,10,10,10,10,10,10,9,9,10,10,10,10,10)
  linear-F  careless : (10,9,9,10,9,10,9,10,9,10,10,10,10,10,9,9,9,10,10,9)
  linear-F  binary   : (10,9,9,10,9,10,9,10,9,10,10,10,10,10,9,9,9,10,10,9)
  mixed     invariant: (356,356,356,356,356,356,356,356,356,356,356,356,356,356,356,356,356,356,356,356)
  mixed     forced   : (10,10,10,10,10,10,10,10,9,10,10,10,10,10,9,9,9,10,9,10)
  mixed     weak     : (10,16,11,15,9,12,10,9,10,14,10,10,10,11,11,11,11,10,11,11)
  mixed     flatten  : (9,10,9,10,10,10,9,10,9,10,10,10,10,9,10,9,10,9,9,9)
  mixed     careless : (10,9,9,10,10,10,10,9,10,10,9,10,10,10,10,10,9,9,9,10)
  mixed     binary   : (10,9,9,10,10,10,10,9,10,10,9,10,10,10,10,10,9,9,9,10)
  Clifford  invariant: (356,356,356,356,356,356,356,356,356,356,356,356,356,356,356,356,356,356,356,356)
  Clifford  forced   : (18,17,17,19,20,17,18,18,19,19,18,18,17,19,18,18,19,18,19,18)
  Clifford  weak     : (19,17,17,18,19,17,18,18,17,18,18,18,17,18,18,17,18,18,19,19)
  Clifford  flatten  : (19,18,18,18,19,18,18,18,18,18,18,19,19,18,18,19,18,19,18,18)
  Clifford  careless : (18,18,18,18,18,18,18,19,19,18,18,18,18,18,18,19,18,19,19,18)
  Clifford  binary   : (18,18,18,18,18,18,18,19,19,18,18,18,18,18,18,19,18,19,19,18)
  Clifford  gyro     : (18,17,18,19,19,19,18,19,18,19,18,18,19,19,19,19,18,18,19,17)
  halfplane invariant: (34,35,36,35,35,35,35,34,35,35,36,35,35,37,36,37,35,35,34,35)
  polar     basic    : (356,356,356,356,356,356,356,356,356,356,356,356,356,356,356,356,356,356,356,356)
  polar     improved : (356,356,356,356,356,356,356,356,356,356,356,356,356,356,356,356,356,356,356,356)
  test_other
  linear+F  invariant: (73,50,65,68,65,50,68,50,64,67,65,63,50,66,63,50,68,50,64,67)
  linear+F  forced   : (10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10)
  linear+F  weak     : (10,10,11,10,11,17,10,12,13,12,11,11,10,10,10,11,11,16,10,14)
  linear+F  flatten  : (10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10)
  linear+F  careless : (10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10)
  linear+F  binary   : (10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10)
  linear-F  invariant: (356,356,356,356,356,356,356,356,356,356,356,356,356,356,356,356,356,356,356,356)
  linear-F  forced   : (12,12,13,12,12,12,12,12,12,12,12,13,12,12,12,12,12,12,12,12)
  linear-F  weak     : (12,12,12,12,13,12,14,13,12,15,12,14,16,12,15,12,12,16,12,12)
  linear-F  flatten  : (12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12)
  linear-F  careless : (13,12,13,12,12,12,12,12,12,12,12,12,12,12,12,12,13,12,12,12)
  linear-F  binary   : (13,12,13,12,12,12,12,12,12,12,12,12,12,12,12,12,13,12,12,12)
  mixed     invariant: (359,359,359,359,359,359,359,359,359,359,359,359,359,359,359,359,359,359,359,359)
  mixed     forced   : (14,14,14,13,14,13,14,13,14,14,13,14,14,13,13,14,13,14,14,13)
  mixed     weak     : (15,13,14,13,14,14,16,13,16,14,14,14,13,13,13,14,13,13,14,15)
  mixed     flatten  : (14,13,13,14,13,14,14,14,13,13,14,14,14,14,13,14,14,13,14,14)
  mixed     careless : (14,14,13,13,13,13,13,13,14,14,13,14,14,13,13,14,13,13,14,13)
  mixed     binary   : (14,14,13,13,13,13,13,13,14,14,13,14,14,13,13,14,13,13,14,13)
  Clifford  invariant: (361,361,361,361,361,361,361,361,361,361,361,361,361,361,361,361,361,361,361,361)
  Clifford  forced   : (22,21,21,21,22,21,22,22,22,22,22,21,21,22,22,22,22,21,21,22)
  Clifford  weak     : (22,21,21,21,22,21,22,22,22,22,22,21,21,22,22,22,22,21,21,22)
  Clifford  flatten  : (22,22,22,22,22,22,22,22,22,22,22,21,21,22,22,22,22,22,22,22)
  Clifford  careless : (22,21,22,22,22,23,21,21,21,22,22,21,22,21,22,22,22,22,22,22)
  Clifford  binary   : (22,21,22,22,22,23,21,21,21,22,22,21,22,21,22,22,22,22,22,22)
  Clifford  gyro     : (23,24,23,23,23,24,23,23,23,23,23,23,23,23,23,23,23,24,23,23)
  halfplane invariant: (35,35,35,35,35,35,35,35,35,36,37,35,35,35,36,36,35,35,35,35)
  polar     basic    : (356,356,356,356,356,356,356,356,356,356,356,356,356,356,356,356,356,356,356,356)
  polar     improved : (360,360,360,360,360,360,360,360,360,360,360,360,360,360,360,360,360,360,360,360)
  test_walk
  linear+F  invariant: (614,576,600,601,581,579,609,600,596,565,596,610,583,588,595,606,591,592,579,614)
  linear+F  forced   : (598,653,583,603,584,597,581,582,618,568,608,621,586,596,636,604,584,619,585,586)
  linear+F  weak     : (606,604,616,593,604,592,583,611,598,576,597,579,603,584,575,591,584,587,600,591)
  linear+F  flatten  : (619,599,580,615,608,615,600,593,601,577,615,603,579,590,635,589,589,586,560,589)
  linear+F  careless : (620,596,575,593,602,591,576,587,603,574,596,604,589,615,599,623,586,620,612,602)
  linear+F  binary   : (620,596,575,593,602,591,576,587,603,574,596,604,589,615,599,623,586,620,612,602)
  linear-F  invariant: (293,291,289,304,1295,307,285,302,290,297,1311,289,320,292,305,289,285,2132,2137,284)
  linear-F  forced   : (1292,288,1303,1298,301,1288,1285,1290,297,288,304,298,293,299,1300,1291,1294,1276,1281,1288)
  linear-F  weak     : (288,288,284,300,301,291,286,294,297,288,304,288,293,299,300,286,291,10000,281,293)
  linear-F  flatten  : (1292,1297,1303,1311,1292,1285,1296,1297,10000,1299,1321,298,1311,1288,293,1315,1287,1280,1283,1286)
  linear-F  careless : (1288,1292,1288,303,1295,1295,1285,1303,291,1290,1310,289,1321,1290,1298,1289,1284,1276,1282,1287)
  linear-F  binary   : (1288,1292,1288,303,1295,1295,1285,1303,291,1290,1310,289,1321,1290,1298,1289,1284,1276,1282,1287)
  mixed     invariant: (592,582,592,572,580,598,589,597,594,567,595,600,605,591,602,611,596,580,589,587)
  mixed     forced   : (592,587,597,579,582,627,592,603,603,565,596,576,589,591,596,606,592,594,591,592)
  mixed     weak     : (592,587,597,579,582,627,592,603,603,565,596,576,589,591,596,606,592,594,591,592)
  mixed     flatten  : (28,363,52,303,296,336,292,316,288,287,308,298,298,298,310,314,293,371,353,324)
  mixed     careless : (592,582,592,572,580,598,589,597,594,567,595,600,605,591,602,611,596,580,589,587)
  mixed     binary   : (592,582,592,572,580,598,589,597,594,567,595,600,605,591,602,611,596,580,589,587)
  Clifford  invariant: (592,582,592,572,580,598,589,597,594,567,595,600,605,591,602,611,596,580,589,587)
  Clifford  forced   : (592,587,597,579,582,627,592,603,603,565,596,576,589,591,596,606,592,594,591,592)
  Clifford  weak     : (592,587,597,579,582,627,592,603,603,565,596,576,589,591,596,606,592,594,591,592)
  Clifford  flatten  : (614,604,595,598,570,595,589,587,599,573,599,585,603,602,634,602,596,600,584,596)
  Clifford  careless : (592,582,592,572,580,598,589,597,594,567,595,600,605,591,602,611,596,580,589,587)
  Clifford  binary   : (592,582,592,572,580,598,589,597,594,567,595,600,605,591,602,611,596,580,589,587)
  Clifford  gyro     : (601,581,606,584,595,604,609,589,582,596,601,601,579,601,619,589,588,583,588,587)
  halfplane invariant: (576,614,594,596,580,604,594,600,610,566,593,584,603,597,606,608,588,579,575,590)
  polar     basic    : (25,1047,69,1045,1045,1033,59,1065,1041,67,1064,56,1044,1034,70,68,55,53,53,55)
  polar     improved : (55,58,70,1045,1045,1057,56,1065,59,68,1065,55,1044,1034,1070,66,1056,53,52,58)
  test_close
  linear+F  invariant: (0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,4,22)
  linear+F  forced   : (0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0)
  linear+F  weak     : (0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0)
  linear+F  flatten  : (0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0)
  linear+F  careless : (0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0)
  linear+F  binary   : (0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0)
  linear-F  invariant: (0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,8)
  linear-F  forced   : (0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0)
  linear-F  weak     : (0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0)
  linear-F  flatten  : (0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0)
  linear-F  careless : (0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1)
  linear-F  binary   : (0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1)
  mixed     invariant: (0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1)
  mixed     forced   : (0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1)
  mixed     weak     : (0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1)
  mixed     flatten  : (5000,2716,1760,1229,986,774,667,584,500,460,430,365,344,299,286,283,257,237,231,234)
  mixed     careless : (0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0)
  mixed     binary   : (0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0)
  Clifford  invariant: (0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0)
  Clifford  forced   : (0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0)
  Clifford  weak     : (0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0)
  Clifford  flatten  : (0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0)
  Clifford  careless : (1666,1539,1427,1047,817,653,547,483,418,381,350,308,283,260,247,230,212,202,188,183)
  Clifford  binary   : (0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0)
  Clifford  gyro     : (0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0)
  halfplane invariant: (0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0)
  polar     basic    : (0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0)
  polar     improved : (0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0)
  test_count
  linear+F  invariant: (spin(60A 78M 24D 5F) L0(60A 78M 24D 5F) L1(60A 78M 24D 5F) ip(16A 16M) ii(124A 142M 24D 3F) d0(1F) angle(1F) inverse() push(70A 96M 25D 3F))
  linear+F  forced   : (spin(64A 98M 24D 6F) L0(64A 98M 24D 6F) L1(64A 98M 24D 6F) ip(20A 24M 1F) ii(128A 162M 24D 4F) d0(1F) angle(1F) inverse() push(74A 116M 25D 4F))
  linear+F  weak     : (spin(68A 102M 24D 6F) L0(68A 102M 24D 6F) L1(68A 102M 24D 6F) ip(20A 24M 1F) ii(132A 166M 24D 4F) d0(4A 5M 2F) angle(1F) inverse() push(82A 128M 25D 5F))
  linear+F  flatten  : (spin(64A 82M 39D 5F) L0(64A 82M 39D 5F) L1(64A 82M 39D 5F) ip(16A 16M 3D) ii(128A 146M 39D 3F) d0(4A 5M 2F) angle(1F) inverse() push(78A 108M 40D 4F))
  linear+F  careless : (spin(64A 82M 24D 5F) L0(64A 82M 24D 5F) L1(64A 82M 24D 5F) ip(16A 16M) ii(128A 146M 24D 3F) d0(4A 5M 2F) angle(1F) inverse() push(78A 108M 25D 4F))
  linear+F  binary   : (spin(64A 82M 24D 5F) L0(64A 82M 24D 5F) L1(64A 82M 24D 5F) ip(16A 16M) ii(128A 146M 24D 3F) d0(4A 5M 2F) angle(1F) inverse() push(78A 108M 25D 4F))
  linear-F  invariant: (spin(2F) L0(2F) L1(2F) ip(16A 16M) ii(64A 64M) d0(1F) angle(1F) inverse() push(10A 18M 1D))
  linear-F  forced   : (spin(4A 20M 3F) L0(4A 20M 3F) L1(4A 20M 3F) ip(20A 24M 1F) ii(68A 84M 1F) d0(1F) angle(1F) inverse() push(14A 38M 1D 1F))
  linear-F  weak     : (spin(4A 20M 3F) L0(4A 20M 3F) L1(4A 20M 3F) ip(20A 24M 1F) ii(68A 84M 1F) d0(4A 5M 2F) angle(1F) inverse() push(18A 46M 1D 2F))
  linear-F  flatten  : (spin(15D 2F) L0(15D 2F) L1(15D 2F) ip(16A 16M 3D) ii(64A 64M 15D) d0(4A 5M 2F) angle(1F) inverse() push(14A 26M 16D 1F))
  linear-F  careless : (spin(2F) L0(2F) L1(2F) ip(16A 16M) ii(64A 64M) d0(4A 5M 2F) angle(1F) inverse() push(14A 26M 1D 1F))
  linear-F  binary   : (spin(2F) L0(2F) L1(2F) ip(16A 16M) ii(64A 64M) d0(4A 5M 2F) angle(1F) inverse() push(14A 26M 1D 1F))
  mixed     invariant: (spin(2F) L0(2F) L1(2F) ip(52A 64M) ii(56A 64M) d0(1F) angle(1F) inverse() push(7A 10M))
  mixed     forced   : (spin(2F) L0(2F) L1(2F) ip(56A 72M 1F) ii(63A 88M 1F) d0(1F) angle(1F) inverse() push(10A 30M 1F))
  mixed     weak     : (spin(2F) L0(2F) L1(2F) ip(56A 72M 1F) ii(63A 88M 1F) d0(4A 5M 2F) angle(1F) inverse() push(14A 38M 2F))
  mixed     flatten  : (spin(1F) L0(1F) L1(1F) ip(52A 49M 3D) ii(56A 56M) d0(4A 5M 2F) angle(1F) inverse() push(11A 21M 1F))
  mixed     careless : (spin(2F) L0(2F) L1(2F) ip(52A 64M) ii(56A 64M) d0(4A 5M 2F) angle(1F) inverse() push(11A 18M 1F))
  mixed     binary   : (spin(2F) L0(2F) L1(2F) ip(52A 64M) ii(56A 64M) d0(4A 5M 2F) angle(1F) inverse() push(11A 18M 1F))
  Clifford  invariant: (spin(2F) L0(2F) L1(2F) ip(39A 68M 1F) ii(56A 64M) d0(1F) angle(1F) inverse() push())
  Clifford  forced   : (spin(2F) L0(2F) L1(2F) ip(39A 68M 1F) ii(63A 88M 1F) d0(1F) angle(1F) inverse() push())
  Clifford  weak     : (spin(2F) L0(2F) L1(2F) ip(39A 68M 1F) ii(63A 88M 1F) d0(3A 5M 2F) angle(1F) inverse() push())
  Clifford  flatten  : (spin(1F) L0(1F) L1(1F) ip(36A 51M) ii(56A 71M) d0(3A 5M 2F) angle(1F) inverse() push())
  Clifford  careless : (spin(2F) L0(2F) L1(2F) ip(36A 48M) ii(56A 64M) d0(3A 5M 2F) angle(1F) inverse() push())
  Clifford  binary   : (spin(2F) L0(2F) L1(2F) ip(36A 48M) ii(56A 64M) d0(3A 5M 2F) angle(1F) inverse() push())
  Clifford  gyro     : (spin(8A 1F) L0(8A 1F) L1(8A 1F) ip(23A 38M 1D) ii(24A 32M) d0(23A 26M 1D 5F) angle(20A 21M 1D 4F) inverse() push(28A 21M 1D 3F))
  halfplane invariant: (spin(8A 2F) L0(8A 2F) L1(8A 2F) ip(23A 38M 1D) ii(24A 32M) d0(23A 38M 1D 5F) angle(23A 38M 1D 5F) inverse() push(31A 38M 1D 4F))
  polar     basic    : (spin(2F) L0() L1() ip(24A 35M 3D 12F) ii(114A 135M 3D 17F) d0() angle(1F) inverse(9A 9M) push())
  polar     improved : (spin(2F) L0() L1() ip(29A 51M 3D 10F) ii(120A 150M 3D 15F) d0() angle(1F) inverse(9A 9M) push())
```