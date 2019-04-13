[![Build Status](https://travis-ci.org/pratas/falcon.svg?branch=master)](https://travis-ci.org/pratas/falcon)
[![Codacy Badge](https://api.codacy.com/project/badge/Grade/873d4398da914abab8ae5c2a124e3d94)](https://www.codacy.com/app/pratas/falcon?utm_source=github.com&amp;utm_medium=referral&amp;utm_content=pratas/falcon&amp;utm_campaign=Badge_Grade)
[![License: GPL v3](https://img.shields.io/badge/License-GPL%20v3-blue.svg)](LICENSE)

<p align="center"><img src="imgs/logoTrans.png"
alt="FALCON" width="204" height="204" border="0" /></p>

<p align="justify">
<b>FALCON is an ultra-fast method to infer metagenomic composition of sequenced reads</b>. FALCON measures <b>similarity between any FASTQ</b> file (or FASTA), independently from the size, <b>against any multi-FASTA database</b>, such as the entire set of complete genomes from the NCBI.</p>

<p align="justify">
The core of the method is based on <b>relative data compression</b>. FALCON uses <b>variable multi-threading</b>, without multiplying the memory for each thread, being able to <b>run efficiently in a common laptop</b>.</p>

<p align="justify">
The tool is also able to <b>identify locally where, in each reference sequence, the similarity occur</b>. FALCON provides programs to <b>filter the local results (FALCON-FILTER)</b> and <b>to visualize the results (FALCON-EYE)</b>. <b>Several running modes</b> are available for different hardware and speed specifications. 
</p>

[![Install and Demo Video](imgs/demo.png)](https://www.youtube.com/watch?v=eLqXE2ghFNk)

<br>

## 1. INSTALLATION ##

### A. First option: with [Conda](https://conda.io/miniconda) ###

```
conda install -c maxibor falcon
```

### B. Second option: manual installation ###

<pre>
git clone https://github.com/pratas/falcon.git
cd falcon/src/
cmake .
make
cp FALCON ../../
cp FALCON-FILTER ../../
cp FALCON-EYE ../../
cd ../../
</pre>
[Cmake](http://www.cmake.org/) is needed for installation. The download it directly available from (http://www.cmake.org/cmake/resources/software.html).

## 2. DEMO ##

Search for the top 10 similar viruses in sample reads that we provide in folder test:
<pre>
cd test
gunzip reads.fq.gz
gunzip VDB.fa.gz
./FALCON -v -F -t 10 -l 47 reads.fq VDB.fa
</pre>
It will identify Zaire Ebolavirus in the samples.

## 3. BUILD A REFERENCE DATABASE ## 

<p align="justify">
An example of a viral reference database (FASTA) can be downloaded from <a href="http://sweet.ua.pt/pratas/datasets/VDB.fa.gz">here</a>. With this example, you only need to uncompress it, namely through: gunzip VDB.fa.gz, and use it in FALCON along with the FASTQ reads.
</p>


## 4. USAGE ##

To see the possible options of FALCON type
<pre>
./FALCON
</pre>
or
<pre>
./FALCON -h
</pre>
These will print the following options:
```
Usage: FALCON [OPTION]... [FILE1] [FILE2]                                
A tool to infer metagenomic sample composition.      
                                                                         
Non-mandatory arguments:                                                 
                                                                         
  -h                       give this help,                               
  -F                       force mode (overwrites top file),             
  -V                       display version number,                       
  -v                       verbose mode (more information),              
  -Z                       database local similarity,                    
  -s                       show compression levels,                      
  -l <level>               compression level [1;47],                    
  -p <sample>              subsampling (default: 1),                    
  -t <top>                 top of similarity (default: 20),              
  -n <nThreads>            number of threads (default: 2),              
  -x <FILE>                similarity top filename,                      
  -y <FILE>                local similarities filename,                  
                                                                         
Mandatory arguments:                                                     
                                                                         
  [FILE1]                  metagenomic filename (FASTA or FASTQ),        
  [FILE2]                  database filename (FASTA or Multi-FASTA).     
```

### 4.1 Local detection

<b>For local interactions detection and visualization</b> the package provides <b>FALCON-FILTER</b> and <b>FALCON-EYE</b>.

#### 4.1.1 Filtering

To see the possible options of FALCON-FILTER type
<pre>
./FALCON-FILTER
</pre>
or
<pre>
./FALCON-FILTER -h
</pre>
These will print the following options:

```
Usage: FALCON-FILTER [OPTION]... [FILE]                                  
Filter and segment FALCON output.                                        
                                                                         
Non-mandatory arguments:                                                 
                                                                         
  -h                       give this help,                               
  -F                       force mode (overwrites top file),             
  -V                       display version number,                       
  -v                       verbose mode (more information),              
  -s  <size>               filter window size,                           
  -w  <type>               filter window type,                           
  -x  <sampling>           filter window sampling,                       
  -sl <lower>              similarity lower bound,                       
  -su <upper>              similarity upper bound,                       
  -dl <lower>              size lower bound,                             
  -du <upper>              size upper bound,                             
  -t  <threshold>          threshold [0;2.0],                            
  -o  <FILE>               output filename,                              
                                                                         
Mandatory arguments:                                                     
                                                                         
  [FILE]                   profile filename (from FALCON).
```

#### 4.1.2 Visualization

To see the possible options of FALCON-EYE type
<pre>
./FALCON-EYE
</pre>
or
<pre>
./FALCON-EYE -h
</pre>
These will print the following options:

```
Usage: FALCON-EYE [OPTION]... [FILE]                                     
Visualize FALCON-FILTER output.                                          
                                                                         
Non-mandatory arguments:                                                 
                                                                         
  -h                       give this help,                               
  -F                       force mode (overwrites top file),             
  -V                       display version number,                       
  -v                       verbose mode (more information),              
  -w  <width>              square width (for each value),                
  -s  <ispace>             square inter-space (between each value),      
  -i  <indexs>             color index start,                            
  -r  <indexr>             color index rotations,                        
  -u  <hue>                color hue,                                    
  -sl <lower>              similarity lower bound,                       
  -su <upper>              similarity upper bound,                       
  -dl <lower>              size lower bound,                             
  -du <upper>              size upper bound,                             
  -bg                      show only the best of group,                  
  -g  <color>              color gamma,                                  
  -e  <size>               enlarge painted regions,                      
  -ss                      do NOT show global scale,                     
  -sn                      do NOT show names,                            
  -o  <FILE>               output image filename,                        
                                                                         
Mandatory arguments:                                                     
                                                                         
  [FILE]                   profile filename (from FALCON-FILTER).
```

## 5. COMMON USE ##

Create the following bash script:
<pre>
#!/bin/bash
./FALCON -v -n 4 -t 200 -F -Z -l 47 -c 20 -y complexity.com $1 $2
./FALCON-FILTER -v -F -t 0.5 -o positions.pos complexity.com
./FALCON-EYE -v -F -o draw.map positions.pos
</pre>
Name it Run.sh, then run it using:
<pre>
chmod +x Run.sh
./Run.sh Eagle.fna virus.fna
</pre>
Eagle.fna and virus.fna are only two examples.
See folder examples for more.

## 6. CITATION ##

On using this software/method please cite:

D. Pratas, M. Hosseini, G. Grilo, A. J. Pinho, R. M. Silva, T. Caetano, J. Carneiro, F. Pereira. (2018). Metagenomic Composition Analysis of an Ancient Sequenced Polar Bear Jawbone from Svalbard. Genes, 9(9), 445.

D. Pratas, A. J. Pinho, R. M. Silva, J. M. O. S. Rodrigues, M. Hosseini, T. Caetano, P. J. S. G. Ferreira. FALCON-meta: a method to infer metagenomic composition of ancient DNA. BioRxiv preprint, 2018.

## 7. ISSUES ##

For any issue let us know at [issues link](https://github.com/pratas/falcon/issues).

## 8. LICENSE ##

GPL v3.

For more information see LICENSE file or visit
<pre>http://www.gnu.org/licenses/gpl-3.0.html</pre>
