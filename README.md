
Overview
=========
A very efficient implemenation of the the Maximal Overlap Discrete Wavelet Tranform (MODWT).  See D. B. Percival and A. T. Walden (2000), Wavelet Methods for Time Series Analysis. Cambridge, England: Cambridge University Press.  This is not the usual discrete wavelet transform found in, for example, gsl but an extended set of algorithms designed to overcome some problems with the usual discrete wavelet transform.  Developed with Scott Kuehn.

Dependencies
=============
boost headers (http://www.boost.org/)

Build
======
cd src/
make
../bin/modwt --help
