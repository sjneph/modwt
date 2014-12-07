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
doc/ has an html document to open in your browser (same information as shown below) 
bin/modwt --help includes all option arguments  


<p>The Maximal Overlap Discrete Wavelet Transform (MODWT) library is written to be as efficient in RAM and time requirements as possible with particular emphasis on RAM.  The application utilizes the library in the most efficient way allowing us to scale to the whole genome level.
</p>
<a name="Design_Intentions"></a><h3>Design Intentions</h3>
<a name="Library"></a><h4>Library</h4>
<ul><li>Make it fast and memory efficient, with particular emphasis on RAM requirements.
</li><li>Build as a generic library API that can work with any number of different data types, such as simple numeric, BED, WIG, etc.  A generic API may be used in any number of ways in any number of applications.  The application discussed here does NOT utilize the full features of the library API, and is only a single example of how an application may be built from the library components.

</li><li>Make computing any type of MODWT wavelet values independent of the level/scale requested in terms of RAM requirements.
</li></ul>
<a name="Application:_wavelets"></a><h4>Application: <i>modwt</i></h4>
<ul><li>Build a wrapper around the most useful features of the library and expose as a command-line tool
</li><li>Use the library in the most efficient ways possible, even if the application itself becomes <i>slightly</i> cumbersome (see Output)
</li></ul>
<p><br />
</p>
<hr/>
<a name="General_Usage"></a><h3>General Usage</h3>

<p><b>NOTE</b>  <code>modwt --help</code> shows a lot of useful information.  It includes all available filters, boundary conditions and more.
</p><p><code>
modwt<br />
[--boundary &lt;string = periodic&gt;]<br />
[--filter &lt;string = LA8&gt;]<br />
[--help]<br />

[--level &lt;integer = 4&gt;]<br />
[--operation &lt;string = smooth&gt;]<br />
[--prefix &lt;string = ""&gt;]<br />
[--to-stdout]<br />
&lt;file-name&gt;<br />
</code>

</p><p><b>Where</b>
</p>
<a name="--boundary_may_be"></a><h4>--boundary may be</h4>
<ul><li>periodic [default]
</li><li>reflected
</li></ul>
<a name="--filter_may_be"></a><h4>--filter may be</h4>
<ul><li>haar

</li><li>d4, d6, d8, d10, d12, d14, d16, d18, d20 (daubechies)
</li><li>la8, la10, la12, la14, la16, la18, la20 (least asymmetric) [la8 by default]
</li><li>bl14, bl18, bl20 (best localized)
</li><li>c6, c12, c18, c24, c30 (coiflet)
</li></ul>
<a name="--level"></a><h4>--level</h4>
<ul><li>is the number of levels the program will sweep through [4 by default]
</li></ul>
<a name="--operation_may_be"></a><h4>--operation may be</h4>

<ul><li>all
</li><li>details
</li><li>mra
</li><li>scale (coefficients)
</li><li>smooth [default]
</li><li>wave (coefficients)
</li><li>wave-scale (coefficients)
</li></ul>
<a name="--prefix"></a><h4>--prefix</h4>
<ul><li>may be anything you want as a prefix to all output files generated.  This may not be used with --to-stdout.
</li></ul>
<a name="--to-stdout"></a><h4>--to-stdout</h4>

<ul><li>only available when --operation set to <i>smooth</i> or <i>scale</i>
</li><li>may not be used with --prefix
</li></ul>
<p><br />
Option names are NOT case sensitive<br />
Values passed to --boundary, --filter or --operation are NOT case sensitive
</p><p><br />
</p>
<hr/>
<a name="Output"></a><h3>Output</h3>
<p>File names produced from the application (not the library) are of the form:
</p>
<ul><li>details.i&nbsp;: i = 1..level
</li><li>scaling-coefficients.level
</li><li>smoothing.level

</li><li>wavelet-coefficients.i&nbsp;: i = 1..level
</li></ul>
<p>Any <code>--prefix</code> specified by the end user precedes each name shown above.<br />
Not all of these files are produced unless --operation is set to ALL
</p>
<hr/>

<a name="Open_Issues.2C_Notes_and_Related_Items"></a><h3>Open Issues, Notes and Related Items</h3>

<ul><li>Only MODWT and related items are available from the library right now.  See D. B. Percival and A. T. Walden (2000), Wavelet Methods for Time Series Analysis. Cambridge, England: Cambridge University Press.
</li><li>We did not expose the capability to feed files back into the program to recalculate the original series.  The library does have this capability.
</li><li>Files are spit out in the current working directory (cwd) when not using --to-stdout nor --prefix.
</li></ul>
<p>
<hr/>
</p>
