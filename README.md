# FALCON #

<br>
<p align="center"><img src="imgs/logot.png"
alt="FALCON" width="350" height="200" border="0" /></p>
<br>

<p align="justify">
<b>FALCON is an alignment-free unsupervised system to measure a similarity top of multiple reads according to a database</b>. The <b>machine learning system</b> can be used, for example, to <b>classify metagenomic samples</b>. The core of the method is based on the <b>relative algorithmic entropy</b>, a notion that uses model-freezing and exclusive information from a reference, allowing to use <b>much lower computational resources</b>. Moreover, it uses <b>variable multi-threading</b>, without multiplying the memory for each thread, being able to <b>run efficiently from a powerful server to a common laptop</b>. To measure the similarity, the system will build <b>multiple finite-context (Markovian) models</b> that at the end of the reference sequence will be kept frozen. The target reads will then be measured using a mixture of the frozen models. The <b>mixture</b> estimates the probabilities assuming dependency from model performance, and thus, it will allow to adapt the usage of the models according to the nature of the target sequence. Furthermore, it uses <b>fault tolerant (substitution edits) Markovian models</b> that bridge the gap between context sizes. <b>Several running modes</b> are available for different hardware and speed specifications. <b>The system is able to automatically learn</b> to measure similarity, whose properties are characteristics of the <b>Artificial Intelligence</b> field.
</p>

## INSTALLATION ##

Cmake is needed for installation (http://www.cmake.org/) for systems not using Linux. You can download it directly from http://www.cmake.org/cmake/resources/software.html or use an appropriate packet manager. In the following instructions we show the procedure to install FALCON:
<pre>
git clone https://github.com/pratas/falcon.git
cd falcon/src/
cmake .
make
cp FALCON ../../
cd ../../
</pre>
Alternatively to git use wget:
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
git clone https://github.com/pratas/falcon.git
cd falcon/src/
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

## DEMO ##

Search for the top 10 similar virus in Chimpanzee chromosome 7:
<pre>
cp scripts/downloadViruses.pl .
perl downloadViruses.pl
wget  --trust-server-names -q ftp://ftp.ncbi.nlm.nih.gov/genomes/Pan_troglodytes/CHR_18/ptr_ref_Pan_troglodytes-2.1.4_chr18.fa.gz -O PT18.fa.gz
gunzip PT18.fa.gz
./FALCON -v -n 4 -c 20 -t 10 -l 15 PT18.fa viruses.fa
</pre>
It will use less than 3.5 GB of RAM memory and about 1 minute (in a common laptop) to run the FALCON.

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
Usage: FALCON [OPTION]... [FILE1] [FILE2]                                  
Machine learning system that classifies metagenomic samples.                      
                                                                         
Non-mandatory arguments:                                                 
                                                                         
  -h                       give this help,                               
  -V                       display version number,                       
  -v                       verbose mode (more information),              
  -s                       show compression levels,                       
  -l &#60level&#62               compression level [1;44],                     
  -p &#60sample&#62              subsampling (default: 1),                     
  -t &#60top&#62                 top of similarity (default: 20),                            
  -n &#60nThreads&#62            number of threads (default: 2),                            
  -x &#60FILE&#62                similarity top filename,                      
                                                                         
Mandatory arguments:                                                     
                                                                         
  [FILE1]                  metagenomic file (FASTA or FASTQ),              
  [FILE2]                  base file (FASTA or Multi-FASTA).             
                                                                         
Report issues to &#60{pratas,ap,pjf,jmr}@ua.pt&#62.
</pre>
All the parameters can be better explained trough the following table:

| Parameters          | Meaning                                                     |
|---------------------|:------------------------------------------------------------|
| -h                  | It will print the parameters menu (help menu)                                        |
| -V                  | It will print the FALCON version number, license type and authors.    |
| -v                  | It will print progress information.    |
| -s                  | It will show pre-defined running levels/modes.    |
| -l &#60;level&#62;    | It will use the selected running levels/modes.    |
| -p &#60;sample&#62;   | If FALCON is using a single model it will sample (or use) only this periodic value of bases.    |
| -t &#60;top&#62;      | It will create a top with this size.              |
| -n &#60;nThreads&#62; | It will use multiple-threading. The time to accomplish the task will be much lower, without use more RAM memory.                   |
| -x &#60;FILE&#62;     | Output top filename. |
| [FILE]           | The metagenomic filename (direct from the NGS sequencing platform). Possible file formats: FASTQ, multi-FASTA, FASTA or sequence [ACGTN]. |
| [FILE]           | The BASE filename (e.g. virus or bacteria database). Possible file formats: FASTA, multi-FASTA or sequence [ACGTN]. There are several scripts, on directory scripts, to download several databases. |

## CITATION ##

On using this software/method please cite the url: https://github.com/pratas/falcon

## ISSUES ##

For any issue let us know at [issues link](https://github.com/pratas/falcon/issues).

## LICENSE ##

GPL v2.

For more information:
<pre>http://www.gnu.org/licenses/gpl-2.0.html</pre>
