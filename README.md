# FALCON #

<br>
<p align="center"><img src="/logo2.png" 
alt="FALCON" width="180" height="180" border="0" /></p>
<br>

FALCON is an alignment-free unsupervised tool to measure and visualise similarity, given a top defined, of multiple reads according to a reference sequence.

## INSTALLATION ##

Cmake is needed for installation (http://www.cmake.org/). You can download it directly from http://www.cmake.org/cmake/resources/software.html or use an appropriate packet manager. In the following instructions we show the procedure to install smash-global:
<pre>
wget https://github.com/pratas/falcon/archive/master.zip
unzip master.zip
cd falcon-master/src
cmake .
make
cp FALCON-map ../../
cp FALCON-visual ../../
cd ../../
</pre>
This will create two binary files: 
<pre>
FALCON-map 
</pre>
to measure the similarity between sequences and
<pre>
FALCON-visual
</pre>
to visualise the similarity in a heatmap scheme.

## USAGE ##

To see the possible options type
<pre>
./FALCON-map
./FALCON-visual
</pre>
or
<pre>
./FALCON-map -h
./FALCON-visual -h
</pre>
These will print the parameters that are needed to run both tools.

## CITATION ##

On using this software/method please cite:

paper: to submit..

## ISSUES ##

For any issue let us know at [issues link](https://github.com/pratas/falcon/issues).

## LICENSE ##

GPL v2.

For more information:
<pre>http://www.gnu.org/licenses/gpl-2.0.html</pre>

