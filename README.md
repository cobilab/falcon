# FALCON #

<br>
<p align="center"><img src="/logo.png" 
alt="FALCON" width="180" height="180" border="0" /></p>
<br>

FALCON is an alignment-free unsupervised tool to measure a similarity top of multiple reads according to a database.

## INSTALLATION ##

Cmake is needed for installation (http://www.cmake.org/). You can download it directly from http://www.cmake.org/cmake/resources/software.html or use an appropriate packet manager. In the following instructions we show the procedure to install smash-global:
<pre>
wget https://github.com/pratas/falcon/archive/master.zip
unzip master.zip
cd falcon-master/src
cmake .
make
cp FALCON ../../
cd ../../
</pre>
or alternatively to cmake, for Linux, use the following:
<pre>
wget https://github.com/pratas/falcon/archive/master.zip
unzip master.zip
cd falcon-master/src
cp Makefile.linux Makefile
make
cp FALCON ../../
cd ../../
</pre>
This will create a binary file: 
<pre>
FALCON 
</pre>
to measure the similarity between sequences.

## USAGE ##

To see the possible options type
<pre>
./FALCON
</pre>
or
<pre>
./FALCON -h
</pre>
These will print the following options:
<pre>
<p>
Usage: FALCON [OPTION]... [FILE] [FILE]                                  
Measure a top similarity between genomic sequences.                      
                                                                         
Non-mandatory arguments:                                                 
                                                                         
  -h                       give this help,                               
  -V                       display version number,                       
  -v                       verbose mode (more information),              
  -s                       how compression levels,                       
  -l &#60level&#62               compression level [1;30],                     
  -p &#60sample&#62              subsampling (default: 1),                     
  -t &#60top&#62                 top of similarity,                            
  -n &#60nThreads&#62            number of threads,                            
  -x &#60FILE&#62                similarity top filename,                      
                                                                         
Mandatory arguments:                                                     
                                                                         
  [FILE]                   sequenced file (FASTA or FASTQ),              
  [FILE]                   base file (FASTA or Multi-FASTA).             
                                                                         
Report issues to &#60{pratas,ap,pjf,jmr}@ua.pt&#62.
</pre>

## CITATION ##

On using this software/method please cite:
paper: to submit..

## ISSUES ##

For any issue let us know at [issues link](https://github.com/pratas/falcon/issues).

## LICENSE ##

GPL v2.

For more information:
<pre>http://www.gnu.org/licenses/gpl-2.0.html</pre>

