updated: 6 May 2020

# Heavy Photon Search Toolkit for Reconstruction

The Heavy Photon Search Toolkit for Reconstruction (hpstr) provides an interface to physics data from the HPS experiment saved in the LCIO format and converts it into an ROOT based format. It also provides tools which can be used to analyze the ROOT format of the data.

## Doxygen
You can find the code documentation here: [HPSTR Doxygen](https://jeffersonlab.github.io/hpstr/).

## Installation

Hpstr can be installed on the following operating systems with some adjustments depending on the tool set available for compilation. 

- MacOSX
- Ubuntu 20.4
- Centos7 

### Prerequisites

Hpstr depends on [LCIO](https://github.com/JeffersonLab/hps-lcio) and [ROOT](https://root.cern.ch/)
So a full working installation of those packages is necessary before trying to checkout and install hpstr. 

Python 3 is also required (Python 2 is no longer supported).

### Example: Installation on Ubuntu 20.4 from scratch

Here is given the full set of instructions on how to install hpstr on Ubuntu 20.4 LTS which is running inside a virtual machine. This tutorial gives a working solution for an user that doesn't have an operating system compatible with the hpstr installation to start with. 

#### Install Virtual Box and setup Ubuntu 20.4

1. Download VirtualBox, latest version 6.1.6 https://www.virtualbox.org/
2. Download ubuntu 20.4 LST image. In this tutorial desktop version was used (about 2.5 GB)
3. Create a virtual machine using the ubuntu image that has been downloaded. This is a pretty straightforward step, if you aren't familiar with virtual box just follow: https://www.wikihow.com/Install-Ubuntu-on-VirtualBox. I selected 2GB RAM, 10GB HD dynamically allocated. When the ubuntu installation setup pops up select "normal installation"
4. After successful start of the Ubuntu VM, install the developer tools:

```bash
sudo apt install build-essential
sudo apt install cmake
sudo apt install git
```
5. Enable the copy paste from guest<->host by selecting
(a) Devices-> Shared Clipboard -> Bidirectional
(b) Devices-> Insert Guest Additions CD images -> then install it
(c) Restart the Virtual machine
More infos here: https://stackoverflow.com/questions/22885658/copy-paste-from-mac-to-virtual-box

6. Install X11 dependencies that are needed to install ROOT

```bash
sudo apt-get install libx11-dev
sudo apt-get install libxpm-dev
sudo apt-get install libxft-dev
sudo apt-get install libxext-dev
```

7. Help CMake finding python 
```bash
sudo apt-get install python3-dev
```

8. Prepare an installation work directory:

```bash
cd ~
mkdir sw
cd sw
```

9. Install LCIO 

```bash
cd src
git clone https://github.com/JeffersonLab/hps-lcio.git
cd hps-lcio
mkdir build
mkdir install
cd build
cmake -DCMAKE_INSTALL_PREFIX=../install/ ..
make install
```

It is suggested to make a script file to export this in the path which include (again I'll assume we are working in ~/sw, otherwise modify accordingly):

```bash
export LCIO_DIR=$HOME/sw/hps-lcio/install
export LCIO_INCLUDE_DIRS=$LCIO_DIR/include
export IO_LCIO_LIBRARY=$LCIO_DIR/lib/liblcio.so
export LD_LIBRARY_PATH=$LCIO_DIR/lib:$LD_LIBRARY_PATH

export PATH=$LCIO_DIR/bin:$PATH
```

10. Install ROOT
Get the source file from https://root.cern.ch/content/release-61902
```bash
mkdir root
cd root
mkdir install build
mv <pathTo>/root_v6.19.02.source.tar.gz ./
tar -xzvf root_v6.19.02.source.tar.gz
rm root_v6.19.02.source.tar.gz
cd ../build
cmake -DCMAKE_INSTALL_PREFIX=../install ../root_v6.19/
cmake --build . 
```


## Checkout hpstr

```bash
mkdir hpstr
cd hpstr
mkdir src build install
cd src
git clone git@github.com:JeffersonLab/hpstr.git
cd ..
```

## Compilation

```bash
cd build
cmake3 -DCMAKE_INSTALL_PREFIX=../install/ -DLCIO_DIR=$LCIO_DIR  ../src/hpstr/
make -j4 install
```

NOTE:: On SLAC machines ```cmake3``` is needed to call cmake version 3+, you might just need to call ```cmake``` to call the right version on your machine. 

If you do not want hpstr to use the default python3 executable which it finds in your environment, then supply the full path to the alternate Python installation.

```
-DPython3_Executable=/path/to/some/python3 
```

To compile with debug information, just add -DCMAKE_BUILD_TYPE=Debug to the cmake3 command. 

After compilation it is necessary to source the setup script in the ```intall/bin``` directory by

```bash
source install/bin/setup.sh 
```
The setup script should be automatically generated by CMake.

## Usage

The basic command string to run hpstr is

```
Usage: hpstr [application arguments] {configuration_script.py} [arguments to configuration script]

Options:
  -h, --help            show this help message and exit
  -i inFilename, --inFile=inFilename
                        Input filename.
  -d outDir, --outDir=outDir
                        Specify the output directory.
  -o outFilename, --outFile=outFilename
                        Output filename.
  -t, --isData          Type of lcio ntuple: 1=data, 0=MC
  -y, --year            Select year of the data
  -n, --nevents         Number of events to process
```


where ```<config.py>``` is a config file (which are stored in ```hpstr/processors/config/```), followed by various command line options. Different configuration files, might have specific command line options. Please check each configuration file to check which options are available on top of the common ones. 

Hpstr can both run on LCIO files to produce ROOT ntuples, producing the hpstr event with all the objects needed for analysis, and on ROOT ntuples to produce histograms. This can be setup by using the appropriate configuration file. 

### Ntuples production

The configuration to produce the nominal ntuples from LCIO files is ```recoTuple_cfg.py```. Typical usage is:
```bash
hpstr recoTuple_cfg.py -i <inLcioFile> -o <outROOTFile> -t <1=isData|0=isMC> -y <2016|2019>
```

where the ```-t ``` flag is used to distinguish between data and MC and ```-y``` to distinguish between 2016 and 2019. 
There are alternative configurations that produce different ntuples and the main are listed here:

### SvtBl2D Histograms production
The configuration to produce rawSVThit 2D histogram root files from ntuples is ```anaSvtBl2D_cfg.py```. Typical usage is:
```bash
hpstr anaSvtBl2D_cfg.py -i <inNtupleROOTFile> -o <outROOTFile>
Note that the output histo configurations are constructed in "/analysis/plotconfigs/svt/Svt2DBl.json"
```
### SvtBlFits production
The configuration to produce baseline fits from an svtBl2D file is ```fitBL_cfg.py```. 
To fit baselines, need to locate apv channel thresholds file used on DAQ at time of the run...for example if analyzing run 14683, need to locate
the thresholds file 'svt_014679_thresholds2pt5sig_1pt5sigF5H1.dat', which is available on clonfarm1...
Typical usage is:

```bash
hpstr fitBL_cfg.py -i <inSvtBl2DROOTFile> -o <outROOTFile> -l L<0..6> -thresh <svt_<run>_thresholds.dat
```

To generate baseline database file and/or thresholds file from offline baselines, run analysis script:
Typical usage is:

```python3 $HPSTR_BASE/scripts/hpsmc_evio_to_offline_baselines/offlineBaselineFitAnalysis.py -i <inputFile>.root -o <outputFile>.root -b <input_online_baselines>.dat -threshIN svt_<run>_thresholds.dat -dbo <offline_baselines_db_format>.dat -thresh <offline_thresholds_db_format>.dat
```

#### Kalman / GBL vertex performance comparison ntuples

```bash
hpstr  kalTuple_cfg.py -i <inLcioFile> -o <outROOTFile> -t <1=isData|0=isMC> -y <2016|2019> 
```

### Making Plots

A working example on how to make some plots out of hpstr ntuple is 

```bash
hpstr anaVtxTuple_cfg.py -i /nfs/slac/g/hps3/users/bravo/data/physrun2016/7800/hps_007800.123_v0_4.2_4.4-SNAPSHOT_rereco.root -o hps_007800.123.root -t 1 
```
This example will run the standard vertex selection on a data file (to specify that this file is data one has to use the ```-t``` flag and passing 0 will tell hpstr that we are processing MonteCarlo. Plots will be produced according to the selections specified. 

#### Special plotting configurations

Comparison plots between Kalman and GBL tracks can be made via the following command 
```bash
hpstr  anaKalVtxTuple_cfg.py -i <inLcioFile> -o <outROOTFile> -t <1=isData|0=isMC> -y <2016|2019> -w <"GBL"|"KF">
```
the switch -w is to select the two different track reconstruction types and it's only available for this particular configuration. 

### Bump Hunt Analysis

hpstr also includes the HPS 2019 resonance search analysis functionality. A brief description of the capabilities of this code, and how to use it, is included here.

A resonance search job may be called with the following form:
```
hpstr bhToys_cfg.py -i ${invariantMassDistro.root} -s ${pathToInvariantMassPlot} -d ${outputDirectory} -m ${mass} -p ${backgroundFitPolynomialOrder} -w ${windowSize} -N 0
```
The variables correspond to:
- ```${invariantMassDistro.root}```: A ROOT file containing the invariant mass histogram that is to be searched.
- ```${pathToInvariantMassPlot}```: The path to the invariant mass histogram within the ROOT file.
- ```${outputDirectory}```: The directory in which to output the result ROOT ntuple.
- ```${mass}```: The mass to probe. This must be in units of MeV.
- ```${backgroundFitPolynomialOrder}```: The resonance search uses background fit polynomials of the form 10^(T_n(m)), where T_n(m) is a Chebyshev polynomial of the first kind of order n. This argument specifies n, which may be any value from 0 - 5.
- ```${windowSize}```: The size of the fit window. This is in units of the mass resolution, so a window size of 5 is equivalent to (5 * massResolution(m)).

This will run a test to determine the p-value, signal yield, and other basic values for the resonance search at the specified point. Multiple jobs must be run to analyze multiple points.

While by default, the resonance search code employs a background fit function of the form 10^(T_n(m)). This is configurable using the ```-M ${fitFunctionID}``` command, which allows for the selection of one of four fit functions. The fit function IDs corrspond to:
- (0) Chebyshev Polynomial
- (1) Exponential Chebyshev Polynomial
- (2) Legendre Polynomial
- (3) Exponential Legendre Polynomial

In addition to the above mentioned variables, it is also possible to scale the mass resolution by an arbitrary float by adding the argument ```-r ${scalingFactor}```.

The resonance search code is also capable of performing toy model analysis. The number of toy models to generate and analyze is specified by the argument ```-N ${toyModelCount}```. Toy models will automatically be generated from a fit polynomial one order higher than that specified by ```-p ${backgroundFitPolynomialOrder}```. By default, toy models will be generated with the same number of entries as the input invariant mass histogram. This may be scaled by an integer factor using the argument ```-b ${toyModelScalingFactor}```. Lastly, signal may be injected into toy models. This is done by adding the argument ```--sig ${signalEventCount}```, which will add an integer number of signal events at ${mass} using a Gaussian distribution. If a different distribution is desired, it is also possible to specify it using a histogram. This is done with the arguments ```--sig_file ${signalShapeHistogramFile.root --sig_hist ${signalHistogramPath}```. Note that the signal histogram should have the same binning as the invariant mass histogram.

## Available Scripts 

The sripts folder in the hpstr repo provides a serie of utilities which some or them are described here. 

### Processing multiple files

The script ```run_jobPool.py``` provides a way to process multiple files with hpstr in parallel in parallel threads. 
Here is an example on how to run it

```bash
python3 run_jobPool.py -t hpstr -c <configFile.py>  -i <inDir> -z <isData> -o <outDir> -r <root|slcio>
```

where ```-c``` is used to specify the configurationFile for hpstr, ```-i``` and ```-o``` are for specifying the input and output directory respectively, ```-z``` is to choose between data (=1) and MC simulation (=0) input type, and finally ```-r``` is needed to tell hpstr to run on root or slcio files. The script runs one hpstr process for each file on the input folder matching the required extension and places the results in the output directory. It is also possible to run with extra command flags that will be attached to the hpstr command. For example to pass ```-w GBL``` to the hpstr command, specify ```-e "-wGBL"``` to the submission script. 

## Contributing to Hpstr

Fork the repository first. Open an issue to first discuss what needs to be changed and then open a pull request using the issue number. 
