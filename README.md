[![License: GPL v3](https://img.shields.io/badge/License-GPL%20v3-blue.svg)](LICENSE)

<p align="center"><img src="imgs/logoTrans.png"
alt="FALCON" width="204" height="204" border="0" /></p>

<p align="justify">
<b>FALCON is an ultra-fast method to infer metagenomic composition of sequenced reads</b>. FALCON measures <b>similarity between any FASTQ</b> file (or FASTA), independently from the size, <b>against any multi-FASTA database</b>, such as the entire set of complete genomes from the NCBI. FALCON supports single reads, paired-end reads, and compositions of both. It has been tested in many plataforms, such as Illumina MySeq, HiSeq, Novaseq, IonTorrent.</p>

<p align="justify">
FALCON is efficient to detect the presence and authenticate a given species in the FASTQ reads.
The core of the method is based on <b>relative data compression</b>. FALCON uses <b>variable multi-threading</b>, without multiplying the memory for each thread, being able to <b>run efficiently in a common laptop</b>.</p>

<p align="justify">
The tool is also able to <b>identify locally where, in each reference sequence, the similarity occur</b>. FALCON provides programs to <b>filter the local results (FALCON-filter)</b> and <b>to visualize the results (FALCON-filter-visual)</b>. Also for database inter-similarity analysis (FALCON-inter) and respective visualization (FALCON-inter-visual).
</p>

<p align="justify">Find additional information <a href="https://www.biorxiv.org/content/biorxiv/suppl/2018/02/27/267179.DC1/267179-1.pdf">here</a>.</p>

<br>

[![Install and Demo Video](imgs/demo.png)](https://www.youtube.com/watch?v=eLqXE2ghFNk)

<br>

## 1. Installation ##

### 1.1 Automatic installation with [Conda](https://conda.io/miniconda) ###

```
conda install -c cobilab falcon --yes
```

### 1.2 Manual installation ###

```
git clone https://github.com/cobilab/falcon.git
cd falcon/src/
cmake .
make
cp FALCON ../../
cp FALCON-filter ../../
cp FALCON-filter-visual ../../
cp FALCON-inter  ../../
cp FALCON-inter-visual ../../
cd ../../
```
[Cmake](http://www.cmake.org/) is needed for installation. 

## 2. Demo ##

Search for the top 15 similar viruses in sample reads that we provide in folder test:
```
cd test
gunzip reads.fq.gz
gunzip VDB.fa.gz
./FALCON -v -F -t 15 -l 47 -x top.txt reads.fq VDB.fa
```
It will identify Zaire Ebolavirus in the samples (top.txt) according to the following image

<p align="center"><img src="imgs/top.png"
alt="Top" width="604" border="0" /></p>

## 3. Building a reference database ## 

### 3.1 Build the latest NCBI viral database

An example of building a reference database from NCBI:
```
wget ftp://ftp.ncbi.nlm.nih.gov/genomes/genbank/viral/assembly_summary.txt
awk -F '\t' '{if($12=="Complete Genome") print $20}' assembly_summary.txt > ASCG.txt
mkdir -p GB_DB_VIRAL
mkdir -p GB_DB_VIRAL_CDS
mkdir -p GB_DB_VIRAL_RNA
cat ASCG.txt | xargs -I{} -n1 -P8 wget -P GB_DB_VIRAL {}/*_genomic.fna.gz
mv GB_DB_VIRAL/*_cds_from_genomic.fna.gz GB_DB_VIRAL_CDS/
mv GB_DB_VIRAL/*_rna_from_genomic.fna.gz GB_DB_VIRAL_RNA/
zcat GB_DB_VIRAL/*.fna.g > VDB.fa
```

For building reference databases for multiple domains/kingdoms (bacterial, fungi, protozoa, plant, etc), use:
```
https://raw.githubusercontent.com/cobilab/gto/master/scripts/gto_build_dbs.sh
```

### 3.2 Download an existing database ###

<p align="justify">
An already reference viral database is available <a href="http://sweet.ua.pt/pratas/datasets/VDB.fa.gz">here</a>. With this example, you only need to uncompress it, namely through: gunzip VDB.fa.gz, and use it in FALCON along with the FASTQ reads.
</p>

## 4. Usage ##

The FALCON package includes the following binaries:

* <b>FALCON</b>: metagenomic composition analysis;
* <b>FALCON-filter</b>: local interations - localization;
* <b>FALCON-filter-visual</b>: visualization of global and local similarities;
* <b>FALCON-inter</b>: inter-similarity between database genomes;
* <b>FALCON-inter-visual</b>: visualization of inter-similarities.

### 4.1 Metagenomic composition analysis ###

To see the possible options of FALCON type
```
./FALCON
```
or
``` 
./FALCON -h
```
These will print the following options:
```
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

  [FILE1]:[FILE2]:...      metagenomic filename (FASTQ),                 
                           Use ":" for splitting files.                
                                                                         
  [FILE]                   database filename (Multi-FASTA).   
```

### 4.2 Local detection ###

For local interactions detection and visualization the FALCON package provides <b>FALCON-filter</b> and <b>FALCON-filter-visual</b>.

#### 4.2.1 Filtering ####

To see the possible options of FALCON-filter type

``` 
./FALCON-filter
```
or
```
./FALCON-filter -h
```
These will print the following options:

```
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

#### 4.2.2 Visualization ####

To see the possible options of FALCON-filter-visual type
```
./FALCON-filter-visual
```
or
```
./FALCON-filter-visual -h
```
These will print the following options:

```
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
                                                                         
  [FILE]                   profile filename (from FALCON-filter).
```

### 4.3 Database inter-similarity ###

#### 4.3.1 Mapping inter-similarity ####

To see the possible options of FALCON-inter type
```
./FALCON-inter
```
or
```
./FALCON-inter -h
```
These will print the following options:

```
Non-mandatory arguments:                                           
                                                                         
  -h                   give this help,                               
  -V                   display version number,                       
  -v                   verbose mode (more information),              
  -s                   how compression levels,                       
  -l <level>           compression level [1;30],                     
  -n <nThreads>        number of threads,                            
  -x <FILE>            similarity matrix filename,                   
  -o <FILE>            labels filename,                              
                                                                         
Mandatory arguments:                                               
                                                                         
  <FILE>:<FILE>:<...>  input files (last arguments).                 
                       Use ":" for file splitting.                 
```

#### 4.3.2 Inter-similarity visualization ####

To see the possible options of FALCON-inter type
```
./FALCON-inter-visual
```
or
```
./FALCON-inter-visual -h
```
These will print the following options:

```
Non-mandatory arguments:                                           
                                                                         
  -h             give this help,                                     
  -V             display version number,                             
  -v             verbose mode (more information),                    
  -w             square width (for each value),                      
  -a             square inter-space (between each value),            
  -s             index color start,                                  
  -r             index color rotations,                              
  -u             color hue,                                          
  -g             color gamma,                                        
  -l <FILE>      labels filename,                                    
  -x <FILE>      heatmap filename,                                   
                                                                       
Mandatory arguments:                                               
                                                                         
  <FILE>         input matrix file (from FALCON-inter).           
```

<p align="justify">Find additional information <a href="https://www.biorxiv.org/content/biorxiv/suppl/2018/02/27/267179.DC1/267179-1.pdf">here</a>.</p>

## 5. Common use ##

Create the following bash script:

```
#!/bin/bash
./FALCON -v -n 4 -t 200 -F -Z -l 47 -c 20 -y complexity.com $1 $2
./FALCON-filter -v -F -t 0.5 -o positions.pos complexity.com
./FALCON-filter-visual -v -F -o draw.map positions.pos
```
Name it FALCON-meta.sh and give run access
```
chmod +x FALCON-meta.sh
```
The, run FALCON:
```
./FALCON-meta.sh reads1.fastq:reads2.fastq VDB.fa
```
reads1.fastq, reads2.fastq and VDB.fa is only an example.
For more, see folder examples.

## 6. Citation ##

On using this software/method please cite:

```
D. Pratas, M. Hosseini, G. Grilo, A. J. Pinho, R. M. Silva, T. Caetano, J. Carneiro, F. Pereira. (2018).
Metagenomic Composition Analysis of an Ancient Sequenced Polar Bear Jawbone from Svalbard. 
Genes, 9(9), 445.
```
Bibtex:
<sup>
```
@article{Pratas-2018a,
  title={Metagenomic Composition Analysis of an Ancient Sequenced Polar Bear Jawbone from Svalbard},
  author={D. Pratas, M. Hosseini, G. Grilo, A. J. Pinho, R. M. Silva, T. Caetano, J. Carneiro, F. Pereira},
  journal={Genes},
  volume={9},
  number={9},
  pages={445},
  year={2018}
}
```
</sup>

## 7. Issues ##

For any issue let us know at [issues link](https://github.com/cobilab/falcon/issues).

## 8. License ##

GPL v3.

For more information see LICENSE file or visit
<pre>http://www.gnu.org/licenses/gpl-3.0.html</pre>
