## Scalable Maximal Overlap Discrete Wavelet Tranform (MODWT) ##
> Shane Neph and Scott Kuehn


Overview
=========
A very efficient implemenation of the the Maximal Overlap Discrete Wavelet Tranform (MODWT).  See D. B. Percival and A. T. Walden (2000), Wavelet Methods for Time Series Analysis. Cambridge, England: Cambridge University Press.  This is not the usual discrete wavelet transform found in, for example, gsl but an extended set of algorithms designed to overcome some problems with the usual discrete wavelet transform.  

See http://faculty.washington.edu/dbp/PDFFILES/4-Lund-A4.pdf for an overview and comparison to the regular discrete transform.  

Dependencies  
=============  
boost headers (http://www.boost.org/)  

Build  
======  
make -C src/  
bin/modwt --help  

Documentation  
==============  
doc/ has an html document to open in your browser  
bin/modwt --help includes all option arguments  
