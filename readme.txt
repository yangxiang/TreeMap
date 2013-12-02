TreeMap Program

This program is used for paper "Answering exact distance queries on real-world graphs with bounded performance guarantees", Yang Xiang, The VLDB Journal, In press. DOI: http://dx.doi.org/10.1007/s00778-013-0338-6

Software License Agreement: You may use or modify this computer program for research purposes, provided that you properly cite our paper in publication. This computer program is provided on an as is basis and there is no guarantee on the program nor additional support offered. Neither the author(s) nor their institute(s) is liable under any circumstances. This program archive (including this license agreement) may be updated without further notice.

Please note that if you would like to use or redistribute the datasets included in this package, you may need to follow their license agreements, and/or contact their owners for permissions. Please refer to Readme-DataSources.txt (in "basic_version\datasets") for sources of the datasets used in the basic version, and SNAP License.txt (in "hybrid_version\datasets\VeryLarge") for the license agreement for the datasets used in the hybrid version.

The following of this README file describes how to run the program. 

This archive contains the program in source code form as well as the makefiles and running scripts.  It is divided into two parts: basic_version and hybrid_version.

The basic_version is the basic implementation of TreeMap-F and TreeMap-C which are used in Sections 8.1 and 8.2. The hybrid_version is described and used in Section 8.3. The two versions are independent and can run separately. 

To run the program, please take the following steps:
(1) upload the program to a Linux machine with PBS installed.
(2) Unzip datasets.zip in both basic_version and hybrid_version.
(3) Go to either basic_version or hybrid version, type make.
(4) running the program using qsub, for example
	qsub script_file_TreeMap_HighwayCentric_amaze

Please note:
(1) To run all the jobs in basic_version except for TEDI_BIG_bay and TEDI_BIG_NA, you may use batch_real_HighwayCentric_TEDI.sh
(2) Running script_file_TreeMap_VeryLarge_cit-Patents_DLS needs about 40GB free memory.


